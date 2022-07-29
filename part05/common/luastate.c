#include "luastate.h"
#include "luamem.h"
#include "../vm/luagc.h"
#include "../vm/luavm.h"
#include "luastring.h"
#include "luatable.h"
#include "../compiler/lualexer.h"
#include "../vm/luafunc.h"

typedef struct LX {
    lu_byte extra_[LUA_EXTRASPACE];
    lua_State l;
} LX;

typedef struct LG {
    LX l;
    global_State g;
} LG;

static void stack_init(struct lua_State *L)
{
    L->stack = (StkId)luaM_realloc(L, NULL, 0, LUA_STACKSIZE * sizeof(TValue));
    L->stack_size = LUA_STACKSIZE;
    L->stack_last = L->stack + LUA_STACKSIZE - LUA_EXTRASTACK;
    L->previous = NULL;
    L->status = LUA_OK;
    L->errorjmp = NULL;
    L->top = L->stack;
    L->errorfunc = 0;

    int i;
    for (i = 0; i < L->stack_size; i++)
	setnilvalue(L->stack + i);

    L->top++;

    L->ci = &L->base_ci;
    L->ci->func = L->stack;
    L->ci->top = L->stack + LUA_MINSTACK;
    L->ci->next = L->ci->previous = NULL;
}

static void init_registry(struct lua_State *L)
{
    struct global_State *g = G(L);
    struct Table *t = luaH_new(L);
    gcvalue(&g->l_registry) = obj2gco(t);
    g->l_registry.tt_ = LUA_TTABLE;
    luaH_resize(L, t, 2, 0);

    setgco(&t->array[LUA_MAINTHREADIDX], obj2gco(g->mainthread));
    setgco(&t->array[LUA_GLOBALTBLIDX], obj2gco(luaH_new(L)));
}

#define addbuff(b, t, p) do { \
    memcpy(b, t, sizeof(t)); \
    p += sizeof(t); \
} while (0)

static unsigned int makeseed(struct lua_State *L)
{
    char buff[4 * sizeof(size_t)];
    unsigned int h = time(NULL);
    int p = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
    addbuff(buff, L, p);
    addbuff(buff, &h, p);
    addbuff(buff, luaO_nilobject, p);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    addbuff(buff, &lua_newstate, p);
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

    return luaS_hash(L, buff, p, h);
}
struct lua_State *lua_newstate(lua_Alloc alloc, void *ud)
{
    struct global_State *g;
    struct lua_State *L;

    struct LG *lg = (struct LG *)(*alloc)(ud, NULL, LUA_TTHREAD, sizeof(*lg));
    if (!lg)
	return NULL;

    g = &lg->g;
    g->ud = ud;
    g->frealloc = alloc;
    g->panic = NULL;

    L = &lg->l.l;
    L->tt_ = LUA_TTHREAD;
    L->nci = 0;
    G(L) = g;
    g->mainthread = L;

    // gc init
    g->gcstate = GCSpause;
    g->currentwhite = bitmask(WHITE0BIT);
    g->totalbytes = sizeof(LG);
    g->allgc = NULL;
    g->fixgc = NULL;
    g->sweepgc = NULL;
    g->gray = NULL;
    g->grayagain = NULL;
    g->GCdebt = 0;
    g->GCmemtrav = 0;
    g->GCestimate = 0;
    g->GCstepmul = LUA_GCSTEPMUL;
    g->seed = makeseed(L);

    L->marked = luaC_white(g);
    L->gclist = NULL;

    stack_init(L);
    luaS_init(L);
    init_registry(L);
    luaX_init(L);

    return L;
}

#define fromstate(L) (cast(LX *, cast(lu_byte *, (L)) - offsetof(LX, l)))

static void free_stack(struct lua_State *L)
{
    global_State *g = G(L);
    (*g->frealloc)(g->ud, L->stack, sizeof(TValue), 0);
    L->stack = L->stack_last = L->top = NULL;
    L->stack_size = 0;
}

void lua_close(lua_State *L)
{
    struct global_State *g = G(L);
    struct lua_State *L1 = g->mainthread; // only mainthread can be close

    luaC_freeallobjects(L);

    struct CallInfo *ci = &L1->base_ci;
    while (ci->next) {
	struct CallInfo *next = ci->next->next;
	struct CallInfo *free_ci = ci->next;

	(*g->frealloc)(g->ud, free_ci, sizeof(struct CallInfo), 0);
	ci = next;
    }

    free_stack(L1);
    (*g->frealloc)(g->ud, fromstate(L1), sizeof(LG), 0);
}

void setivalue(StkId target, int integer)
{
    target->value_.i = integer;
    target->tt_ = LUA_NUMINT;
}

void setfvalue(StkId target, lua_CFunction f)
{
    target->value_.f = f;
    target->tt_ = LUA_TLCF;
}

void setfltvalue(StkId target, float number)
{
    target->value_.n = number;
    target->tt_ = LUA_NUMFLT;
}

void setbvalue(StkId target, bool b)
{
    target->value_.b = b ? 1 : 0;
    target->tt_ = LUA_TBOOLEAN;
}

void setnilvalue(StkId target)
{
    target->value_.p = NULL;
    target->tt_ = LUA_TNIL;
}

void setpvalue(StkId target, void *p)
{
    target->value_.p = p;
    target->tt_ = LUA_TLIGHTUSERDATA;
}

void setgco(StkId target, struct GCObject *gco)
{
    target->value_.gc = gco;
    target->tt_ = gco->tt_;
}

void setlclvalue(StkId target, struct LClosure *cl)
{
    setgco(target, obj2gco(cl));
}

void setcclosure(StkId target, struct CClosure *cc)
{
    setgco(target, obj2gco(cc));
}

void setobj(StkId target, StkId value)
{
    target->value_ = value->value_;
    target->tt_ = value->tt_;
}

void increase_top(struct lua_State *L)
{
    L->top++;
    assert(L->top <= L->stack_last);
}

void lua_pushcfunction(struct lua_State *L, lua_CFunction f)
{
    setfvalue(L->top, f);
    increase_top(L);
}

void lua_pushCclosure(struct lua_State *L, lua_CFunction f, int nup)
{
    luaC_checkgc(L);
    CClosure *cc = luaF_newCclosure(L, f, nup);
    setcclosure(L->top, cc);
    increase_top(L);
}

void lua_pushinteger(struct lua_State *L, int integer)
{
    setivalue(L->top, integer);
    increase_top(L);
}

void lua_pushnumber(struct lua_State *L, float number)
{
    setfltvalue(L->top, number);
    increase_top(L);
}

void lua_pushboolean(struct lua_State *L, bool b)
{
    setbvalue(L->top, b);
    increase_top(L);
}

void lua_pushnil(struct lua_State *L)
{
    setnilvalue(L->top);
    increase_top(L);
}

void lua_pushlightuserdata(struct lua_State *L, void *p)
{
    setpvalue(L->top, p);
    increase_top(L);
}

void lua_pushstring(struct lua_State *L, const char *str)
{
    unsigned int l = strlen(str);
    struct TString *ts = luaS_newlstr(L, str, l);
    struct GCObject *gco = obj2gco(ts);
    setgco(L->top, gco);
    increase_top(L);
}

int lua_createtable(struct lua_State *L)
{
    struct Table *tbl = luaH_new(L);
    struct GCObject *gco = obj2gco(tbl);
    setgco(L->top, gco);
    increase_top(L);
    return 1;
}

int lua_settable(struct lua_State *L, int idx)
{
    TValue *o = index2addr(L, idx);
    struct Table *t = gco2tbl(gcvalue(o));

    luaV_settable(L, t, L->top - 2, L->top - 1);

    L->top = L->top - 2;
    return 1;
}

int lua_gettable(struct lua_State *L, int idx)
{
    TValue *o = index2addr(L, idx);
    struct Table *t = gco2tbl(gcvalue(o));
    luaV_gettable(L, t, L->top - 1, L->top - 1);
    return 1;
}

int lua_setfield(struct lua_State *L, int idx, const char *k)
{
    setobj(L->top, L->top - 1);
    increase_top(L);

    TString *s = luaS_newliteral(L, k);
    struct GCObject *gco = obj2gco(s);
    setgco(L->top - 2, gco);

    return lua_settable(L, idx);
}

int lua_pushvalue(struct lua_State *L, int idx)
{
    TValue *o = index2addr(L, idx);
    setobj(L->top, o);
    increase_top(L);
    return LUA_OK;
}

int lua_pushglobaltable(struct lua_State *L)
{
    struct global_State *g = G(L);
    struct Table *registry = gco2tbl(gcvalue(&g->l_registry));
    struct Table *t = gco2tbl(gcvalue(&registry->array[LUA_GLOBALTBLIDX]));
    setgco(L->top, obj2gco(t));
    increase_top(L);

    return LUA_OK;
}

int lua_getglobal(struct lua_State *L, const char *name)
{
    struct GCObject *gco = gcvalue(&G(L)->l_registry);
    struct Table *t = gco2tbl(gco);
    TValue *_go = &t->array[LUA_GLOBALTBLIDX];
    struct Table *_G = gco2tbl(gcvalue(_go));
    TValue *o = (TValue *)luaH_getstr(L, _G, luaS_newliteral(L, name));
    setobj(L->top, o);
    increase_top(L);
    return LUA_OK;
}

int lua_remove(struct lua_State *L, int idx)
{
    for (TValue *o = index2addr(L, idx); o != L->top; o++)
	setobj(o, o + 1);

    L->top--;
    return LUA_OK;
}

int lua_insert(struct lua_State *L, int idx, TValue *v)
{
    TValue *o = index2addr(L, idx);

    for (TValue *top = L->top - 1; top > o; top--)
	setobj(top, top - 1);

    increase_top(L);
    setobj(o, v);
    return LUA_OK;
}

TValue *index2addr(struct lua_State *L, int idx)
{
    if (idx >= 0) {
	assert(L->ci->func + idx < L->ci->top);
	return L->ci->func + idx;
    } else if (LUA_REGISTRYINDEX == idx) {
	return &G(L)->l_registry;
    } else {
	assert(L->top + idx > L->ci->func);
	return L->top + idx;
    }
}

lua_Integer lua_tointegerx(struct lua_State *L, int idx, int *isnum)
{
    lua_Integer ret = 0;
    TValue *addr = index2addr(L, idx);

    if (LUA_NUMINT == addr->tt_) {
	ret = addr->value_.i;
	*isnum = 1;
    } else {
	*isnum = 0;
	LUA_ERROR(L, "can not convert to integer!\n");
    }

    return ret;
}


lua_Number lua_tonumberx(struct lua_State *L, int idx, int *isnum)
{
    lua_Number ret = 0.0f;
    TValue *addr = index2addr(L, idx);

    if (LUA_NUMFLT == addr->tt_) {
	ret = addr->value_.n;
	*isnum = 1;
    } else {
	*isnum = 0;
	LUA_ERROR(L, "can not convert to number!\n");
    }

    return ret;
}

bool lua_toboolean(struct lua_State *L, int idx)
{
    TValue *addr = index2addr(L, idx);
    return !(LUA_TNIL == addr->tt_ || (LUA_TBOOLEAN == addr->tt_ && addr->value_.b == 0));
}

int lua_isnil(struct lua_State *L, int idx)
{
    TValue *addr = index2addr(L, idx);
    return LUA_TNIL == addr->tt_;
}

const char *lua_tostring(struct lua_State *L, int idx)
{
    TValue *addr = index2addr(L, idx);
    if (novariant(addr) != LUA_TSTRING)
	return NULL;

    struct TString *ts = gco2ts(addr->value_.gc);
    return getstr(ts);
}

int lua_gettop(struct lua_State *L)
{
    return cast(int, L->top - (L->ci->func + 1));
}

void lua_settop(struct lua_State *L, int idx)
{
    StkId func = L->ci->func;

    if (idx >= 0) {
	assert(idx <= L->stack_last - (func + 1));

	while (L->top < (func + 1) + idx)
	    setnilvalue(L->top++);

	L->top = func + 1 + idx;
    } else {
	assert(L->top + idx > func);
	L->top = L->top + idx;
    }
}

void lua_pop(struct lua_State *L)
{
    lua_settop(L, -1);
}

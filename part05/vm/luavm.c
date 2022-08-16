#include "luavm.h"
#include "../common/luastring.h"
#include "luado.h"
#include "luagc.h"
#include "luaopcodes.h"
#include "luafunc.h"

// slot == NULL意味着t不是table类型
void luaV_finishget(struct lua_State *L, struct Table *t, StkId val, const TValue *slot)
{
    if (slot == NULL)
	luaD_throw(L, LUA_ERRRUN);

    setnilvalue(val);
}

void luaV_finishset(struct lua_State *L, struct Table *t, const TValue *key, StkId val, const TValue *slot)
{
    if (slot == NULL)
	luaD_throw(L, LUA_ERRRUN);

    if (luaO_nilobject == slot)
	slot = luaH_newkey(L, t, key);

    setobj(cast(TValue *, slot), val);
    luaC_barrierback(L, t, slot);
}

int luaV_eqobject(struct lua_State* L, const TValue *a, const TValue *b)
{
    if ((ttisnumber(a) && lua_numisnan(a->value_.n))
	    || (ttisnumber(b) && lua_numisnan(b->value_.n)))
	return 0;

    if (a->tt_ != b->tt_) {
        // 只有数值，在类型不同的情况下可能相等
	if (LUA_TNUMBER == novariant(a) && LUA_TNUMBER == novariant(b)) {
	    double fa = ttisinteger(a) ? a->value_.i : a->value_.n;
	    double fb = ttisinteger(b) ? b->value_.i : b->value_.n;
	    return fa == fb;
	}

	return 0;
    }

    switch (a->tt_) {
    case LUA_TNIL:
	return 1;

    case LUA_NUMINT:
	return a->value_.i == b->value_.i;

    case LUA_NUMFLT:
	return a->value_.n == b->value_.n;

    case LUA_SHRSTR:
	return luaS_eqshrstr(L, gco2ts(gcvalue(a)), gco2ts(gcvalue(b)));

    case LUA_LNGSTR:
	return luaS_eqlngstr(L, gco2ts(gcvalue(a)), gco2ts(gcvalue(b)));

    case LUA_TBOOLEAN:
	return a->value_.b == b->value_.b;

    case LUA_TLIGHTUSERDATA:
	return a->value_.p == b->value_.p;

    case LUA_TLCF:
	return a->value_.f == b->value_.f;

    default:
	return gcvalue(a) == gcvalue(b);
    }

    return 0;
}

// implement of instructions
static void newframe(struct lua_State *L);
static inline void op_loadk(struct lua_State *L, LClosure *cl, StkId ra, Instruction i)
{
    int bx = GET_ARG_Bx(i);
    setobj(ra, &cl->p->k[bx]);
}

static inline void op_gettabup(struct lua_State *L, LClosure *cl, StkId ra, Instruction i)
{
    int b = GET_ARG_B(i);
    TValue *upval = cl->upvals[b]->v;
    struct Table *t = gco2tbl(gcvalue(upval));
    int arg_c = GET_ARG_C(i);
    if (ISK(arg_c)) {
	int index = arg_c - MAININDEXRK - 1;
	TValue *key = &cl->p->k[index];
	TValue *value = (TValue *)luaH_get(L, t, key);
	setobj(ra, value);
    } else {
	TValue *key = L->ci->l.base + arg_c;
	TValue *value = (TValue *)luaH_get(L, t, key);
	setobj(ra, value);
    }
}

static inline void op_call(struct lua_State *L, LClosure *cl, StkId ra, Instruction i)
{
    int narg = GET_ARG_B(i);
    int nresult = GET_ARG_C(i) - 1;

    if (narg > 0)
	L->top = ra + narg;

    if (luaD_precall(L, ra, nresult)) { // c function
	if (nresult >= 0)
	    L->top = L->ci->top;
    } else
	newframe(L);
}

static inline void op_return(struct lua_State *L, LClosure *cl, StkId ra, Instruction i)
{
    int b = GET_ARG_B(i);
    luaD_poscall(L, ra, b ? (b - 1) : (int)(L->top - ra));
	
    if (L->ci->callstatus & CIST_LUA) {
	L->top = L->ci->top;
	lua_assert(OP_CALL == GET_OPCODE(*(L->ci->savedpc - 1)));
    }
}

void luaV_execute(struct lua_State *L)
{
    L->ci->callstatus |= CIST_FRESH;
    newframe(L);
}

static inline Instruction vmfetch(struct lua_State *L)
{
    Instruction i = *(L->ci->l.savedpc++);
    return i;
}

static inline StkId vmdecode(struct lua_State *L, Instruction i)
{
    StkId ra = L->ci->l.base + GET_ARG_A(i);
    return ra;
}

static bool vmexecute(struct lua_State *L, StkId ra, Instruction i)
{
    bool is_loop = true;
    struct GCObject *gco = gcvalue(L->ci->func);
    LClosure *cl = gco2lclosure(gco);

    switch (GET_OPCODE(i)) {
    case OP_GETTABUP:
	op_gettabup(L, cl, ra, i);
	break;

    case OP_LOADK:
	op_loadk(L, cl, ra, i);
	break;

    case OP_CALL:
	op_call(L, cl, ra, i);
	break;

    case OP_RETURN:
	op_return(L, cl, ra, i);
	is_loop = false;
	break;

    default:
	break;
    }

    return is_loop;
}

static void print_TValue(const TValue* v) {
    switch (v->tt_) {
    case LUA_NUMINT:
	printf("%ld ", v->value_.i);
	break;

    case LUA_NUMFLT:
	printf("%.14g ", v->value_.n);
	break;

    case LUA_SHRSTR: case LUA_LNGSTR: {
	TString* ts = gco2ts(gcvalue(v));
	printf("%s ", getstr(ts));
    }
	break;

    case LUA_TBOOLEAN:
	printf("%s ", v->value_.b ? "true" : "false");
	break;

    default:
	break;
    }
}

static char *code2name[NUM_OPCODES] = {
	"OP_MOVE",
	"OP_LOADK",
	"OP_GETUPVAL",
	"OP_CALL",
	"OP_RETURN",
	"OP_GETTABUP",
	"OP_GETTABLE",
};

static void print_Instruction(int idx, Instruction i)
{
    switch (luaP_opmodes[GET_OPCODE(i)] & 0x03) {
    case iABC:
	printf("[%d] opcode(%s) ra(%d) rb(%d) rc(%d)\n",
	    idx, code2name[GET_OPCODE(i)], GET_ARG_A(i), GET_ARG_B(i), GET_ARG_C(i));
	break;

    case iABx:
	printf("[%d] opcode(%s) ra(%d) bx(%d)\n",
	    idx, code2name[GET_OPCODE(i)], GET_ARG_A(i), GET_ARG_Bx(i));
	break;

    default: break;
    }
}

static void newframe(struct lua_State *L)
{
    // print table k
    struct GCObject *gco = gcvalue(L->ci->func);
    LClosure *cl = gco2lclosure(gco);

    fputs("k:", stdout);
    for (int i = 0; i < cl->p->sizek; i++)
	print_TValue(&cl->p->k[i]);
    fputs("\n", stdout);

    for (int i = 0; i < cl->p->sizecode; i++) {
	print_Instruction(i, cl->p->code[i]);
	if (OP_RETURN == GET_OPCODE(cl->p->code[i]))
	    break;
    }
    fputs("\n", stdout);

    bool is_loop = true;
    while (is_loop) {
	Instruction i = vmfetch(L);
	StkId ra = vmdecode(L, i);
	is_loop = vmexecute(L, ra, i);
    }
}

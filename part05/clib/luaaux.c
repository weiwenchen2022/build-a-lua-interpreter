#include "luaaux.h"
#include "../vm/luado.h"
#include "../common/luastring.h"
#include "../common/luatable.h"
#include "../vm/luafunc.h"

static void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    (void)ud;
    (void)osize;

    if (nsize == 0) {
	free(ptr);
	return NULL;
    }

    return realloc(ptr, nsize);
}

struct lua_State *luaL_newstate(void)
{
    struct lua_State *L = lua_newstate(&l_alloc, NULL);
    return L;
}

void luaL_close(lua_State *L)
{
    lua_close(L);
}

int luaL_requiref(struct lua_State *L, const char *name, lua_CFunction func, int glb)
{
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED);

    TValue *top = luaL_index2addr(L, lua_gettop(L));
    if (LUA_TNIL == novariant(top)) {
	luaL_pop(L);
	luaL_createtable(L); // _LOADED = {}
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_REGISTRYINDEX, LUA_LOADED); // set _LOADED to registry
    }

    luaL_getsubtable(L, -1, name);
    if (LUA_TNIL == novariant(L->top - 1)) {
	luaL_pop(L);

	luaL_pushcfunction(L, func);
	luaL_pushstring(L, name);
	luaL_pcall(L, 1, 1);

	lua_pushvalue(L, -1);
	lua_setfield(L, -3, name);
    }

    lua_remove(L, -2); // remove _LOADED

    if (glb) {
	lua_pushglobaltable(L); // push _G into stack top

	TValue *o = index2addr(L, lua_gettop(L));
	lua_insert(L, -2, o); // ... _G M _G
	lua_pop(L); // ... _G M

	lua_pushstring(L, name); // ... _G M name
	o = index2addr(L, lua_gettop(L));
	lua_insert(L, -2, o); // ... _G name M name
	lua_pop(L); // ... _G name M

	luaL_settable(L, -3); // ... _G
    }

    lua_pop(L); // pop _G
    return LUA_OK;
}

void luaL_pushinteger(struct lua_State *L, int integer)
{
    lua_pushinteger(L, integer);
}

void luaL_pushnumber(struct lua_State *L, float number)
{
    lua_pushnumber(L, number);
}

void luaL_pushlightuserdata(struct lua_State *L, void *userdata)
{
    lua_pushlightuserdata(L, userdata);
}

void luaL_pushnil(struct lua_State *L)
{
    lua_pushnil(L);
}

void luaL_pushcfunction(struct lua_State *L, lua_CFunction f)
{
    lua_pushcfunction(L, f);
}

void luaL_pushboolean(struct lua_State *L, bool boolean)
{
    lua_pushboolean(L, boolean);
}

void luaL_pushstring(struct lua_State *L, const char *str)
{
    lua_pushstring(L, str);
}

// function call
typedef struct CallS {
    StkId func;
    int nresult;
} CallS;

static int f_call(lua_State *L, void *ud)
{
    CallS *c = cast(CallS *, ud);
    luaD_call(L, c->func, c->nresult);
    return LUA_OK;
}

int luaL_pcall(struct lua_State *L, int narg, int nresult)
{
    int status;
    CallS c;
    c.func = L->top - (narg + 1);
    c.nresult = nresult;

    status = luaD_pcall(L, &f_call, &c, savestack(L, L->top), 0);
    return status;
}

bool luaL_checkinteger(struct lua_State *L, int idx)
{
    int isnum;
    lua_tointegerx(L, idx, &isnum);

    if (isnum)
	return true;
    else
	return false;
}

lua_Integer luaL_tointeger(struct lua_State *L, int idx)
{
    int isnum;
    lua_Integer ret = lua_tointegerx(L, idx, &isnum);
    return ret;
}

lua_Number luaL_tonumber(struct lua_State *L, int idx)
{
    int isnum;
    lua_Number ret = lua_tonumberx(L, idx, &isnum);
    return ret;
}

int luaL_isnil(struct lua_State *L, int idx)
{
    return lua_isnil(L, idx);
}

const char *luaL_tostring(struct lua_State *L, int idx)
{
    return lua_tostring(L, idx);
}

TValue *luaL_index2addr(struct lua_State *L, int idx)
{
    return index2addr(L, idx);
}

int luaL_createtable(struct lua_State *L)
{
    return lua_createtable(L);
}

int luaL_settable(struct lua_State *L, int idx)
{
    return lua_settable(L, idx);
}

int luaL_gettable(struct lua_State *L, int idx)
{
    return lua_gettable(L, idx);
}

int luaL_getglobal(struct lua_State *L)
{
    return lua_pushglobaltable(L);
}

int luaL_getsubtable(struct lua_State *L, int idx, const char *name)
{
    TValue *o = luaL_index2addr(L, idx);
    if (novariant(o) != LUA_TTABLE)
	return LUA_ERRERR;

    struct Table *tbl = gco2tbl(gcvalue(o));
    TValue *subtbl = (TValue *)luaH_getstr(L, tbl, luaS_newliteral(L, name));
    if (luaO_nilobject == subtbl) {
	lua_pushnil(L);
	return LUA_OK;
    }

    setobj(L->top, subtbl);
    increase_top(L);
    return LUA_OK;
}

void *luaL_touserdata(struct lua_State *L, int idx)
{
    return NULL;
}

bool luaL_toboolean(struct lua_State *L, int idx)
{
    return lua_toboolean(L, idx);
}

void luaL_pop(struct lua_State *L)
{
    lua_pop(L);
}

int luaL_stacksize(struct lua_State *L)
{
    return lua_gettop(L);
}

static char *getF(struct lua_State *L, void *data, size_t *sz)
{
    LoadF *lf = (LoadF *)data;
    if (lf->n > 0) {
	*sz = lf->n;
	lf->n = 0;
    } else {
	*sz = fread(lf->buff, sizeof(char), BUFSIZE, lf->f);
	lf->n = 0;
    }

    return lf->buff;
}

static void init_upval(struct lua_State *L)
{
    StkId top = L->top - 1;
    LClosure *cl = gco2lclosure(gcvalue(top));
    if (cl->nupvalues > 0) {
	struct Table *t = gco2tbl(gcvalue(&G(L)->l_registry));
	TValue *_G = &t->array[LUA_GLOBALTBLIDX];
	setobj(cl->upvals[0]->v, _G);
    }
}

int luaL_loadfile(struct lua_State *L, const char *filename)
{
    FILE *fp;
    l_fopen(&fp, filename, "rb");
    if (fp == NULL) {
	fprintf(stderr, "can not open file '%s'\n", filename);
	return LUA_ERRERR;
    }

    // init LoadF
    LoadF lf;
    lf.f = fp;
    lf.n = 0;
    memset(lf.buff, 0, BUFSIZE);

    int ok = luaD_load(L, getF, &lf, filename);
    if (LUA_OK == ok)
	init_upval(L);

    fclose(fp);
    return ok;
}

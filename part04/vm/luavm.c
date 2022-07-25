#include "luavm.h"
#include "../common/luastring.h"
#include "luado.h"
#include "luagc.h"

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

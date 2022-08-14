#include "luabase.h"
#include "../clib/luaaux.h"
#include "luastring.h"
#include "../vm/luagc.h"
#include "lua.h"

#define MAX_NUMBER_STR_SIZE 64

static int lprint(struct lua_State *L)
{
    int num_arg = lua_gettop(L);
    for (int i = 0; i < num_arg; i++) {
	lua_getglobal(L, "tostring");
	lua_pushvalue(L, i + 1);
	luaL_pcall(L, 1, 1);

	const char *s = lua_tostring(L, -1);
	lua_writestring(s);
	if (i < num_arg - 1)
	    lua_writestring("\t");

	lua_pop(L);
    }

    lua_writeline();
    return 0;
}

static int ltostring(struct lua_State *L)
{
    TValue *o = index2addr(L, 1);
    char max_number_buffer[MAX_NUMBER_STR_SIZE];
    memset(max_number_buffer, 0, sizeof(max_number_buffer));

    switch (o->tt_) {
    case LUA_NUMINT:
	l_sprintf(max_number_buffer, MAX_NUMBER_STR_SIZE, LUA_INTEGER_FORMAT, o->value_.i);
	luaL_pushstring(L, max_number_buffer);
	break;

    case LUA_NUMFLT:
	l_sprintf(max_number_buffer, MAX_NUMBER_STR_SIZE, LUA_NUMBER_FORMAT, o->value_.n);
	luaL_pushstring(L, max_number_buffer);
	break;

    case LUA_LNGSTR:
    case LUA_SHRSTR: {
	struct GCObject *gco = gcvalue(o);
	TString *s = gco2ts(gco);
	luaL_pushstring(L, getstr(s));
    }
	break;

    case LUA_TNIL:
	luaL_pushstring(L, "nil");
	break;

    case LUA_TBOOLEAN:
	luaL_pushstring(L, o->value_.b ? "true" : "false");
	break;

    case LUA_TLIGHTUSERDATA: {
	uintptr_t addr = (uintptr_t)o->value_.p;
	l_sprintf(max_number_buffer, MAX_NUMBER_STR_SIZE, "%#lx", addr);
	luaL_pushstring(L, max_number_buffer);
    }
	break;

    case LUA_TNONE:
	luaL_pushstring(L, "(none)");
	break;

    case LUA_TLCF: {
	uintptr_t addr = (uintptr_t)o->value_.f;
	l_sprintf(max_number_buffer, MAX_NUMBER_STR_SIZE, "%#lx", addr);
	luaL_pushstring(L, max_number_buffer);
    }
	break;

    default: {
	struct GCObject *gco = gcvalue(o);
	uintptr_t addr = (uintptr_t)gco;
	l_sprintf(max_number_buffer, MAX_NUMBER_STR_SIZE, "%#lx", addr);
	luaL_pushstring(L, max_number_buffer);
    }
	break;
    }

    return 1;
}

static const lua_Reg base_reg[] = {
    {"print", lprint,},
    {"tostring", ltostring,},

    {NULL, NULL,},
};

int luaB_openbase(struct lua_State *L)
{
    lua_pushglobaltable(L);

    for (int i = 0; i < sizeof(base_reg)/sizeof(base_reg[0]); i++) {
	if (base_reg[i].name && base_reg[i].func) {
	    lua_pushstring(L, base_reg[i].name);
	    lua_pushCclosure(L, base_reg[i].func, 1);
	    lua_settable(L, -3);
	}
    }

    return 1;
}

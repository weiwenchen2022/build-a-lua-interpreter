#include "p4_test.h"
#include "../clib/luaaux.h"
#include "../common/luatable.h"
#include "../vm/luagc.h"
#include "../common/luastring.h"

static void print_object(struct lua_State *L, TValue *o)
{
    switch (o->tt_) {
    case LUA_TNIL:
	fputs("nil\n", stdout);
	break;

    case LUA_NUMINT:
	fprintf(stdout, "type: int value: %ld\n", o->value_.i);
	break;

    case LUA_NUMFLT:
	fprintf(stdout, "type: float value: %f\n", o->value_.n);
	break;

    case LUA_SHRSTR:
    case LUA_LNGSTR:
	fprintf(stdout, "type: string value: '%s'\n", getstr(gco2ts(gcvalue(o))));
	break;

    case LUA_TLCF:
	fprintf(stdout, "type light c function value: %#x\n", point2uint(o->value_.f));
	break;

    default:
	break;
    }
}

static int test_func(struct lua_State *L)
{
    fputs("this is a test func hahaha!\n", stdout);
    return 0;
}

static void test_kv(struct lua_State *L)
{
    int tbl_idx = luaL_stacksize(L);
    printf("stack_size: %d, tbl_idx: %d\n", luaL_stacksize(L), tbl_idx);

    // push integer key
    luaL_pushinteger(L, 1);
    luaL_pushstring(L, "test integer key");
    luaL_settable(L, tbl_idx);

    // push short string key
    luaL_pushstring(L, "short string key");
    luaL_pushstring(L, "test short string key");
    luaL_settable(L, tbl_idx);

    // push long string key
    luaL_pushstring(L, "This is long string key!"
	    " This is long string key!"
	    " This is long string key!"
	    " This is long string key!");
    luaL_pushstring(L, "This is long string key!"
	    " This is long string key!"
	    " This is long string key!"
	    " This is long string key!");
    luaL_settable(L, tbl_idx);

    // push float key
    luaL_pushnumber(L, 2.0f);
    luaL_pushstring(L, "test float key");
    luaL_settable(L, tbl_idx);

    // push boolean key
    luaL_pushboolean(L, true);
    luaL_pushstring(L, "test boolean key");
    luaL_settable(L, tbl_idx);

    // push function key(test pointer)
    luaL_pushcfunction(L, &test_func);
    luaL_pushstring(L, "test pointer key");
    luaL_settable(L, tbl_idx);

    luaL_pushinteger(L, 1);
    luaL_gettable(L, tbl_idx);
    print_object(L, L->top - 1);
    luaL_pop(L); // now top is table

    TValue *tbl_object = luaL_index2addr(L, tbl_idx);
    struct Table *t = gco2tbl(gcvalue(tbl_object));
    printf("test t->tt_: %d, stack_size: %d\n", t->tt_, luaL_stacksize(L));

    luaL_pushinteger(L, 1); // push nil, this slot for key
    luaL_pushnil(L); // push nil, this slot for value

    while (luaH_next(L, t, L->top - 2)) {
	printf("key: ");
	print_object(L, L->top - 2);
	printf("value: ");
	print_object(L, L->top - 1);
    }

    printf("getn = %d\n", luaH_getn(L, t));
    luaL_pop(L);
    luaL_pop(L);
}

static void test_gc(struct lua_State *L)
{
    time_t start_time = time(NULL), end_time;
    size_t max_bytes = 0;
    struct global_State *g = G(L);
    for (int j = 0; j < 50000; j++) {
	luaL_createtable(L);
	test_kv(L);
	luaL_pop(L);
	luaC_checkgc(L);

	if (max_bytes < g->totalbytes + g->GCdebt)
	    max_bytes = g->totalbytes + g->GCdebt;

	if (j % 1000 == 0)
            printf("timestamp: %d totalbytes: %f Kb\n",
		    (int)time(NULL), (float)(g->totalbytes + g->GCdebt) / 1024.0f);
    }

    end_time = time(NULL);
    printf("finish test start_time: %d, end_time: %d, max_bytes: %f Kb\n",
	    (int)start_time, (int)end_time, (float)max_bytes / 1024.0f);
}

void p4_test_main(void)
{
    struct lua_State *L = luaL_newstate();

    luaL_createtable(L);
    test_kv(L);
    luaL_pop(L); // pop table

    luaL_getglobal(L);
    test_kv(L);
    luaL_pop(L);

    test_gc(L);

    luaL_close(L);
}

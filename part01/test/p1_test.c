#include "p1_test.h"
#include "../clib/luaaux.h"

#include <assert.h>

static void test_integer(lua_Integer expected, lua_Integer actual)
{
    if (expected != actual) {
	fprintf(stderr, "test_integer, expected = %ld, got = %ld\n", expected, actual);
	exit(EXIT_FAILURE);
    }
}

static void test_number(lua_Number expected, lua_Number actual)
{
    if (expected != actual) {
	fprintf(stderr, "test_number, expected = %f, got = %f\n", expected, actual);
	exit(EXIT_FAILURE);
    }
}

static void test_boolean(bool expected, bool actual)
{
    if (expected != actual) {
	fprintf(stderr, "test_boolean, expected = %d, got = %d\n", (int)expected, (int)actual);
	exit(EXIT_FAILURE);
    }
}

static void test_stacksize(int expected, int actual)
{
    if (expected != actual) {
	fprintf(stderr, "test_stacksize, expected = %d, got = %d\n", expected, actual);
	exit(EXIT_FAILURE);
    }
}

static void test_nil(struct lua_State *L, int idx)
{
    if (!lua_isnil(L, idx)) {
	fprintf(stderr, "test_nil not nil, idx = %d\n", idx);
	exit(EXIT_FAILURE);
    }
}

// test case 1
static int test_main01(struct lua_State *L)
{
    lua_Integer i = luaL_tointeger(L, 1);
    test_integer(1, i);
    // printf("test_main01 luaL_tointeger value = %d\n", i);
    return 0;
}

// nwant = 0; nresult = 0;
void p1_test_result01(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main01);
    luaL_pushinteger(L, 1);
    luaL_pcall(L, 1, 0);
    luaL_close(L);
}

// test case 2
static int test_main02(struct lua_State *L)
{
    lua_Integer i = luaL_tointeger(L, 1);
    luaL_pushinteger(L, ++i);
    test_integer(2, luaL_tointeger(L, -1));
    // printf("test_main02 luaL_tointeger value = %d\n", luaL_tointeger(L, -1));
    return 1;
}

// nwant = 0; nresult = 1;
void p1_test_result02(void)
{
    struct lua_State *L = luaL_newstate();

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result02 before push args test_main02 stacksize = %d\n", luaL_stacksize(L));

    luaL_pushcfunction(L, &test_main02);
    luaL_pushinteger(L, 1);

    test_stacksize(2, luaL_stacksize(L));
    // printf("p1_test_result02 before call test_main02 stacksize = %d\n", luaL_stacksize(L));

    luaL_pcall(L, 1, 0);

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result02 after call test_main02 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

// test case 3
static int test_main03(struct lua_State *L)
{
    static const int nret = 5;

    test_boolean(true, luaL_toboolean(L, 2));
    test_number(1.0f, luaL_tonumber(L, 1));

    // int b = luaL_toboolean(L, 2) ? 1 : 0;
    // lua_Number n = luaL_tonumber(L, 1);
    // printf("test_main03 b: %d, n: %f\n", b, n);

    int i;
    for (i = 0; i < nret; i++)
	luaL_pushinteger(L, i);

    return nret;
}

// nwant = 0; nresult > 1;
void p1_test_result03(void)
{
    struct lua_State *L = luaL_newstate();

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result03 before push arg test_main03 stacksize = %d\n", luaL_stacksize(L));

    luaL_pushcfunction(L, &test_main03);
    luaL_pushnumber(L, 1.0f);
    luaL_pushboolean(L, true);

    test_stacksize(3, luaL_stacksize(L));
    // printf("p1_test_result03 before call test_main03 stacksize = %d\n", luaL_stacksize(L));

    luaL_pcall(L, 2, 0);

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result03 after call test_main03 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

// test case 4
static int test_main04(struct lua_State *L)
{
    test_integer(1, luaL_tointeger(L, 1));
    test_integer(2, luaL_tointeger(L, 2));
    // int arg1 = luaL_tointeger(L, 1);
    // int arg2 = luaL_tointeger(L, 2);
    // printf("test_main04 arg1: %d, arg2: %d\n", arg1, arg2);

    return 0;
}

// nwant = 1; nresult = 0;
void p1_test_result04(void)
{
    struct lua_State *L = luaL_newstate();

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result04 before push arg test_main04 stacksize = %d\n", luaL_stacksize(L));

    luaL_pushcfunction(L, &test_main04);
    luaL_pushinteger(L, 1);
    luaL_pushinteger(L, 2);

    test_stacksize(3, luaL_stacksize(L));
    // printf("p1_test_result04 before call test_main04 stacksize = %d\n", luaL_stacksize(L));
    luaL_pcall(L, 2, 1);

    test_stacksize(1, luaL_stacksize(L));
    // printf("p1_test_result04 after call test_main04 stacksize = %d\n", luaL_stacksize(L));

    test_nil(L, -1);
    // int isnil = luaL_isnil(L, -1);
    // printf("p1_test_result04 isnil: %d\n", isnil);

    luaL_pop(L);
    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result04 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

static int test_main05(struct lua_State *L)
{
    test_integer(1, luaL_tointeger(L, 1));
    test_number(2.0f, luaL_tonumber(L, 2));

    // lua_Integer i = luaL_tointeger(L, 1);
    // lua_Number n = luaL_tonumber(L, 2);
    // printf("test_main05 n = %f, i = %d\n", n, i);

    luaL_pushboolean(L, true);

    return 1;
}
// nwant = 1; nresult = 1;
void p1_test_result05(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main05);
    luaL_pushinteger(L, 1);
    luaL_pushnumber(L, 2.0f);

    luaL_pcall(L, 2, 1);

    test_boolean(true, luaL_toboolean(L, -1));
    // bool b = luaL_toboolean(L, -1);
    // printf("p1_test_result05 top_value: %d\n", b ? 1 : 0);

    luaL_pop(L);
    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result05 before close stack_size:%d\n", luaL_stacksize(L));

    luaL_close(L);
}

// test case 6
static int test_main06(struct lua_State *L)
{
    static const int nret = 5;

    int i;
    for (i = 0; i < nret; i++)
       luaL_pushinteger(L, i);

    return nret;
}

// nwant = 1; nresult > 1;
void p1_test_result06(void)
{
    struct lua_State *L = luaL_newstate();

    luaL_pushcfunction(L, &test_main06);
    luaL_pushinteger(L, 1);
    luaL_pushnumber(L, 2.0f);
    luaL_pcall(L, 2, 1);

    test_stacksize(1, luaL_stacksize(L));
    // printf("p1_test_result06 after call stacksize: %d\n", luaL_stacksize(L));

    test_integer(0, luaL_tointeger(L, -1));
    // lua_Integer v = luaL_tointeger(L, -1);
    // printf("p1_test_result06 top value: %d\n", v);

    luaL_pop(L);
    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result06 final stacksize:%d\n", luaL_stacksize(L));

    luaL_close(L);
}

static int test_main07(struct lua_State *L)
{
    (void)L;
    return 0;
}

// nwant > 1; nresult = 0;
void p1_test_result07(void)
{
    static const int nwant = 8;
    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main07);
    luaL_pcall(L, 0, nwant);

    test_stacksize(nwant, luaL_stacksize(L));
    // printf("p1_test_result07 after call stack_size: %d\n", luaL_stacksize(L));

    int i;
    for (i = 0; i < nwant; i++) {
	test_nil(L, -1);
        // int isnil = luaL_isnil(L, -1);
        luaL_pop(L);
        // printf("p1_test_result07 stack_idx: %d isnil:%d\n", nwant - i, isnil);
    }

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result07 final stack_size:%d\n", luaL_stacksize(L));

    luaL_close(L);
}

// test case 8
static const int test_result08_nresult = 7;
static int test_main08(struct lua_State *L)
{
    int i;
    for (i = 0; i < test_result08_nresult; i++)
        luaL_pushinteger(L, i + 10);

    return test_result08_nresult;
}

// nwant > 1; nresult > 0;
void p1_test_result08(void)
{
    static const int test_result08_nwant = 8;

    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main08);
    luaL_pcall(L, 0, test_result08_nwant);

    test_stacksize(test_result08_nwant, luaL_stacksize(L));
    // printf("p1_test_result08 after call stack_size: %d\n", luaL_stacksize(L));

    int i;
    for (i = 0; i < test_result08_nwant; i++) {
        if (i != 0) {
	    test_integer(10 + test_result08_nresult - i, luaL_tointeger(L, -1));
            // lua_Integer integer = luaL_tointeger(L, -1);
            // printf("p1_test_result08 stack_idx: %d integer: %d\n", test_result08_nwant - i, integer);
        } else {
	    test_nil(L, -1);
            // int isnil = luaL_isnil(L, -1);
            // printf("p1_test_result08 stack_idx: %d isnil: %d\n", test_result08_nwant - i, isnil);
        }

	luaL_pop(L);
    }

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result08 final stack_size: %d\n", luaL_stacksize(L));
    luaL_close(L);
}

// test case 9
static int test_main09(struct lua_State *L)
{
    (void)L;
    return 0;
}

// nwant = -1; nresult = 0;
void p1_test_result09(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main09);
    test_stacksize(1, luaL_stacksize(L));
    // printf("p1_test_result09 before call  stack_size: %d\n", luaL_stacksize(L));

    luaL_pcall(L, 0, LUA_MULRET);
    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result09 after call stack_size: %d\n", luaL_stacksize(L));
    luaL_close(L);
}

// test case 10
static const int test_result10_nresult = 10;
static int test_main10(struct lua_State *L)
{
    int i;
    for (i = 0; i < test_result10_nresult; i++)
        luaL_pushinteger(L, i);

    return test_result10_nresult;
}

// nwant = -1; nresult > 0;
void p1_test_result10(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_pushcfunction(L, &test_main10);
    luaL_pcall(L, 0, LUA_MULRET);

    test_stacksize(test_result10_nresult, luaL_stacksize(L));
    // int stack_size = luaL_stacksize(L);
    // printf("p1_test_result10 after call stack_size: %d\n", stack_size);

    int i;
    for (i = 0; i < test_result10_nresult; i++) {
	test_integer(test_result10_nresult - (i + 1), luaL_tointeger(L, -1));
        // lua_Integer integer = luaL_tointeger(L, -1);
        luaL_pop(L);
        // printf("stack value %d\n", integer);
    }

    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_result10 final stack_size: %d\n", luaL_stacksize(L));

    luaL_close(L);
}

// test case nest call 01
static int current_calls = 0;
static int test_nestcal01(struct lua_State *L)
{
    lua_Integer arg = luaL_tointeger(L, -1);
    current_calls++;

    // int temp = current_calls;
    // printf("test_nestcal01 enter_calls %d\n", current_calls);

    if (current_calls < LUA_MAXCALLS - 1) {
        luaL_pushcfunction(L, &test_nestcal01);
        luaL_pushinteger(L, arg + 1);
        luaL_pcall(L, 1, 1);
        arg = luaL_tointeger(L, -1);
        luaL_pop(L);
    }

    // printf("test_nestcal01 current_calls: %d stack_size: %d\n", temp, luaL_stacksize(L));
    luaL_pushinteger(L, arg);

    return 1;
}

// call count < LUA_MAXCALLS
void p1_test_nestcall01(void)
{
    struct lua_State *L = luaL_newstate();

    luaL_pushcfunction(L, &test_nestcal01);
    luaL_pushinteger(L, 1);
    luaL_pcall(L, 1, 1);

    test_integer(LUA_MAXCALLS - 1, luaL_tointeger(L, -1));

    luaL_pop(L);
    test_stacksize(0, luaL_stacksize(L));
    // printf("p1_test_nestcall01 result = %ld stack_size: %d\n", luaL_tointeger(L, -1), luaL_stacksize(L));

    luaL_close(L);
}

// call count >= LUA_MAXCALLS
void p1_test_nestcall02(void)
{

}

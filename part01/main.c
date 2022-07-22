#include "clib/luaaux.h"
#include "test/p1_test.h"

// static int add_op(struct lua_State *L);

int main(int argc, const char *argv[])
{
    printf("\n--------------------test case 1--------------------------\n");
    p1_test_result01();

    printf("\n--------------------test case 2--------------------------\n");
    p1_test_result02();

    printf("\n--------------------test case 3--------------------------\n");
    p1_test_result03();

    printf("\n--------------------test case 4--------------------------\n");
    p1_test_result04();

    printf("\n--------------------test case 5--------------------------\n");
    p1_test_result05();

    printf("\n--------------------test case 6--------------------------\n");
    p1_test_result06();

    printf("\n--------------------test case 7--------------------------\n");
    p1_test_result07();

    printf("\n--------------------test case 8--------------------------\n");
    p1_test_result08();

    printf("\n--------------------test case 9--------------------------\n");
    p1_test_result09();

    printf("\n--------------------test case 10-------------------------\n");
    p1_test_result10();

    printf("\n--------------------test case 11-------------------------\n");
    p1_test_nestcall01();

    return 0;

    // struct lua_State *L = luaL_newstate();

    // luaL_pushcfunction(L, &add_op);
    // luaL_pushinteger(L, 1);
    // luaL_pushinteger(L, 2);
    // luaL_pcall(L, 2, 1);

    // int result = luaL_tointeger(L, -1);
    // printf("result is %d\n", result);
    // luaL_pop(L);

    // printf("final stack size %d\n", luaL_stacksize(L));
    // luaL_close(L);
    // system("pause");
    // return 0;
}

// static int add_op(struct lua_State *L)
// {
//     int left = luaL_tointeger(L, -2);
//     int right = luaL_tointeger(L, -1);
// 
//     luaL_pushinteger(L, left + right);
//     return 1;
// }

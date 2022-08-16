#include "p5_test.h"
#include "../common/lua.h"
#include "../clib/luaaux.h"

void p5_test_main(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    int ok = luaL_loadfile(L, "./scripts/part05_test.lua");
    if (LUA_OK == ok)
        luaL_pcall(L, 0, 0);

    luaL_close(L);
}

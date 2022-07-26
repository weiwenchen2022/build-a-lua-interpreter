#pragma once
#include "../common/luastate.h"

// a structure for us to load standard libraries
typedef struct lua_Reg {
    const char *name;
    lua_CFunction func;
} lua_Reg;

struct lua_State *luaL_newstate(void);
void luaL_close(lua_State *L);

void luaL_openlibs(struct lua_State *L); // open standard libraries
int luaL_requiref(struct lua_State *L, const char *name, lua_CFunction func, int glb); // try to load module inot _LOADED table

void luaL_pushinteger(struct lua_State *L, int integer);
void luaL_pushnumber(struct lua_State *L, float number);
void luaL_pushlightuserdata(struct lua_State *L, void *userdata);
void luaL_pushnil(struct lua_State *L);
void luaL_pushcfunction(struct lua_State *L, lua_CFunction f);
void luaL_pushboolean(struct lua_State *L, bool boolean);
void luaL_pushstring(struct lua_State *L, const char *str);

int luaL_pcall(struct lua_State *L, int narg, int nresult);

bool luaL_checkinteger(struct lua_State *L, int idx);
lua_Integer luaL_tointeger(struct lua_State *L, int idx);
lua_Number luaL_tonumber(struct lua_State *L, int idx);
void *luaL_touserdata(struct lua_State *L, int idx);
bool luaL_toboolean(struct lua_State *L, int idx);
const char *luaL_tostring(struct lua_State *L, int idx);
int luaL_isnil(struct lua_State *L, int idx);
TValue *luaL_index2addr(struct lua_State *L, int idx);

int luaL_createtable(struct lua_State *L);
int luaL_settable(struct lua_State *L, int idx);
int luaL_gettable(struct lua_State *L, int idx);
int luaL_getglobal(struct lua_State *L);
int luaL_getsubtable(struct lua_State *L, int idx, const char *name); // try to get a sub table by name and push into the stack

void luaL_pop(struct lua_State *L);
int luaL_stacksize(struct lua_State *L);

// load source and compile, if load success, then it returns 1, otherwise it returns 0
int luaL_loadfile(struct lua_State *L, const char *filename);

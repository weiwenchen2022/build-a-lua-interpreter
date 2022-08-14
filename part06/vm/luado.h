#pragma once

#include "../common/luastate.h"
#include "../compiler/luazio.h"

#define CIST_LUA 1
#define CIST_FRESH (1 << 1)

typedef int (*Pfunc)(struct lua_State *L, void *ud);

void seterrobj(struct lua_State *L, int error);
void luaD_checkstack(struct lua_State *L, int need);
void luaD_growstack(struct lua_State *L, int size);
void luaD_throw(struct lua_State *L, int error);

int luaD_rawrunprotected(struct lua_State *L, Pfunc f, void *ud);
int luaD_precall(struct lua_State *L, StkId func, int nresult);
int luaD_poscall(struct lua_State *L, StkId first_result, int nresult);
int luaD_call(struct lua_State *L, StkId func, int nresult);
int luaD_pcall(struct lua_State *L, Pfunc f, void *ud, ptrdiff_t oldtop, ptrdiff_t ef);

// load lua source from file, and parser it
int luaD_load(struct lua_State *L, lua_Reader reader, void *data, const char *filename);
int luaD_protectedparser(struct lua_State *L, Zio *z, const char *filename);

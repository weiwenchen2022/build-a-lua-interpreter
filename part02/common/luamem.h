#pragma once

#include "luastate.h"

#define luaM_free(L, ptr, osize) luaM_realloc(L, ptr, osize, 0)

void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize);

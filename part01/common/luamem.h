#pragma once

#include "luastate.h"

void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize);

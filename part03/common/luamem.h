#pragma once

#include "luastate.h"

#define luaM_free(L, ptr, osize) luaM_realloc(L, ptr, osize, 0)
#define luaM_reallocvector(L, ptr, osize, nsize, t) \
    ((ptr) = (t *)luaM_realloc(L, ptr, osize * sizeof(t), nsize * sizeof(t)))

void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize);

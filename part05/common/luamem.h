#pragma once

#include "luastate.h"

#define luaM_free(L, ptr, osize) luaM_realloc(L, ptr, osize, 0)
#define luaM_reallocvector(L, ptr, osize, nsize, t) \
    ((ptr) = (t *)luaM_realloc(L, ptr, osize * sizeof(t), nsize * sizeof(t)))
#define luaM_newvector(L, n, t) luaM_realloc(L, NULL, 0, n * sizeof(t))
#define luaM_growvector(L, ptr, n, s, t, l) \
    if ((n) + 1 >= (s)) ((ptr) = cast(t *, luaM_growaux_(L, ptr, &(s), sizeof(t), l)))

void *luaM_growaux_(struct lua_State *L, void *ptr, int *size, int element, int limit);
void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize);

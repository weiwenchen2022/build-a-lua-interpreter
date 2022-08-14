#include "luamem.h"
#include "../vm/luado.h"

#define MINARRAYSIZE 4

void *luaM_growaux_(struct lua_State *L, void *ptr, int *size, int element, int limit)
{
    void *newblock;
    int block_size = *size;
    if (block_size > limit/2) {
	if (block_size > limit) {
	    LUA_ERROR(L, "luaM_growaux_ size too big");
	    luaD_throw(L, LUA_ERRMEM);
	}

	block_size = limit;
    } else
	block_size *= 2;

    if (block_size <= MINARRAYSIZE)
	block_size = MINARRAYSIZE;

    newblock = luaM_realloc(L, ptr, *size * element, block_size * element);
    *size = block_size;
    return newblock;
}

void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize)
{
    struct global_State *g = G(L);
    osize = ptr ? osize : 0;

    void *ret = (*g->frealloc)(g->ud, ptr, osize, nsize);
    if (ret == NULL && nsize > 0)
	luaD_throw(L, LUA_ERRMEM);

    // fprintf(stdout, "luaM_realloc, GCdebt = %zd, osize = %zu, nsize = %zu\n",
    //         g->GCdebt, osize, nsize);
    g->GCdebt = g->GCdebt - osize + nsize;

    return ret;
}

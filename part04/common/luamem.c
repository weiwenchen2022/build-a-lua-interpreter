#include "luamem.h"
#include "../vm/luado.h"

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

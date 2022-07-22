#include "luamem.h"
#include "../vm/luado.h"

void *luaM_realloc(struct lua_State *L, void *ptr, size_t osize, size_t nsize)
{
    struct global_State *g = G(L);
    osize = ptr ? osize : 0;

    void *ret = (*g->frealloc)(g->ud, ptr, osize, nsize);
    if (ret == NULL)
	luaD_throw(L, LUA_ERRMEM);

    return ret;
}

#include "p2_test.h"
#include "../clib/luaaux.h"
#include "../vm/luagc.h"

#include <time.h>

#define ELEMENTNUM 5

void p2_test_main(void)
{
    struct lua_State *L = luaL_newstate();

    int i;

    for (i = 0; i < ELEMENTNUM; i++)
	luaL_pushnil(L);

    time_t start_time = time(NULL), end_time;
    size_t max_bytes;
    struct global_State *g = G(L);
    int j;

    for (max_bytes = 0, j = 0; j < 50000000; j++) {
	TValue *o = luaL_index2addr(L, (j % ELEMENTNUM) + 1);
	struct GCObject *gco = luaC_newobj(L, LUA_TSTRING, sizeof(TString));
	o->value_.gc = gco;
	o->tt_ = LUA_TSTRING;
	luaC_checkgc(L);

	if (max_bytes < (g->totalbytes + g->GCdebt))
	    max_bytes = g->totalbytes + g->GCdebt;

	if (j % 1000 == 0)
            printf("timestamp: %d totalbytes: %f Kb\n",
		    (int)time(NULL), (float)(g->totalbytes + g->GCdebt) / 1024.0f);
    }

    end_time = time(NULL);
    printf("finish test start_time: %d, end_time: %d, max_bytes: %f Kb\n",
	    (int)start_time, (int)end_time, (float)max_bytes / 1024.0f);
    printf("sizeof(TString) = %zu\n", sizeof(TString));

    luaL_close(L);
}

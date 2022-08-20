#include "p3_test.h"
#include "../clib/luaaux.h"
#include "../vm/luagc.h"
#include "../common/luastring.h"
#include <time.h>

#define ELEMENTNUM 5

static const char g_shrstr[MAXSHORTSTR] = "This is a short string";
static const char g_lngstr[] = "This is a long string."
			" This is a long string."
			" This is a long string."
			" This is a long string."
			;

static int lprint(struct lua_State *L)
{
    const char *str = luaL_tostring(L, 1);
    fprintf(stdout, "%s\n", str);
    return 0;
}

static int ldummyprint(struct lua_State *L)
{
    const char *str = luaL_tostring(L, 1);
    assert(strcmp(g_lngstr, str) == 0);

    return 0;
}

static void test_print(struct lua_State *L, const char *str)
{
    luaL_pushcfunction(L, lprint);
    luaL_pushstring(L, str);
    luaL_pcall(L, 1, 0);
}

static void test_internal(struct lua_State *L, const char *str, bool is_variant)
{
    fprintf(stdout, "test_internal, str: '%s', is_variant: %s\n",
	    str, is_variant ? "true" : "false");

    struct GCObject *previous = NULL;

    for (int i = 0; i < 5; i++) {
	if (is_variant && strlen(str) <= MAXSHORTSTR) {
	    char buf[256];
	    snprintf(buf, sizeof(buf), "%s %d", str, i);
	    luaL_pushstring(L, buf);
	} else
	    luaL_pushstring(L, str);

	TValue *addr = luaL_index2addr(L, -1);
	if (previous == addr->value_.gc)
	    fputs("The same\n", stdout);
	else
	    fputs("Not the same\n", stdout);

	previous = addr->value_.gc;
    }
}

static void test_string_cache(struct lua_State *L, bool is_same_str)
{
    fprintf(stdout, "test_string_cache, is_same_str: %s\n", is_same_str ? "true" : "false");

    struct TString *previous_ts = NULL;

    for (int i = 0; i < 5; i++) {
	struct TString *ts;

	if (is_same_str)
	    ts = luaS_new(L, g_lngstr, strlen(g_lngstr));
	else {
	    char *buf = (char *)malloc(sizeof(g_lngstr));
	    printf("buff addr %#lx\n", (uintptr_t)buf);
	    memcpy(buf, g_lngstr, sizeof(g_lngstr));
	    ts = luaS_new(L, buf, strlen(buf));
	    free(buf);
	}

	if (previous_ts == ts)
	    fputs("string cache is same\n", stdout);
	else
	    fputs("string cache is not same\n", stdout);

	previous_ts = ts;
    }
}

static void test_gc(struct lua_State *L)
{
    time_t start_time = time(NULL), end_time;
    size_t max_bytes = 0;
    struct global_State *g = G(L);

    for (int j = 0; j < 5000; j++) {
	luaL_pushcfunction(L, ldummyprint);
	luaL_pushstring(L, g_lngstr);
	luaL_pcall(L, 1, 0);
	luaC_checkgc(L);

	if (max_bytes < (g->totalbytes + g->GCdebt))
	    max_bytes = (g->totalbytes + g->GCdebt);

	if (j % 1000 == 0)
            printf("timestamp: %d totalbytes: %f Kb\n",
		    (int)time(NULL), (float)(g->totalbytes + g->GCdebt) / 1024.0f);
    }

    end_time = time(NULL);
    printf("finish test start_time: %d end_time: %d max_bytes: %f Kb\n",
	    (int)start_time, (int)end_time, (float)max_bytes / 1024.0f);
}

void p3_test_main(void)
{
    printf("short string len: %zu, long string len: %zu\n", strlen(g_shrstr), strlen(g_lngstr));

    struct lua_State *L = luaL_newstate();

    test_print(L, g_shrstr);
    test_print(L, g_lngstr);

    test_internal(L, g_shrstr, false);
    test_internal(L, g_shrstr, true);
    test_internal(L, g_lngstr, false);

    test_string_cache(L, true);
    test_string_cache(L, false);

    test_gc(L);

    luaL_close(L);
}

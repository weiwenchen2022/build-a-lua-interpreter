#pragma once

#include "../common/luastate.h"

// GCState
#define GCSpause 0
#define GCSpropagate 1
#define GCSatomic 2
#define GCSinsideatomic 3
#define GCSsweepallgc 4
#define GCSsweepend 5

// Color
#define WHITE0BIT 0
#define WHITE1BIT 1
#define BLACKBIT 2

// Bit operation
#define bitmask(b) (1 << b)
#define bit2mask(b1, b2) (bitmask(b1) | bitmask(b2))

#define resetbits(x, m) ((x) &= cast(lu_byte, ~(m)))
#define setbits(x, m) ((x) |= (m))
#define testbits(x, m) ((x) & (m))
#define resetbit(x, b) resetbits(x, bitmask(b))
#define l_setbit(x, b) setbits(x, bitmask(b))
#define testbit(x, b) testbits(x, bitmask(b))

#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)
#define luaC_white(g) (g->currentwhite & WHITEBITS)
#define otherwhite(g) (g->currentwhite ^ WHITEBITS)

#define iswhite(o) testbits((o)->marked, WHITEBITS)
#define isgray(o) (!testbits((o)->marked, bitmask(BLACKBIT) | WHITEBITS))
#define isblack(o) testbit((o)->marked, BLACKBIT)
#define isdeadm(ow, m) (!((m ^ WHITEBITS) & (ow)))
#define isdead(g, o) isdeadm(otherwhite(g), (o)->marked)
#define changewhite(o) ((o)->marked ^= WHITEBITS)

#define obj2gco(o) (&cast(union GCUnion *, o)->gc)
#define gco2th(o) check_exp(LUA_TTHREAD == (o)->tt_, &cast(union GCUnion *, o)->th)
#define gco2ts(o) check_exp(LUA_SHRSTR == (o)->tt_ || LUA_LNGSTR == (o)->tt_, &cast(union GCUnion *, o)->ts)
#define gcvalue(o) ((o)->value_.gc)

#define iscollectable(o) \
    (LUA_TTHREAD == (o)->tt_ \
     || LUA_SHRSTR == (o)->tt_ \
     || LUA_LNGSTR == (o)->tt_)

#define markobject(L, o) do { \
    if (iswhite(o)) \
	reallymarkobject(L, obj2gco(o)); \
} while (0)

#define markvalue(L, o) do { \
    if (iscollectable(o) && iswhite(gcvalue(o))) \
	reallymarkobject(L, gcvalue(o)); \
} while (0)

#define linkgclist(gco, prev) do { \
    (gco)->gclist = prev; \
    prev = obj2gco(gco); \
} while (0)

// try trigger gc
#define luaC_condgc(pre, L, pos) do { \
    if (G(L)->GCdebt > 0) { pre; luaC_step(L); pos; } \
} while (0)

#define luaC_checkgc(L) luaC_condgc((void)0, L, (void)0)

struct GCObject *luaC_newobj(struct lua_State *L, lu_byte tt_, size_t size);
void luaC_step(struct lua_State *L);
void luaC_fix(struct lua_State *L, struct GCObject *o); // GCObject can not collect
void reallymarkobject(struct lua_State *L, struct GCObject *gco);
void luaC_freeallobjects(struct lua_State *L);

#pragma once

#include "../common/luastate.h"
#include "../common/luatable.h"

#define luaV_fastget(L, t, k, get, slot) \
    (!ttistable(t) ? (slot = NULL, 0) : \
     (slot = get(L, t, k), !ttisnil(slot)))

#define luaV_gettable(L, t, k, v) do { \
    const TValue *slot = NULL; \
    if (luaV_fastget(L, t, k, luaH_get, slot)) \
	setobj(v, cast(TValue *, slot)); \
    else \
	luaV_finishget(L, t, v, slot); \
} while (0)

#define luaV_fastset(L, t, k, v, get, slot) \
    (!ttistable(t) ? (slot = NULL, 0) : \
     (slot = cast(TValue *, get(L, t, k)), \
      (ttisnil(slot) ? (0) : \
       (setobj(slot, v), \
	luaC_barrierback(L, t, slot), 1))))

#define luaV_settable(L, t, k, v) do { \
    TValue *slot = NULL; \
    if (!luaV_fastset(L, t, k, v, luaH_get, slot)) \
	luaV_finishset(L, t, k, v, slot); \
} while (0)

void luaV_finishget(struct lua_State *L, struct Table *t, StkId val, const TValue *slot);
void luaV_finishset(struct lua_State *L, struct Table *t, const TValue *key, StkId val, const TValue *slot);
int luaV_eqobject(struct lua_State *L, const TValue *a, const TValue *b);

void luaV_execute(struct lua_State *L);

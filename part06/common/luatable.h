#pragma once

#include "luastate.h"

#define isdummy(t) (NULL == (t)->lastfree)
#define getkey(n) (cast(const TValue *, &n->key.tvk))
#define getwkey(n) (cast(TValue *, &n->key.tvk))
#define getval(n) (cast(TValue *, &n->value))
#define getnode(t, i) (&(t)->node[i])

#define hashint(key, t) getnode(t, lmod(key, twoto(t->lsizenode)))
#define hashstr(ts, t) getnode(t, lmod(ts->hash, twoto(t->lsizenode)))
#define hashboolean(key, t) getnode(t, lmod(key, twoto(t->lsizenode)))
#define hashpointer(p, t) getnode(t, lmod(point2uint(p), twoto(t->lsizenode)))

struct Table *luaH_new(struct lua_State *L);
void luaH_free(struct lua_State *L, struct Table *t);

// if luaH_getint() return luaO_nilobject, that means key not exists
const TValue *luaH_getint(struct lua_State *L, struct Table *t, int key);
int luaH_setint(struct lua_State *L, struct Table *t, int key, const TValue *value);

const TValue *luaH_getshrstr(struct lua_State *L, struct Table *t, struct TString *key);
const TValue *luaH_getstr(struct lua_State *L, struct Table *t, struct TString *key);

const TValue *luaH_get(struct lua_State *L, struct Table *t, const TValue *key);
TValue *luaH_set(struct lua_State *L, struct Table *t, const TValue *key);

int luaH_resize(struct lua_State *L, struct Table *t, unsigned int asize, unsigned int hsize);
TValue *luaH_newkey(struct lua_State *L, struct Table *t, const TValue *key);

int luaH_next(struct lua_State *L, struct Table *t, TValue *key);
int luaH_getn(struct lua_State *L, struct Table *t);

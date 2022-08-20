#pragma once

#include "../common/luaobject.h"

#define sizeofLClosure(n) (sizeof(LClosure) + ((n) - 1) * sizeof(TValue *))
#define sizeofCClosure(n) (sizeof(CClosure) + ((n) - 1) * sizeof(TValue *))
#define upisopen(up) (&up->u.value != up->v)

struct UpVal {
    TValue *v; // point to stack or its own value (when open)
    int refcount;
    union {
	struct UpVal *next; // next open upvalue
	TValue value; // its value (when closed)
    } u;
};

Proto *luaF_newproto(struct lua_State *L);
void luaF_freeproto(struct lua_State *L, Proto *f);
lu_mem luaF_sizeproto(struct lua_State *L, Proto *f);

LClosure *luaF_newLclosure(struct lua_State *L, int nup);
void luaF_freeLclosure(struct lua_State *L, LClosure *cl);

CClosure *luaF_newCclosure(struct lua_State *L, lua_CFunction func, int nup);
void luaF_freeCclosure(struct lua_State *L, CClosure *cc);

void luaF_initupvals(struct lua_State *L, LClosure *cl);

#pragma once

#include "lua.h"

typedef struct lua_State lua_State;
typedef void *(*lua_Alloc)(void *ud, void *ptr, size_t osize, size_t nsize);

typedef LUA_INTEGER lua_Integer;
typedef LUA_NUMBER lua_Number;
typedef unsigned char lu_byte;
typedef int (*lua_CFunction)(lua_State *L);

// lua number type
#define LUA_NUMINT (LUA_TNUMBER | (0 << 4))
#define LUA_NUMFLT (LUA_TNUMBER | (1 << 4))

// lua function type
#define LUA_TLCL (LUA_TFUNCTION | (0 << 4))
#define LUA_TLCF (LUA_TFUNCTION | (1 << 4))
#define LUA_TCCL (LUA_TFUNCTION | (2 << 4))

// string type
#define LUA_LNGSTR (LUA_TSTRING | (0 << 4))
#define LUA_SHRSTR (LUA_TSTRING | (1 << 4))

// GCObject
#define CommonHeader \
    struct GCObject *next; \
    lu_byte tt_; \
    lu_byte marked

#define LUA_GCSTEPMUL 200

struct GCObject {
    CommonHeader;
};

typedef union lua_Value {
    struct GCObject *gc;
    void *p;
    int b;
    lua_Integer i;
    lua_Number n;
    lua_CFunction f;
} Value;

typedef struct lua_TValue {
    Value value_;
    int tt_;
} TValue;

typedef struct TString {
    int test_field[2];
    // int test_field2;
} TString;

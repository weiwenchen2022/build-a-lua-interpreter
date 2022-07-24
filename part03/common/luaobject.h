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

#define luaO_nilobject (&luaO_nilobject_)
#define MAXSHORTSTR 40

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

const TValue luaO_nilobject_;

typedef struct TString {
    CommonHeader;
    unsigned int hash; // string hash value

    // if TString is long string type, then extra = 1 means it has been hash,
    // extra = 0 means it has not hash yet. if TString is short string type,
    // then extra = 0 means it can be reclaim by gc, or if extra is not 0,
    // gc can not reclaim it.
    unsigned short extra;
    unsigned short shrlen;

    union {
        struct TString *hnext; // only for short string, if two different string encounter hash collision, then chain them
	size_t lnglen;
    } u;

    char data[0];
} TString;

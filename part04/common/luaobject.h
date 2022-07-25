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

#define dummynode (&dummynode_)
#define twoto(lsize) (1 << lsize)
#define lmod(hash, size) check_exp((size) & (size - 1) == 0, (hash) & (size - 1))

#define lua_numeq(a, b) ((a) == (b))
#define lua_numisnan(a) (!lua_numeq(a, a))

#define lua_numbertointeger(n, p) \
    (cast(lua_Number, INT_MIN) <= n) \
     && (n <= cast(lua_Number, INT_MAX)) \
     && ((*p = cast(lua_Integer, n)), 1)

#define ttisinteger(o) (LUA_NUMINT == (o)->tt_)
#define ttisnumber(o) (LUA_NUMFLT == (o)->tt_)
#define ttisshrstr(o) (LUA_SHRSTR == (o)->tt_)
#define ttislngstr(o) (LUA_LNGSTR == (o)->tt_)
#define ttisdeadkey(o) (LUA_TDEADKEY == (o)->tt_)
#define ttistable(o) (LUA_TTABLE == (o)->tt_)
#define ttisnil(o) (LUA_TNIL == (o)->tt_)

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

extern const TValue luaO_nilobject_;

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    char data[0];
#pragma GCC diagnostic pop
} TString;

// lua Table
typedef union TKey {
    struct {
	Value value_;
	int tt_;
	int next;
    } nk;

    TValue tvk;
} TKey;

typedef struct Node {
    TKey key;
    TValue value;
} Node;

extern const Node dummynode_;

struct Table {
    CommonHeader;

    TValue *array;
    unsigned int arraysize;

    Node *node;
    unsigned int lsizenode; // real hash size is 2 ^ lsizenode

    Node *lastfree;
    struct GCObject *gclist;
};

int luaO_ceillog2(int value);

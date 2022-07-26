#pragma once

#include "luaobject.h"

#define LUA_EXTRASPACE sizeof(void *)
#define G(L) ((L)->l_G)

#define STEPMULADJ 200
#define GCSTEPSIZE 1024 // 1Kb
#define GCPAUSE 100

// size for string cache
#define STRCACHE_M 53
#define STRCACHE_N 2

#define LUA_MAINTHREADIDX 0
#define LUA_GLOBALTBLIDX 1
#define LUA_REGISTRYINDEX (-LUA_MAXSTACK - 1)

typedef TValue *StkId;

struct CallInfo {
    StkId func;
    StkId top;

    struct { // only for lua
	StkId base;
	const Instruction *savedpc;
    } l;

    int nresult;
    int callstatus;
    struct CallInfo *next;
    struct CallInfo *previous;
};

typedef struct lua_State {
    CommonHeader; // gc header, all gcobject should have the commonheader
    StkId stack;
    StkId stack_last;
    StkId top;
    int stack_size;

    struct lua_longjmp *errorjmp;
    int status;
    struct lua_State *previous;

    struct CallInfo base_ci;
    struct CallInfo *ci;
    int nci;
    struct global_State *l_G;
    ptrdiff_t errorfunc;
    int ncalls;
    struct GCObject *gclist;
} lua_State;

// only for short string
struct stringtable {
    struct TString **hash;
    unsigned int nuse;
    unsigned int size;
};

typedef struct global_State {
    struct lua_State *mainthread;
    lua_Alloc frealloc;
    void *ud;
    lua_CFunction panic;

    struct stringtable strt;
    TString *strcache[STRCACHE_M][STRCACHE_N];
    unsigned int seed; // hash seed, just for string hash
    TString *memerrmsg;
    TValue l_registry;

    // gc fields
    lu_byte gcstate;
    lu_byte currentwhite;
    struct GCObject *allgc; // gc root set
    struct GCObject **sweepgc;
    struct GCObject *gray;
    struct GCObject *grayagain;
    struct GCObject *fixgc; // objects can not collect by gc
    lu_mem totalbytes;
    l_mem GCdebt; // GCdebt will be negative
    lu_mem GCmemtrav; // per gc step traverse memory bytes
    lu_mem GCestimate; // after finish a gc cycle,it records total memory bytes (totalbytes + GCdebt)
    int GCstepmul;
} global_State;

// GCUnion
union GCUnion {
    struct GCObject gc;

    lua_State th;
    TString ts;
    struct Table tbl;
    Closure cl;
    Proto p;
};

struct lua_State *lua_newstate(lua_Alloc alloc, void *ud);
void lua_close(lua_State *L);

void setivalue(StkId target, int integer);
void setfvalue(StkId target, lua_CFunction f);
void setfltvalue(StkId target, float number);
void setbvalue(StkId target, bool b);
void setnilvalue(StkId target);
void setpvalue(StkId target, void *p);
void setgco(StkId target, struct GCObject *gco);
void setlclvalue(StkId target, struct LClosure *cl);
void setcclosure(StkId target, struct CClosure *cc);

void setobj(StkId target, StkId value);

void increase_top(struct lua_State *L);

void lua_pushcfunction(struct lua_State *L, lua_CFunction f);
void lua_pushCclosure(struct lua_State *L, lua_CFunction f, int nup);
void lua_pushinteger(struct lua_State *L, int integer);
void lua_pushnumber(struct lua_State *L, float number);
void lua_pushboolean(struct lua_State *L, bool b);
void lua_pushnil(struct lua_State *L);
void lua_pushlightuserdata(struct lua_State *L, void *p);
void lua_pushstring(struct lua_State *L, const char *str);

// create a new table, and push it into the stack
int lua_createtable(struct lua_State *L);

// t[k] = v, the table t is index by idx argument, and k, v are the values beyond t, it will trigger metamethod
int lua_settable(struct lua_State *L, int idx);

// get t[k] as the result, k is in the top value of stack and t[k] will replace it, this function will trigger metamethod
int lua_gettable(struct lua_State *L, int idx);

// set t[k] = v, v is the top value
int lua_setfield(struct lua_State *L, int idx, const char *k);

// push the value at the stack indexed by idx onto the top
int lua_pushvalue(struct lua_State *L, int idx);

// get _G and push it into the stack
int lua_pushglobaltable(struct lua_State *L);

// get a field from _G, and push it onto stack
int lua_getglobal(struct lua_State *L, const char *name);

// remove the value indexed by 'idx', and all the values beyond it will be moved down
int lua_remove(struct lua_State *L, int idx);

// insert o into the position of stack indexed by idx, and all the values beyond it will be moved up
int lua_insert(struct lua_State *L, int idx, TValue *v);

lua_Integer lua_tointegerx(struct lua_State *L, int idx, int *isnum);
lua_Number lua_tonumberx(struct lua_State *L, int idx, int *isnum);
bool lua_toboolean(struct lua_State *L, int idx);
int lua_isnil(struct lua_State *L, int idx);
const char *lua_tostring(struct lua_State *L, int idx);

void lua_settop(struct lua_State *L, int idx);
int lua_gettop(struct lua_State *L);
void lua_pop(struct lua_State *L);
TValue *index2addr(struct lua_State *L, int idx);

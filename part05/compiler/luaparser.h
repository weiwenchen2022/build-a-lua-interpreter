#pragma once

#include "../common/luaobject.h"
#include "../compiler/luazio.h"
#include "lualexer.h"

typedef enum expkind {
    VVOID, // expression is void
    VNIL, // expression is nil value
    VFLT, // expression is float value
    VINT, // expression is integer value
    VTRUE, // expression is true value
    VFALSE, // expression is false value
    VINDEXED, // ind field of struct expdesc is in use
    VCALL, // expression is a function call, info field of struct expdesc is represent instruction pc
    VLOCAL, // expression is a local value, info field of struct expdesc is represent the pos of the stack
    VUPVAL, // expression is a upvalue, ind is in use
    VK, // expression is a constant, info field of struct expdesc is represent the index of k
    VRELOCATE, // expression can put result in any register, info field represent the instruction pc
    VNONRELOC, // expression has result in a register, info field represent the pos of the stack
} expkind;

typedef struct expdesc {
    expkind k; // expkind
    union {
	int info;
	lua_Integer i; // for VINT
	lua_Number r; // for VFLT

	struct {
	    int t; // the index of the upvalue or table
	    int vt; // whether 't' is a upvalue(VUPVAL) or table(VLOCAL)
	    int idx; // index (R/K)
	} ind;
    } u;
} expdesc;

// Token cache
typedef struct MBuffer {
    char *buffer;
    int n;
    int size;
} MBuffer;

typedef struct Dyndata {
    struct {
	short *arr;
	int n;
	int size;
    } actvar;
} Dyndata;

typedef struct FuncState {
    int firstlocal;
    struct FuncState *prev;
    struct LexState *ls;
    Proto *p;
    int pc; // next code array index to save instruction
    int nk; // next constant array index to save const value
    int nups; // next upvalues array index to save upval
    int nlocvars; // the number of local values
    int nactvars; // the number of activate values
    int np; // the number of protos
    int freereg; // free register index
} FuncState;

void test_lexer(struct lua_State *L, LexState *ls);

LClosure *luaY_parser(struct lua_State *L, Zio *zio, MBuffer *buffer, Dyndata *dyd, const char *name);

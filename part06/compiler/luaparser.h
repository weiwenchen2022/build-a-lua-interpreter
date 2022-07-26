#pragma once

#include "../common/luaobject.h"
#include "../compiler/luazio.h"

#define UNOPR_PRIORITY 12

typedef enum UnOpr {
    UNOPR_MINUS,
    UNOPR_LEN,
    UNOPR_BNOT,
    UNOPR_NOT,

    NOUNOPR,
} UnOpr;

typedef enum BinOpr {
    BINOPR_ADD = 0,
    BINOPR_SUB,
    BINOPR_MUL,
    BINOPR_DIV,
    BINOPR_IDIV,
    BINOPR_MOD,
    BINOPR_POW,

    BINOPR_BAND,
    BINOPR_BOR,
    BINOPR_BXOR,

    BINOPR_SHL,
    BINOPR_SHR,

    BINOPR_CONCAT,

    BINOPR_GREATER,
    BINOPR_LESS,
    BINOPR_EQ,
    BINOPR_GREATEQ,
    BINOPR_LESSEQ,
    BINOPR_NOTEQ,

    BINOPR_AND,
    BINOPR_OR,

    NOBINOPR,
} BinOpr;

typedef enum expkind {
    VVOID, // expression is void
    VNIL, // expression is nil value
    VFLT, // expression is float value
    VINT, // expression is integer value
    VTRUE, // expression is true value
    VFALSE, // expression is false value
    VCALL, // expression is a function call, info field of struct expdesc is represent instruction pc
    VLOCAL, // expression is a local value, info field of struct expdesc is represent the pos of the stack
    VUPVAL, // expression is a upvalue, ind is in use
    VINDEXED, // ind field of struct expdesc is in use
    VK, // expression is a constant, info field of struct expdesc is represent the index of k
    VJMP,
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

    int t; // patch list of 'exit when true'
    int f; // patch list of 'exit when false'
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
    int jpc; // the position of the first test instruction
    int last_target;
    int nk; // next constant array index to save const value
    int nups; // next upvalues array index to save upval
    int nlocvars; // the number of local values
    int nactvars; // the number of activate values
    int np; // the number of protos
    int freereg; // free register index
} FuncState;

typedef struct LH_assign {
    struct LH_assign *prev;
    expdesc v;
} LH_assign;

LClosure *luaY_parser(struct lua_State *L, Zio *zio, MBuffer *buffer, Dyndata *dyd, const char *name);

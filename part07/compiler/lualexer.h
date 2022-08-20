#pragma once

#include "../common/lua.h"
#include "../common/luaobject.h"
#include "luazio.h"

// 1~256 should reserve for ASCII character token
enum RESERVED {
    FIRST_REVERSED = 257,

    /* terminal token donated by reserved word */
    TK_LOCAL = FIRST_REVERSED,
    TK_NIL,
    TK_TRUE,
    TK_FALSE,
    TK_END,
    TK_THEN,
    TK_IF,
    TK_ELSEIF,
    TK_NOT,
    TK_AND,
    TK_OR,
    TK_FUNCTION,

    NUM_RESERVED = TK_FUNCTION - FIRST_REVERSED + 1,

    /* other token */
    TK_STRING = NUM_RESERVED,
    TK_NAME,
    TK_FLOAT,
    TK_INT,
    TK_NOTEQUAL,
    TK_EQUAL,
    TK_GREATEREQUAL,
    TK_LESSEQUAL,
    TK_SHL,
    TK_SHR,
    TK_MOD,
    TK_DOT,
    TK_VARARG,
    TK_CONCAT,
    TK_EOS,
};

extern const char *luaX_tokens[NUM_RESERVED];

typedef union Seminfo {
    lua_Number r;
    lua_Integer i;
    TString *s;
} Seminfo;

typedef struct Token {
    int token; // token enum value
    Seminfo seminfo; // token info
} Token;

typedef struct LexState {
    Zio *zio; // get a char from stream
    int current; // current char in file
    struct MBuffer *buff; // we cache a series of characters into buff, and recognize which token it is
    Token t; // current token
    Token lookahead;
    int linenumber;
    struct Dyndata *dyd;
    struct FuncState *fs;
    lua_State *L;
    TString *source;
    TString *env;
    struct Table *h; // In order to fast lookup the indexes of constants in a proto, we use a hash table to cache them
} LexState;

void luaX_init(struct lua_State *L);
void luaX_setinput(struct lua_State *L, LexState *ls, Zio *z, struct MBuffer *buffer, struct Dyndata *dyd, TString *source, TString *env);
int luaX_lookahead(struct lua_State *L, LexState *ls);
int luaX_next(struct lua_State *L, LexState *ls);
void luaX_syntaxerror(struct lua_State *L, LexState *ls, const char *error_text);

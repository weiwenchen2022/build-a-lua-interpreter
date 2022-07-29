#pragma once

#include <setjmp.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#if defined(LLONG_MAX)
#define LUA_INTEGER long
#define LUA_NUMBER double

#define LUA_INTEGER_FORMAT "%ld"
#define LUA_NUMBER_FORMAT "%f"
#else
#define LUA_INTEGER int
#define LUA_NUMBER float

#define LUA_INTEGER_FORMAT "%d"
#define LUA_NUMBER_FORMAT "%f"
#endif

#define LUA_ENV "_ENV"
#define LUA_LOADED "_LOADED"

#define LUA_UNSIGNED unsigned LUA_INTEGER

#define lua_assert(c) ((void)0)
#define check_exp(c, e) (lua_assert(c), e)

// ERROR CODE
#define LUA_OK 0
#define LUA_ERRERR 1
#define LUA_ERRMEM 2
#define LUA_ERRRUN 3
#define LUA_ERRLEXER 4

#define cast(t, exp) ((t)(exp))
#define savestack(L, o) ((o) - (L)->stack)
#define restorestack(L, o) ((L)->stack + (o))
#define point2uint(p) ((unsigned int)((uintptr_t)(p) & UINT_MAX))
#define novariant(o) ((o)->tt_ & 0xf)

// basic object type
#define LUA_TNUMBER 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TBOOLEAN 3
#define LUA_TSTRING 4
#define LUA_TNIL 5
#define LUA_TTABLE 6
#define LUA_TFUNCTION 7
#define LUA_TTHREAD 8
#define LUA_TPROTO 9
#define LUA_TNONE 10
#define LUA_NUMS LUA_TNONE
#define LUA_TDEADKEY (LUA_NUMS + 1)

// stack define
#define LUA_MINSTACK 20
#define LUA_STACKSIZE (2 * LUA_MINSTACK)
#define LUA_EXTRASTACK 5
#define LUA_MAXSTACK 15000
#define LUA_ERRORSTACK 200
#define LUA_MULRET -1
#define LUA_MAXCALLS 200

// error tips
#define LUA_ERROR(L, s) fprintf(stderr, "LUA ERROR: %s", s)

// mem define
typedef size_t lu_mem;
typedef ptrdiff_t l_mem;

typedef LUA_UNSIGNED lua_Unsigned;

// vm
typedef int Instruction;

#define MAX_LUMEM ((lu_mem)(~(lu_mem)0))
#define MAX_LMEM (MAX_LUMEM >> 1)
#define BUFSIZE 512

// IO
#define lua_writestring(s) fwrite((s), strlen(s) * sizeof(char), 1, stdout)
#define lua_writeline() (fputs("\n", stdout), fflush(stdout))

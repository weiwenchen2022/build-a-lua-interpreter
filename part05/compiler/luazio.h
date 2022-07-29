#pragma once

#include "../common/lua.h"
#include "../common/luaobject.h"

typedef char *(*lua_Reader)(struct lua_State *L, void *data, size_t *size);

#define MIN_BUFF_SIZE 32
#define zget(z) ((z)->n-- > 0 ? (*(z)->p++) : luaZ_fill(z))
#define luaZ_resetbuffer(ls) do { (ls)->buff->n = 0; } while (0)
#define luaZ_buffersize(ls) ((ls)->buff->size)

typedef struct LoadF {
    FILE *f;
    char buff[BUFSIZE]; // read the file stream into buff
    int n; // how many char you have read
} LoadF;

typedef struct Zio {
    lua_Reader reader; // read buffer to p
    int n; // the number of unused bytes
    char *p; // the pointer to buffer
    void *data; // structure which holds FILE handler
    struct lua_State *L;
} Zio;

void luaZ_init(struct lua_State *L, Zio *zio, lua_Reader reader, void *data);

// if fill success, then it will return next character in ASCII table, or it will return -1
int luaZ_fill(Zio *z);

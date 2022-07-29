#include "p5_test.h"
#include "../common/lua.h"
#include "../clib/luaaux.h"
#include "../compiler/luazio.h"
#include "../compiler/luaparser.h"
#include "../common/luastring.h"

static char *getF(struct lua_State *L, void *data, size_t *sz)
{
    LoadF *lf = (LoadF *)data;

    if (lf->n > 0) {
	*sz = lf->n;
	lf->n = 0;
    } else {
	*sz = fread(lf->buff, sizeof(char), BUFSIZE, lf->f);
	lf->n = 0;
    }

    return lf->buff;
}

typedef struct SParser {
    MBuffer buffer;
    Dyndata dyd;
    Zio *z;
    const char *filename;
} SParser;

// skip utf-8 BOM
static int skipBOM(LoadF *lf) {
    const char *bom = "\xEF\xBB\xBF";

    do {
	int c = fgetc(lf->f);
	if (EOF == c || c != *bom)
	    return c;

	lf->buff[lf->n++] = c;
	bom++;
    } while (*bom != '\0');
    
    lf->n = 0;
    return fgetc(lf->f);
}

// skip first line comment, if it exist
static int skipcomment(LoadF *lf, int *c) {
    *c = skipBOM(lf);
    if (*c == '#') { // Unix exec file?
	do {
	    *c = getc(lf->f);
	} while (*c != '\n' && *c != EOF);

	return 1;
    }

    return 0;
}

void p5_test_main(void)
{
    struct lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    int ok = luaL_loadfile(L, "./scripts/part05_test.lua");

    if (LUA_OK == ok)
        luaL_pcall(L, 0, 0);

    luaL_close(L);

    // const char *filename = "./scripts/part05_test.lua";

    // FILE *fp;
    // l_fopen(&fp, filename, "rb");

    // LoadF lf;
    // lf.f = fp;
    // lf.n = 0;
    // memset(lf.buff, 0, BUFSIZE);

    // int c;
    // skipcomment(&lf, &c);
    // lf.buff[lf.n++] = c;

    // Zio z;
    // struct lua_State *L = luaL_newstate();
    // luaZ_init(L, &z, getF, &lf);

    // SParser p;
    // p.filename = filename;
    // p.z = &z;
    // p.dyd.actvar.arr = NULL;
    // p.dyd.actvar.n = p.dyd.actvar.size = 0;
    // p.buffer.buffer = NULL;
    // p.buffer.n = p.buffer.size = 0;

    // LexState ls;
    // luaX_setinput(L, &ls, p.z, &p.buffer, &p.dyd, luaS_newliteral(L, filename), luaS_newliteral(L, LUA_ENV));
    // ls.current = zget(ls.zio);
    // test_lexer(L, &ls);

    // fclose(fp);
    // luaL_close(L);
}

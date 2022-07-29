#include "luafunc.h"
#include "luagc.h"
#include "../common/luamem.h"
#include "../common/luastate.h"
#include "../common/luaobject.h"

Proto *luaF_newproto(struct lua_State *L)
{
    struct GCObject *gco = luaC_newobj(L, LUA_TPROTO, sizeof(Proto));
    Proto *f = gco2proto(gco);
    f->code = NULL;
    f->sizecode = 0;
    f->is_vararg = 0;
    f->k = NULL;
    f->sizek = 0;
    f->locvars = NULL;
    f->sizelocvar = 0;
    f->nparam = 0;
    f->p = NULL;
    f->sizep = 0;
    f->upvalues = NULL;
    f->sizeupvalues = 0;
    f->source = NULL;
    f->maxstacksize = 0;

    return f;
}

void luaF_freeproto(struct lua_State *L, Proto *f)
{
    if (f->code)
	luaM_free(L, f->code, f->sizecode * sizeof(Instruction));

    if (f->k)
	luaM_free(L, f->k, f->sizek * sizeof(TValue));

    if (f->locvars)
	luaM_free(L, f->locvars, f->sizelocvar * sizeof(LocVar));

    if (f->p)
	luaM_free(L, f->p, f->sizep * sizeof(Proto *));

    if (f->upvalues)
	luaM_free(L, f->upvalues, f->sizeupvalues * sizeof(Upvaldesc));

    luaM_free(L, f, sizeof(Proto));
}

lu_mem luaF_sizeproto(struct lua_State *L, Proto *f)
{
    lu_mem sz = sizeof(struct Proto);
    sz += f->sizek * sizeof(TValue);
    sz += f->sizecode * sizeof(Instruction);
    sz += f->sizelocvar * sizeof(LocVar);
    sz += f->sizep * sizeof(Proto *);
    sz += f->sizeupvalues * sizeof(Upvaldesc);

    return sz;
}

LClosure *luaF_newLclosure(struct lua_State *L, int nup)
{
    struct GCObject *gco = luaC_newobj(L, LUA_TLCL, sizeofLClosure(nup));
    LClosure *cl = gco2lclosure(gco);
    cl->p = NULL;
    cl->nupvalues = nup;

    while (nup--)
	cl->upvals[nup] = NULL;

    return cl;
}

void luaF_freeLclosure(struct lua_State *L, LClosure *cl)
{
    for (int i = 0; i < cl->nupvalues; i++) {
	if (cl->upvals[i] != NULL) {
	    cl->upvals[i]->refcount--;
	    if (cl->upvals[i]->refcount <= 0 && !upisopen(cl->upvals[i])) {
		luaM_free(L, cl->upvals[i], sizeof(UpVal));
		cl->upvals[i] = NULL;
	    }
	}
    }

    luaM_free(L, cl, sizeofLClosure(cl->nupvalues));
}

CClosure *luaF_newCclosure(struct lua_State *L, lua_CFunction func, int nup)
{
    struct GCObject *gco = luaC_newobj(L, LUA_TCCL, sizeofCClosure(nup));
    CClosure *cc = gco2cclosure(gco);
    cc->nupvalues = nup;
    cc->f = func;

    while (nup--)
	cc->upvals[nup] = NULL;

    return cc;
}

void luaF_freeCclosure(struct lua_State *L, CClosure *cc)
{
    luaM_free(L, cc, sizeofCClosure(cc->nupvalues));
}

void luaF_initupvals(struct lua_State *L, LClosure *cl)
{
    for (int i = 0; i < cl->nupvalues; i++) {
	if (cl->upvals[i] == NULL) {
	    cl->upvals[i] = luaM_realloc(L, NULL, 0, sizeof(UpVal));
	    cl->upvals[i]->refcount = 1;
	    cl->upvals[i]->v = &cl->upvals[i]->u.value;
	    setnilvalue(cl->upvals[i]->v);
	}
    }
}

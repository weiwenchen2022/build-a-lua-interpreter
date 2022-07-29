#include "luaobject.h"

const TValue luaO_nilobject_ = {{NULL,}, LUA_TNIL,};
const Node dummynode_;

int luaO_ceillog2(int value)
{
    int x;
    for (x = 0; (int)pow(2, x) < value; x++)
	;

    return x;
}

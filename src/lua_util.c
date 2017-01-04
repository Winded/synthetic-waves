#include <lua_util.h>

int lua_util_udata_is_valid(lua_State *L)
{
    if(!lua_isuserdata(L, 1) || lua_objlen(L, 1) < sizeof(char)) {
        luaL_error(L, "Argument #1 should be userdata of at least 1 byte length");
    }
    char *dptr = (char*)lua_touserdata(L, 1);
    char is_valid = *dptr;
    lua_pushboolean(L, (int)is_valid);
    return 1;
}

int lua_util_udata_ptr_is_valid(lua_State *L)
{
    if(!lua_isuserdata(L, 1) || lua_objlen(L, 1) < sizeof(void*)) {
        luaL_error(L, "Argument #1 should be userdata of at least 4 bytes length");
    }
    char **dptr = (char**)lua_touserdata(L, 1);
    char is_valid = **dptr;
    lua_pushboolean(L, (int)is_valid);
    return 1;
}

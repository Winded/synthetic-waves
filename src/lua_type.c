#include <lua_type.h>

int lua_type_ext(lua_State *L)
{
    if((lua_istable(L, 1) || lua_isuserdata(L, 1)) && lua_getmetatable(L, 1)) {
        lua_getfield(L, -1, "__type");
        if(lua_isstring(L, -1)) {
            return 1;
        }
        else if(lua_isfunction(L, -1)) {
            lua_pushvalue(L, 1);
            lua_call(L, 1, 1);
            luaL_checkstring(L, -1);
            return 1;
        }
    }

    lua_getglobal(L, "_type");
    luaL_checktype(L, -1, LUA_TFUNCTION);
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    return 1;
}

void lua_type_load(lua_State *L)
{
    lua_getglobal(L, "type");
    lua_setglobal(L, "_type");
    lua_pushcfunction(L, lua_type_ext);
    lua_setglobal(L, "type");
}


void lua_type_test(lua_State *L)
{
    int status = luaL_loadfile(L, "../tests/type.lua");
    if(status) {
        fprintf(stderr, "Couldn't load file: %s", lua_tostring(L, -1));
        return 0;
    }

    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if(result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        return 0;
    }
}

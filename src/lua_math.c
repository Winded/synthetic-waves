#include <lua_math.h>

void lua_math_load(lua_State *L)
{
    luaL_openlib(L, "window", lua_window_methods, 0);  /* create methods table, add it to the globals */
    luaL_newmetatable(L, "window");          /* create metatable for Foo, and add it to the Lua registry */
    luaL_openlib(L, 0, lua_window_meta, 0);    /* fill metatable */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -3);               /* dup methods table*/
    lua_rawset(L, -3);                  /* metatable.__index = methods */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);               /* dup methods table*/
    lua_rawset(L, -3);                  /* hide metatable:  metatable.__metatable = methods */
    lua_pop(L, 1);                      /* drop metatable */
}

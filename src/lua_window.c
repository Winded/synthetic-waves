#include <lua_window.h>
#include <GL/glew.h>

window *lua_window_towindow(lua_State *L, int index)
{
    window** w = (window**)lua_touserdata(L, index);
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

window *lua_window_checkwindow(lua_State *L, int index)
{
    window** w;
    luaL_checktype(L, index, LUA_TUSERDATA);
    w = (window**)luaL_checkudata(L, index, "window");
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

int lua_window_create(lua_State *L)
{
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);

    window *w_handle = window_create(title, width, height);

    window** w = (window**)lua_newuserdata(L, sizeof(window*));
    luaL_getmetatable(L, "window");
    lua_setmetatable(L, -2);

    *w = w_handle;

    return 1;
}

int lua_window_getDeltaTime(lua_State *L)
{
    window *w_handle = lua_window_checkwindow(L, 1);
    float = window_get_delta_time(w_handle);
    lua_pushnumber(L, deltaTime);
    return 1;
}

int lua_window_shouldClose(lua_State *L)
{
    window *w_handle = lua_window_checkwindow(L, 1);
    int shouldClose = window_should_close(w_handle) ? 1 : 0;
    lua_pushboolean(L, shouldClose);
    return 1;
}

int lua_window_swapBuffers(lua_State *L)
{
    window *w_handle = lua_window_checkwindow(L, 1);
    window_swap_buffers(w_handle);
    return 0;
}

int lua_window_pollEvents(lua_State *L)
{
    window *w_handle = lua_window_checkwindow(L, 1);
    window_poll_events(w_handle);
    return 0;
}

int lua_window_destroy(lua_State *L)
{
    window *w_handle = lua_window_checkwindow(L, 1);
    window_destroy(&w_handle);
    lua_pushboolean(L, 1);
    return 1;
}

int lua_window_tostring(lua_State *L)
{
    char buff[32];
    sprintf(buff, "%p", lua_window_towindow(L, 1));
    lua_pushfstring(L, "Foo (%s)", buff);
    return 1;
}

void lua_window_load(lua_State *L)
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

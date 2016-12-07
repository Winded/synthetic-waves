/*
 * lua_window.h
 *
 * A LUA interface for window handling (glfw)
 * */

#ifndef lua_window_h
#define lua_window_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <lua.h>
#include <lauxlib.h>

GLFWwindow *lua_window_towindow(lua_State *L, int index);

GLFWwindow *lua_window_checkwindow(lua_State *L, int index);

int lua_window_create(lua_State *L);

int lua_window_shouldClose(lua_State *L);

int lua_window_clear(lua_State *L);

int lua_window_swapBuffers(lua_State *L);

int lua_window_pollEvents(lua_State *L);

int lua_window_destroy(lua_State *L);

int lua_window_tostring(lua_State *L);

/**
 * @brief Load the window library into given state
 * @param L lua context
 */
void lua_window_load(lua_State *L);

static const luaL_reg lua_window_methods[] = {
    {"create", lua_window_create},
    {"shouldClose", lua_window_shouldClose},
    {"clear", lua_window_clear},
    {"swapBuffers", lua_window_swapBuffers},
    {"pollEvents", lua_window_pollEvents},
    {"destroy", lua_window_destroy},
    {0, 0}
};

static const luaL_reg lua_window_meta[] = {
    {"__tostring", lua_window_tostring},
    {0, 0}
};

#endif // lua_window_h

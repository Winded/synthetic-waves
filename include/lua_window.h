/*
 * lua_window.h
 *
 * A LUA interface for window handling
 * */

#ifndef lua_window_h
#define lua_window_h

#include <SDL.h>
#include <lua.h>
#include <lauxlib.h>
#include "window.h"

window *lua_window_to(lua_State *L, int index);

window *lua_window_check(lua_State *L, int index);

void lua_window_push(lua_State *L, const window *w);

/**
 * @brief Load the window library into given state
 * @param L lua context
 */
void lua_window_load(lua_State *L);

#endif // lua_window_h

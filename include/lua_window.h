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

#define LUA_WINDOW_MAX_EVENT_HANDLERS 50

typedef struct lua_window
{
    window w_handle;
    int graphics_context_ref;
    int event_handlers[LUA_WINDOW_MAX_EVENT_HANDLERS];
} lua_window;

lua_window *lua_window_to(lua_State *L, int index);

lua_window *lua_window_check(lua_State *L, int index);

lua_window *lua_window_new(lua_State *L);

/**
 * @brief Load the window library into given state
 * @param L lua context
 */
void lua_window_load(lua_State *L);

#endif // lua_window_h

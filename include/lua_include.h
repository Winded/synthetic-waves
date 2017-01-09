/*
 * lua_include.h
 *
 * include method for Lua. Simpler than the package module: It just runs the lua file.
 * The path is relative to the currently running lua file invoked with include.
 * */
#ifndef lua_include_h
#define lua_include_h

#include <lua.h>
#include <lauxlib.h>

void lua_include_load(lua_State *L);

#endif // lua_include_h

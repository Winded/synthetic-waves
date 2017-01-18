/*
 * lua_graphics.h
 *
 * Graphics library interface for Lua
 * */
#ifndef lua_graphics_h
#define lua_graphics_h

#include <lua.h>
#include <lauxlib.h>
#include <graphics.h>

void lua_graphics_load(lua_State *L);

#endif // lua_graphics_h

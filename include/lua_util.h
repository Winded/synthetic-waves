/*
 * lua_util.h
 *
 * Lua utilities
 * */
#ifndef lua_util_h
#define lua_util_h

#include <lua.h>
#include <lauxlib.h>

int lua_util_udata_is_valid(lua_State *L);

#endif // lua_util_h

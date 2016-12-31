/*
 * lua_type.h
 *
 * Extension for lua's type method to look for __type meta field
 * */
#ifndef lua_type_h
#define lua_type_h

#include <lua.h>
#include <lauxlib.h>

void lua_type_load(lua_State *L);

void lua_type_test(lua_State *L);

#endif // lua_type_h

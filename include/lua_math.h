/*
 * lua_math.h
 *
 * Math extensions for lua, including linmath types
 * */
#ifndef lua_math_h
#define lua_math_h

#include <lua.h>
#include <lauxlib.h>
#include <linmath.h>

void lua_math_load(lua_State *L);

#endif //lua_math_h

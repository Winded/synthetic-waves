/*
 * lua_math.h
 *
 * Math extensions for lua, including linmath types
 * */
#ifndef lua_math_h
#define lua_math_h

#include <lua.h>
#include <lauxlib.h>

int lua_math_vec3_create(lua_State *L);
int lua_math_vec3_length(lua_State *L);

void lua_math_load(lua_State *L);

static const luaL_reg lua_math_vec3_meta[] = {
    {"length", lua_math_vec3_length},
};

#endif //lua_math_h

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

vec2 *lua_math_vec2_to(lua_State *L, int index);
vec2 *lua_math_vec2_check(lua_State *L, int index);
void lua_math_vec2_push(lua_State *L, const vec2 *v);

vec3 *lua_math_vec3_to(lua_State *L, int index);
vec3 *lua_math_vec3_check(lua_State *L, int index);
void lua_math_vec3_push(lua_State *L, const vec3 *v);

vec4 *lua_math_vec4_to(lua_State *L, int index);
vec4 *lua_math_vec4_check(lua_State *L, int index);
void lua_math_vec4_push(lua_State *L, const vec4 *v);

mat4x4 *lua_math_mat4x4_to(lua_State *L, int index);
mat4x4 *lua_math_mat4x4_check(lua_State *L, int index);
void lua_math_mat4x4_push(lua_State *L, const mat4x4 *m);

void lua_math_load(lua_State *L);

#endif //lua_math_h

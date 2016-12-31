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

typedef graphics_context lua_graphics_context;

lua_graphics_context *lua_graphics_to(lua_State *L, int index);
lua_graphics_context *lua_graphics_check(lua_State *L, int index);
lua_graphics_context *lua_graphics_new(lua_State *L);

graphics_shader *lua_graphics_shader_check(lua_State *L, int index);
void lua_graphics_shader_push(lua_State *L, const graphics_shader *shader);

graphics_shader_program *lua_graphics_shader_program_check(lua_State *L, int index);
void lua_graphics_shader_program_push(lua_State *L, const graphics_shader_program *program);

graphics_texture *lua_graphics_texture_check(lua_State *L, int index);
void lua_graphics_texture_push(lua_State *L, const graphics_texture *texture);

graphics_vertex_array *lua_graphics_vertex_array_check(lua_State *L, int index);
void lua_graphics_vertex_array_push(lua_State *L, const graphics_vertex_array *varr);

graphics_object *lua_graphics_object_check(lua_State *L, int index);
void lua_graphics_object_push(lua_State *L, const graphics_object *object);

void lua_graphics_load(lua_State *L);

#endif // lua_graphics_h

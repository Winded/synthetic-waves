/*
 * lua_color.h
 *
 * color structure for lua
 * */
#ifndef lua_color_h
#define lua_color_h

#include <lua.h>
#include <lauxlib.h>

typedef int lua_color[4];

lua_color *lua_color_to(lua_State *L, int index);
lua_color *lua_color_check(lua_State *L, int index);
void lua_color_push(lua_State *L, const lua_color *color);

void lua_color_load(lua_State *L);

#endif // lua_color_h

/*
 * lua_font.h
 *
 * Font loading for Lua. Uses lua assets
 * */
#ifndef lua_font_h
#define lua_font_h

#include <lua.h>
#include <lauxlib.h>

typedef struct lua_font lua_font;

lua_font *lua_font_check(lua_State *L, int index);
lua_font *lua_font_new(lua_State *L);

void lua_font_load_lib(lua_State *L);

#endif // lua_font_h

/*
 * lua_texture.h
 *
 * Texture loading. Uses lua_asset library
 * */
#ifndef lua_texture_h
#define lua_texture_h

#include <lua.h>
#include <lauxlib.h>

typedef struct lua_texture {
        int asset_ref;
        int width, height, num_channels;
        void *data;
        int data_size;
} lua_texture;

lua_texture *lua_texture_check(lua_State *L, int index);
lua_texture *lua_texture_new(lua_State *L);

void lua_texture_load_lib(lua_State *L);

#endif // lua_texture_h

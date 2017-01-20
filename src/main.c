#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <linmath.h>
#include <lua_type.h>
#include <lua_include.h>
#include <lua_color.h>
#include <lua_math.h>
#include <lua_asset.h>
#include <lua_texture.h>
#include <lua_font.h>
#include <lua_window.h>
#include <lua_graphics.h>
#include <lua_input.h>
#include <lua_audio.h>
#include <SDL.h>
#include <window.h>
#include <graphics.h>

void openlibs(lua_State *L)
{
    // Standard lua libraries
    lua_pushcfunction(L, luaopen_base);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_io);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L, 0, 0);
    //lua_pushcfunction(L, luaopen_package);
    //lua_call(L, 0, 0);

    // luajogo libraries. All native libs are contained in the luajogo table
    lua_newtable(L);

    lua_type_load(L);
    lua_include_load(L);

    lua_color_load(L);
    lua_math_load(L);

    lua_input_load(L);

    lua_asset_lib_load(L);
    lua_texture_load_lib(L);
    lua_font_load_lib(L);

    lua_window_load(L);
    lua_graphics_load(L);
    lua_audio_load(L);

    lua_setglobal(L, "luajogo");
}

int main(int argc, char *argv[])
{
    lua_State *L = luaL_newstate();

    openlibs(L);

    lua_type_test(L);
    lua_color_test(L);
    lua_math_test(L);
    lua_asset_lib_test(L);

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("FAIL init");
        return 0;
    }

    lua_getglobal(L, "include");
    lua_pushstring(L, "../main.lua");
    int result = lua_pcall(L, 1, 0, 0);
    if(result) {
        fprintf(stderr, "LUA error on load: %s\n", lua_tostring(L, -1));
        return 0;
    }

    lua_getglobal(L, "main");
    if(!lua_isfunction(L, -1)) {
        fprintf(stderr, "Error: No main() function found in global namespace. Aborting...\n");
        return 0;
    }

    result = lua_pcall(L, 0, 0, 0);
    if(result) {
        fprintf(stderr, "LUA error: %s\n", lua_tostring(L, -1));
        return 0;
    }

    SDL_Quit();

    lua_close(L);

    return 0;
}

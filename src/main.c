#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <linmath.h>
#include <lua_type.h>
#include <lua_color.h>
#include <lua_math.h>
#include <lua_window.h>
#include <lua_graphics.h>
#include <SDL.h>
#include <window.h>
#include <graphics.h>

void openlibs(lua_State *L)
{
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_os(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_debug(L);
    //luaopen_package(L);

    lua_type_load(L);
    lua_color_load(L);
    lua_math_load(L);
    lua_window_load(L);
    lua_graphics_load(L);
}

int main(int argc, char *argv[])
{
    lua_State *L = luaL_newstate();

    openlibs(L);

    lua_color_test(L);
    lua_math_test(L);

    int status = luaL_loadfile(L, "../main.lua");
    if(status) {
        fprintf(stderr, "Couldn't load file: %s", lua_tostring(L, -1));
        return 0;
    }

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("FAIL init");
        return 0;
    }

    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if(result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        return 0;
    }

    SDL_Quit();

    lua_close(L);

    return 0;
}

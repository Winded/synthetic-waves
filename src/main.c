#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <linmath.h>
#include <lua_window.h>
#include <SDL.h>

void print_mat4x4(mat4x4 m)
{
    printf("[%f, %f, %f, %f]\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("[%f, %f, %f, %f]\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("[%f, %f, %f, %f]\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("[%f, %f, %f, %f]\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

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

    lua_window_load(L);
}

int main(int argc, char *argv[])
{
    mat4x4 m, mr;
    mat4x4_identity(m);

    mat4x4_rotate_Z(mr, m, 90.f);

    print_mat4x4(m);
    print_mat4x4(mr);

    lua_State *L = luaL_newstate();

    openlibs(L);

    int status = luaL_loadfile(L, "../test.lua");
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
        fprintf(stderr, "Failed to run script: %s", lua_tostring(L, -1));
        return 0;
    }

    SDL_Quit();

    lua_close(L);

    return 0;
}

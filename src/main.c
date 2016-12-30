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

static const char test_vertex_shader[] = ""
        "#version 330\n"

        "in vec3 position;"
        "in vec2 uvCoordinates;"

        "out vec2 UV;"

        "uniform mat4 WorldToViewportMatrix;"
        "uniform mat4 LocalToWorldMatrix;"

        "void main()"
        "{"
            "gl_Position = WorldToViewportMatrix * LocalToWorldMatrix * vec4(position.xyz, 1.0);"
            "UV = uvCoordinates;"
        "}";

static const char test_fragment_shader[] = ""
        "#version 330\n"

        "in vec2 UV;"

        "out vec4 outColor;"

        "uniform sampler2D Texture1;"
        "uniform vec4 BaseColor;"

        "void main()"
        "{"
            "outColor = texture2D(Texture1, UV) * BaseColor;"
        "}";

static const char test_texture[] = {
    (char)255, (char)255, (char)255, (char)255
};

static const float test_vbo[] = {
    -0.5f, 0.5f, 0, 0, 0,
    0.5f, 0.5f, 0, 0, 1,
    0.5f, -0.5f, 0, 1, 1,
    -0.5f, -0.5f, 0, 1, 0,
};

static const int test_ebo[] = {
    0, 1, 3,
    1, 2, 3
};

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

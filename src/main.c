#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <linmath.h>
#include <lua_window.h>
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

    lua_window_load(L);
}

int main(int argc, char *argv[])
{
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

    //int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    //if(result) {
    //    fprintf(stderr, "Failed to run script: %s", lua_tostring(L, -1));
    //    return 0;
    //}

    window *w = window_create("Test", 640, 480);

    graphics_context *g = graphics_context_create();

    graphics_shader *vertexShader = graphics_shader_create(g, test_vertex_shader, graphics_vertex_shader);
    graphics_shader *fragmentShader = graphics_shader_create(g, test_fragment_shader, graphics_fragment_shader);
    graphics_shader_program *program = graphics_shader_program_create(g, vertexShader, fragmentShader);

    void *texData = malloc(1 * 1 * 4 * sizeof(char));
    memcpy(texData, test_texture, 1 * 1 * 4 * sizeof(char));
    graphics_texture *texture = graphics_texture_create(g, texData, 1, 1);

    graphics_vertex_array *va = graphics_vertex_array_create(g, test_vbo, 5 * 4, test_ebo, 3 * 2);
    graphics_vertex_array_set_attribute(va, 0, 3);
    graphics_vertex_array_set_attribute(va, 1, 2);

    float clearColor[] = {0.5f, 0.5f, 0.5f, 1};

    float orthoScale = 5.f;
    float aspectRatio = (640.f / 480.f);
    float orthoWidth = orthoScale * aspectRatio;
    float orthoHeight = orthoScale;
    mat4x4 projMat;
    mat4x4_identity(projMat);
    //mat4x4_ortho(projMat, -orthoWidth / 2.f, orthoWidth / 2.f, -orthoHeight / 2.f, orthoHeight / 2.f, 0, 100);
    mat4x4_perspective(projMat, 90.f * DEG2RAD, aspectRatio, 0, 100);
    mat4x4 wToVPMat;
    mat4x4_identity(wToVPMat);
    mat4x4_translate(wToVPMat, 0, 1.f, -3.f);
    mat4x4_rotate_X(wToVPMat, wToVPMat, 45.f * DEG2RAD);
    mat4x4_mul(wToVPMat, projMat, wToVPMat);
    graphics_shader_param_set(g, "WorldToViewportMatrix", wToVPMat, 4 * 4);

    mat4x4 lToWMat;
    mat4x4_identity(lToWMat);
    mat4x4 localMat;
    mat4x4_identity(localMat);
    mat4x4_translate(localMat, 1, 0, 0);

    float baseColor[] = {1, 1, 1, 1};
    graphics_shader_param_set(g, "BaseColor", baseColor, 4);

    graphics_object *gObj = graphics_object_create(g);
    gObj->shader_program = program;
    gObj->texture = texture;
    gObj->vertex_array = va;

    graphics_refresh_draw_order(g);

    while(!window_should_close(w)) {
        window_poll_events(w);

        float delta = window_get_delta_time(w);
        mat4x4_rotate_Z(lToWMat, lToWMat, 45.f * DEG2RAD * delta);
        mat4x4 posMat;
        mat4x4_identity(posMat);
        mat4x4_mul(posMat, lToWMat, localMat);
        graphics_object_shader_param_set(gObj, "LocalToWorldMatrix", posMat, 4 * 4);

        graphics_clear(g, clearColor);

        graphics_draw(g);

        window_swap_buffers(w);
    }

    graphics_context_destroy(&g);
    window_destroy(&w);

    SDL_Quit();

    lua_close(L);

    return 0;
}

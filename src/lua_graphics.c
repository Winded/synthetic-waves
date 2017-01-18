#include <lua_graphics.h>
#include <lua_util.h>
#include <lua_math.h>
#include <lua_color.h>
#include <lua_texture.h>
#include <memory.h>

int lua_graphics_is_feature_enabled(lua_State *L)
{
    int feature = luaL_checkinteger(L, 1);
    lua_pushinteger(L, graphics_is_feature_enabled(feature));
    return 1;
}

int lua_graphics_set_feature_enabled(lua_State *L)
{
    int feature = luaL_checkinteger(L, 1);
    int enabled = lua_toboolean(L, 2);
    graphics_set_feature_enabled(feature, enabled);
    return 0;
}

int lua_graphics_create_shader(lua_State *L)
{
    const char *code = luaL_checkstring(L, 1);
    int type = luaL_checkinteger(L, 2);

    char errBuf[2048];
    int handle = graphics_shader_create(code, type, errBuf);
    if(!handle) {
        luaL_error(L, errBuf);
        return 0;
    }

    lua_pushinteger(L, handle);
    return 1;
}

int lua_graphics_delete_shader(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    graphics_shader_destroy(handle);
    return 0;
}

int lua_graphics_create_shader_program(lua_State *L)
{
    int vertex_handle = luaL_checkinteger(L, 1);
    int fragment_handle = luaL_checkinteger(L, 2);

    char errBuf[2048];
    int handle = graphics_shader_program_create(vertex_handle, fragment_handle, errBuf);
    if(!handle) {
        luaL_error(L, errBuf);
        return 0;
    }

    lua_pushinteger(L, handle);
    return 1;
}

int lua_graphics_delete_shader_program(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    graphics_shader_program_destroy(handle);
    return 0;
}

int lua_graphics_use_shader_program(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    graphics_shader_program_use(handle);
    return 0;
}

int lua_graphics_shader_param_name_to_id(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    const char *name = luaL_checkstring(L, 2);
    int id = graphics_shader_param_name_to_id(handle, name);
    if(id > -1) {
        lua_pushinteger(L, id);
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

int lua_graphics_set_shader_param(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);

    int id = 0;
    if(lua_isstring(L, 2)) {
        const char *name = luaL_checkstring(L, 2);
        id = graphics_shader_param_name_to_id(handle, name);
    }
    else {
        id = luaL_checkinteger(L, 2);
    }

    luaL_getmetatable(L, "vec2");
    luaL_getmetatable(L, "vec3");
    luaL_getmetatable(L, "vec4");
    luaL_getmetatable(L, "color");
    luaL_getmetatable(L, "mat4x4");

    if(!lua_isnumber(L, 3) && !lua_getmetatable(L, 3)) {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
        return 0;
    }

    char errBuf[1024];
    int result = 0;

    if(lua_isnumber(L, 3)) {
        float value = lua_tonumber(L, 3);
        result = graphics_shader_param_set(id, &value, 1, errBuf);
    }
    else if(lua_equal(L, -1, -6)) {
        const vec2 *value = lua_math_vec2_check(L, 3);
        result = graphics_shader_param_set(id, (float*)value, 2, errBuf);
    }
    else if(lua_equal(L, -1, -5)) {
        const vec3 *value = lua_math_vec3_check(L, 3);
        result = graphics_shader_param_set(id, (float*)value, 3, errBuf);
    }
    else if(lua_equal(L, -1, -4)) {
        const vec4 *value = lua_math_vec4_check(L, 3);
        result = graphics_shader_param_set(id, (float*)value, 4, errBuf);
    }
    else if(lua_equal(L, -1, -3)) {
        const lua_color *c = lua_color_check(L, 3);
        float value[] = {(float)(*c)[0] / 255.f, (float)(*c)[1] / 255.f, (float)(*c)[2] / 255.f, (float)(*c)[3] / 255.f};
        result = graphics_shader_param_set(id, value, 4, errBuf);
    }
    else if(lua_equal(L, -1, -2)) {
        const mat4x4 *value = lua_math_mat4x4_check(L, 3);
        result = graphics_shader_param_set(id, value, 4 * 4, errBuf);
    }
    else {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
    }

    if(!result) {
        fprintf(stderr, errBuf);
    }

    lua_pushboolean(L, result);
    return 1;
}

int lua_graphics_create_texture(lua_State *L)
{
    const lua_texture *texture = lua_texture_check(L, 1);

    char errBuf[1024];
    int handle = graphics_texture_create(texture->data, texture->width, texture->height, texture->num_channels, errBuf);
    if(!handle) {
        luaL_error(L, errBuf);
        return 0;
    }

    lua_pushinteger(L, handle);
    return 1;
}

int lua_graphics_update_texture(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    const lua_texture *texture = lua_texture_check(L, 2);

    char errBuf[1024];
    int result = graphics_texture_update(handle, texture->data, texture->width, texture->height, texture->num_channels, errBuf);
    if(!result) {
        luaL_error(L, errBuf);
        return 0;
    }

    return 0;
}

int lua_graphics_delete_texture(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    graphics_texture_destroy(handle);
    return 0;
}

int lua_graphics_use_texture(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    graphics_texture_use(handle);
    return 0;
}

int lua_graphics_create_vertex_array(lua_State *L)
{
    // TODO lua_asset_data loading

    luaL_checktype(L, 1, LUA_TTABLE);
    int vBufSize = lua_objlen(L, 1);
    float *vBuf = (float*)malloc(vBufSize * sizeof(float));
    lua_pushnil(L);
    while(lua_next(L, 1)) {
        float value = luaL_checknumber(L, -1);
        int idx = luaL_checkinteger(L, -2) - 1;
        if(idx >= 0 && idx < vBufSize)
            vBuf[idx] = value;
        lua_pop(L, 1);
    }

    luaL_checktype(L, 2, LUA_TTABLE);
    int eBufSize = lua_objlen(L, 2);
    int *eBuf = (int*)malloc(eBufSize * sizeof(int));
    lua_pushnil(L);
    while(lua_next(L, 2)) {
        int value = luaL_checknumber(L, -1);
        int idx = luaL_checkinteger(L, -2) - 1;
        if(idx >= 0 && idx < eBufSize)
            eBuf[idx] = value;
        lua_pop(L, 1);
    }

    char errBuf[1024];
    int handle = graphics_vertex_array_create(vBuf, vBufSize, eBuf, eBufSize, errBuf);
    if(!handle) {
        luaL_error(L, errBuf);
        return 0;
    }

    lua_pushinteger(L, handle);
    return 1;
}

int lua_graphics_update_vertex_array(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);

    // TODO lua_asset_data loading

    luaL_checktype(L, 2, LUA_TTABLE);
    int vBufSize = lua_objlen(L, 2);
    float *vBuf = (float*)malloc(vBufSize * sizeof(float));
    lua_pushnil(L);
    while(lua_next(L, 2)) {
        float value = luaL_checknumber(L, -1);
        int idx = luaL_checkinteger(L, -2) - 1;
        if(idx >= 0 && idx < vBufSize)
            vBuf[idx] = value;
        lua_pop(L, 1);
    }

    luaL_checktype(L, 3, LUA_TTABLE);
    int eBufSize = lua_objlen(L, 3);
    int *eBuf = (int*)malloc(eBufSize * sizeof(int));
    lua_pushnil(L);
    while(lua_next(L, 3)) {
        int value = luaL_checknumber(L, -1);
        int idx = luaL_checkinteger(L, -2) - 1;
        if(idx >= 0 && idx < eBufSize)
            eBuf[idx] = value;
        lua_pop(L, 1);
    }

    char errBuf[1024];
    int result = graphics_vertex_array_update(handle, vBuf, vBufSize, eBuf, eBufSize, errBuf);
    if(!result) {
        luaL_error(L, errBuf);
        return 0;
    }

    return 0;
}

int lua_graphics_delete_vertex_array(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    int result = graphics_vertex_array_destroy(handle);
    if(!result) {
        luaL_error(L, "Failed to delete vertex array (invalid handle, perhaps?)");
    }
    return 0;
}

int lua_graphics_use_vertex_array(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    int result = graphics_vertex_array_use(handle);
    if(!result) {
        luaL_error(L, "Failed to bind vertex array (invalid handle, perhaps?)");
    }
    return 0;
}

int lua_graphics_set_vertex_array_attribute(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    int id = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);

    int result = graphics_vertex_array_set_attribute(handle, id, size);
    if(!result) {
        luaL_error(L, "Failed to set VA attribute (invalid handle, perhaps?)");
    }
    return 0;
}

int lua_graphics_delete_vertex_array_attribute(lua_State *L)
{
    int handle = luaL_checkinteger(L, 1);
    int id = luaL_checkinteger(L, 2);

    int result = graphics_vertex_array_delete_attribute(handle, id);
    if(!result) {
        luaL_error(L, "Failed to set VA attribute (invalid handle, perhaps?)");
    }
    return 0;
}

int lua_graphics_draw(lua_State *L)
{
    graphics_draw();
    return 0;
}

int lua_graphics_clear(lua_State *L)
{
    lua_color *color = lua_color_check(L, 1);
    float c[] = {
        (float)((*color)[0]) / 255.f,
        (float)((*color)[1]) / 255.f,
        (float)((*color)[2]) / 255.f,
        (float)((*color)[3]) / 255.f,
    };
    graphics_clear(c);
    return 0;
}

static const luaL_reg lua_graphics_lib[] = {
    {"isFeatureEnabled", lua_graphics_is_feature_enabled},
    {"setFeatureEnabled", lua_graphics_set_feature_enabled},

    {"createShader", lua_graphics_create_shader},
    {"deleteShader", lua_graphics_delete_shader},

    {"createShaderProgram", lua_graphics_create_shader_program},
    {"deleteShaderProgram", lua_graphics_delete_shader_program},
    {"useShaderProgram", lua_graphics_use_shader_program},

    {"shaderParamNameToID", lua_graphics_shader_param_name_to_id},
    {"setShaderParam", lua_graphics_set_shader_param},

    {"createTexture", lua_graphics_create_texture},
    {"updateTexture", lua_graphics_update_texture},
    {"deleteTexture", lua_graphics_delete_texture},
    {"useTexture", lua_graphics_use_texture},

    {"createVertexArray", lua_graphics_create_vertex_array},
    {"updateVertexArray", lua_graphics_update_vertex_array},
    {"deleteVertexArray", lua_graphics_delete_vertex_array},
    {"useVertexArray", lua_graphics_use_vertex_array},

    {"setVertexArrayAttribute", lua_graphics_set_vertex_array_attribute},
    {"deleteVertexArrayAttribute", lua_graphics_delete_vertex_array_attribute},

    {"draw", lua_graphics_draw},

    {"clear", lua_graphics_clear},

    {0, 0}
};

void lua_graphics_load(lua_State *L)
{
    lua_newtable(L);

    luaL_openlib(L, 0, lua_graphics_lib, 0);

    // graphics_version
    lua_pushinteger(L, graphics_opengl_3_2);
    lua_setfield(L, -2, "OPENGL_3_2");

    // graphics_feature
    lua_pushinteger(L, graphics_feature_depth_test);
    lua_setfield(L, -2, "DEPTH_TEST");
    lua_pushinteger(L, graphics_feature_blend);
    lua_setfield(L, -2, "BLEND");

    // graphics_shader_type
    lua_pushinteger(L, graphics_vertex_shader);
    lua_setfield(L, -2, "VERTEX_SHADER");
    lua_pushinteger(L, graphics_fragment_shader);
    lua_setfield(L, -2, "FRAGMENT_SHADER");

    lua_setfield(L, -2, "graphics");
}

#include <lua_graphics.h>
#include <lua_util.h>
#include <lua_math.h>
#include <lua_color.h>

lua_graphics_context *lua_graphics_to(lua_State *L, int index)
{
    lua_graphics_context *ctx = (lua_graphics_context*)lua_touserdata(L, index);
    if(ctx == NULL) luaL_typerror(L, index, "graphicsContext");
    return ctx;
}

lua_graphics_context *lua_graphics_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    lua_graphics_context *ctx = (lua_graphics_context*)luaL_checkudata(L, index, "graphicsContext");
    if(ctx == NULL) luaL_typerror(L, index, "graphicsContext");
    return ctx;
}

lua_graphics_context *lua_graphics_new(lua_State *L)
{
    lua_graphics_context *lctx = (lua_graphics_context*)lua_newuserdata(L, sizeof(lua_graphics_context));
    memset(lctx, 0, sizeof(lua_graphics_context));
    luaL_getmetatable(L, "graphicsContext");
    lua_setmetatable(L, -2);
    return lctx;
}

int lua_graphics_create(lua_State *L)
{
    lua_graphics_context *ctx = lua_graphics_new(L);
    graphics_context_init(ctx);
    return 1;
}

int lua_graphics_create_shader(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    const char *code = luaL_checkstring(L, 2);
    int type = luaL_checkinteger(L, 3);

    graphics_shader *shader = graphics_shader_create(ctx, code, type);
    lua_graphics_shader_push(L, shader);
    return 1;
}

int lua_graphics_create_shader_program(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    const graphics_shader *vShader = lua_graphics_shader_check(L, 2);
    const graphics_shader *fShader = lua_graphics_shader_check(L, 3);

    graphics_shader_program *p = graphics_shader_program_create(ctx, vShader, fShader);
    lua_graphics_shader_program_push(L, p);
    return 1;
}

int lua_graphics_create_texture(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);

    if(lua_isuserdata(L, 2)) {
        // TODO lua_asset_data loading
        lua_pushnil(L);
        return 1;
    }
    else if(lua_istable(L, 2)) {
        char *data = (char*)malloc(sizeof(char) * width * height * 4);
        lua_pushnil(L);
        while(lua_next(L, 2)) {
            int value = luaL_checkinteger(L, -1);
            int idx = luaL_checkinteger(L, -2) - 1;
            if(idx >= 0 && idx < width * height * 4)
                data[idx] = (char)value;
            lua_pop(L, 1);
        }
        graphics_texture *t = graphics_texture_create(ctx, data, width, height);
        lua_graphics_texture_push(L, t);
        return 1;
    }
    else {
        luaL_typerror(L, 2, "assetData or table");
        return 0;
    }
}

int lua_graphics_create_vertex_array(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);

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

    graphics_vertex_array *va = graphics_vertex_array_create(ctx, vBuf, vBufSize, eBuf, eBufSize);
    free(vBuf);
    free(eBuf);
    lua_graphics_vertex_array_push(L, va);
    return 1;
}

int lua_graphics_create_object(lua_State *L)
{
    graphics_object *ctx = lua_graphics_check(L, 1);
    graphics_object *obj = graphics_object_create(ctx);
    lua_graphics_object_push(L, obj);
    return 1;
}

int lua_graphics_set_shader_param(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    const char *name = luaL_checkstring(L, 2);

    luaL_getmetatable(L, "vec2");
    luaL_getmetatable(L, "vec3");
    luaL_getmetatable(L, "vec4");
    luaL_getmetatable(L, "color");
    luaL_getmetatable(L, "mat4x4");

    if(!lua_isnumber(L, 3) && !lua_getmetatable(L, 3)) {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
        return 0;
    }

    if(lua_isnumber(L, 3)) {
        float value = lua_tonumber(L, 3);
        graphics_shader_param_set(ctx, name, &value, 1);
    }
    else if(lua_equal(L, -1, -6)) {
        const vec2 *value = lua_math_vec2_check(L, 3);
        graphics_shader_param_set(ctx, name, (float*)value, 2);
    }
    else if(lua_equal(L, -1, -5)) {
        const vec3 *value = lua_math_vec3_check(L, 3);
        graphics_shader_param_set(ctx, name, (float*)value, 3);
    }
    else if(lua_equal(L, -1, -4)) {
        const vec4 *value = lua_math_vec4_check(L, 3);
        graphics_shader_param_set(ctx, name, (float*)value, 4);
    }
    else if(lua_equal(L, -1, -3)) {
        const lua_color *c = lua_color_check(L, 3);
        float value[] = {(float)(*c)[0] / 255.f, (float)(*c)[1] / 255.f, (float)(*c)[2] / 255.f, (float)(*c)[3] / 255.f};
        graphics_shader_param_set(ctx, name, value, 4);
    }
    else if(lua_equal(L, -1, -2)) {
        const mat4x4 *value = lua_math_mat4x4_check(L, 3);
        graphics_shader_param_set(ctx, name, value, 4 * 4);
    }
    else {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
    }
    return 0;
}

int lua_graphics_delete_shader_param(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    const char *name = luaL_checkstring(L, 2);
    graphics_shader_param_delete(ctx, name);
    return 0;
}

int lua_graphics_refresh_draw_order(lua_State *L)
{
    graphics_context *ctx = lua_graphics_check(L, 1);
    graphics_refresh_draw_order(ctx);
    return 0;
}

int lua_graphics_gc(lua_State *L)
{
    lua_graphics_context *ctx = lua_graphics_check(L, 1);
    if(!ctx->is_valid) return 0;
    graphics_context_destroy(ctx);
    return 0;
}

static const luaL_reg lua_graphics_meta[] = {
    {"isValid", lua_util_udata_is_valid},
    {"createShader", lua_graphics_create_shader},
    {"createShaderProgram", lua_graphics_create_shader_program},
    {"createTexture", lua_graphics_create_texture},
    {"createVertexArray", lua_graphics_create_vertex_array},
    {"createObject", lua_graphics_create_object},
    {"setShaderParam", lua_graphics_set_shader_param},
    {"deleteShaderParam", lua_graphics_delete_shader_param},
    {"refreshDrawOrder", lua_graphics_refresh_draw_order},
    {"__gc", lua_graphics_gc},
    {0, 0}
};

void lua_graphics_ctx_load(lua_State *L)
{
    lua_pushcfunction(L, lua_graphics_create);
    lua_setglobal(L, "graphicsContext");
    luaL_newmetatable(L, "graphicsContext");
    luaL_openlib(L, 0, lua_graphics_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

graphics_shader *lua_graphics_shader_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    graphics_shader **shader = (graphics_shader**)luaL_checkudata(L, index, "graphicsShader");
    if(shader == NULL) luaL_typerror(L, index, "graphicsShader");
    return *shader;
}

void lua_graphics_shader_push(lua_State *L, const graphics_shader *shader)
{
    graphics_shader **lctx = (graphics_shader**)lua_newuserdata(L, sizeof(graphics_shader*));
    *lctx = shader;
    luaL_getmetatable(L, "graphicsShader");
    lua_setmetatable(L, -2);
}

int lua_graphics_shader_type(lua_State *L)
{
    const graphics_shader *shader = lua_graphics_shader_check(L, 1);
    if(!shader->is_valid) return 0;
    lua_pushinteger(L, shader->type);
    return 1;
}

int lua_graphics_shader_code(lua_State *L)
{
    const graphics_shader *shader = lua_graphics_shader_check(L, 1);
    if(!shader->is_valid) return 0;
    lua_pushstring(L, shader->code);
    return 1;
}

int lua_graphics_shader_destroy(lua_State *L)
{
    graphics_shader *shader = lua_graphics_shader_check(L, 1);
    graphics_shader_destroy(shader);
    return 0;
}

static const luaL_reg lua_graphics_shader_meta[] = {
    {"isValid", lua_util_udata_ptr_is_valid},
    {"type", lua_graphics_shader_type},
    {"code", lua_graphics_shader_code},
    {"destroy", lua_graphics_shader_destroy},
    {0, 0}
};

void lua_graphics_shader_load(lua_State *L)
{
    lua_pushinteger(L, graphics_vertex_shader);
    lua_setglobal(L, "VERTEX_SHADER");
    lua_pushinteger(L, graphics_fragment_shader);
    lua_setglobal(L, "FRAGMENT_SHADER");
    luaL_newmetatable(L, "graphicsShader");
    luaL_openlib(L, 0, lua_graphics_shader_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

graphics_shader_program *lua_graphics_shader_program_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    graphics_shader_program **program = (graphics_shader_program**)luaL_checkudata(L, index, "graphicsShaderProgram");
    if(program == NULL) luaL_typerror(L, index, "graphicsShaderProgram");
    return *program;
}

void lua_graphics_shader_program_push(lua_State *L, const graphics_shader_program *program)
{
    graphics_shader_program **lctx = (graphics_shader_program**)lua_newuserdata(L, sizeof(graphics_shader_program*));
    *lctx = program;
    luaL_getmetatable(L, "graphicsShaderProgram");
    lua_setmetatable(L, -2);
}

int lua_graphics_shader_program_get_shader(lua_State *L)
{
    const graphics_shader_program *p = lua_graphics_shader_program_check(L, 1);
    int type = luaL_checkinteger(L, 2);
    if(type == graphics_vertex_shader) {
        lua_graphics_shader_push(L, p->vertex_shader);
    }
    else if(type == graphics_fragment_shader) {
        lua_graphics_shader_push(L, p->fragment_shader);
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

int lua_graphics_shader_program_destroy(lua_State *L)
{
    graphics_shader_program *p = lua_graphics_shader_program_check(L, 1);
    graphics_shader_program_destroy(p);
    return 0;
}

static const luaL_reg lua_graphics_shader_program_meta[] = {
    {"isValid", lua_util_udata_ptr_is_valid},
    {"getShader", lua_graphics_shader_program_get_shader},
    {"destroy", lua_graphics_shader_program_destroy},
    {0, 0}
};

void lua_graphics_shader_program_load(lua_State *L)
{
    luaL_newmetatable(L, "graphicsShaderProgram");
    luaL_openlib(L, 0, lua_graphics_shader_program_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

graphics_texture *lua_graphics_texture_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    graphics_texture **t = (graphics_texture**)luaL_checkudata(L, index, "graphicsTexture");
    if(t == NULL) luaL_typerror(L, index, "graphicsTexture");
    return *t;
}

void lua_graphics_texture_push(lua_State *L, const graphics_texture *texture)
{
    graphics_texture **lt = (graphics_texture**)lua_newuserdata(L, sizeof(graphics_texture*));
    *lt = texture;
    luaL_getmetatable(L, "graphicsTexture");
    lua_setmetatable(L, -2);
}

int lua_graphics_texture_size(lua_State *L)
{
    const graphics_texture *t = lua_graphics_texture_check(L, 1);
    lua_pushinteger(L, t->width);
    lua_pushinteger(L, t->height);
    return 2;
}

static const luaL_reg lua_graphics_texture_meta[] = {
    {"size", lua_graphics_texture_size},
    {0, 0}
};

void lua_graphics_texture_load(lua_State *L)
{
    luaL_newmetatable(L, "graphicsTexture");
    luaL_openlib(L, 0, lua_graphics_texture_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

graphics_vertex_array *lua_graphics_vertex_array_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    graphics_vertex_array **va = (graphics_vertex_array**)luaL_checkudata(L, index, "graphicsVertexArray");
    if(va == NULL) luaL_typerror(L, index, "graphicsVertexArray");
    return *va;
}

void lua_graphics_vertex_array_push(lua_State *L, const graphics_vertex_array *varr)
{
    graphics_vertex_array **lva = (graphics_vertex_array**)lua_newuserdata(L, sizeof(graphics_vertex_array*));
    *lva = varr;
    luaL_getmetatable(L, "graphicsVertexArray");
    lua_setmetatable(L, -2);
}

int lua_graphics_vertex_array_set_attribute(lua_State *L)
{
    graphics_vertex_array *va = lua_graphics_vertex_array_check(L, 1);
    int id = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);

    graphics_vertex_array_set_attribute(va, id, size);
    return 0;
}

int lua_graphics_vertex_array_destroy(lua_State *L)
{
    graphics_vertex_array *va = lua_graphics_vertex_array_check(L, 1);
    graphics_vertex_array_destroy(va);
    return 0;
}

static const luaL_reg lua_graphics_vertex_array_meta[] = {
    {"isValid", lua_util_udata_ptr_is_valid},
    //{"update", lua_graphics_vertex_array_update},
    {"setAttribute", lua_graphics_vertex_array_set_attribute},
    {"destroy", lua_graphics_vertex_array_destroy},
    {0, 0}
};

void lua_graphics_vertex_array_load(lua_State *L)
{
    luaL_newmetatable(L, "graphicsVertexArray");
    luaL_openlib(L, 0, lua_graphics_vertex_array_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

graphics_object *lua_graphics_object_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    graphics_object **obj = (graphics_object**)luaL_checkudata(L, index, "graphicsObject");
    if(obj == NULL) luaL_typerror(L, index, "graphicsObject");
    return *obj;
}

void lua_graphics_object_push(lua_State *L, const graphics_object *object)
{
    graphics_object **lobj = (graphics_object**)lua_newuserdata(L, sizeof(graphics_object*));
    *lobj = object;
    luaL_getmetatable(L, "graphicsObject");
    lua_setmetatable(L, -2);
}

int lua_graphics_object_shader_program(lua_State *L)
{
    const graphics_object *obj = lua_graphics_object_check(L, 1);
    lua_graphics_shader_program_push(L, obj->shader_program);
    return 1;
}

int lua_graphics_object_set_shader_program(lua_State *L)
{
    graphics_object *obj = lua_graphics_object_check(L, 1);
    graphics_shader_program *program = lua_graphics_shader_program_check(L, 2);
    obj->shader_program = program;
    return 0;
}

int lua_graphics_object_texture(lua_State *L)
{
    const graphics_object *obj = lua_graphics_object_check(L, 1);
    lua_graphics_texture_push(L, obj->texture);
    return 1;
}

int lua_graphics_object_set_texture(lua_State *L)
{
    graphics_object *obj = lua_graphics_object_check(L, 1);
    graphics_texture *texture = lua_graphics_texture_check(L, 2);
    obj->texture = texture;
    return 0;
}

int lua_graphics_object_vertex_array(lua_State *L)
{
    const graphics_object *obj = lua_graphics_object_check(L, 1);
    lua_graphics_vertex_array_push(L, obj->vertex_array);
    return 1;
}

int lua_graphics_object_set_vertex_array(lua_State *L)
{
    graphics_object *obj = lua_graphics_object_check(L, 1);
    graphics_vertex_array *va = lua_graphics_vertex_array_check(L, 2);
    obj->vertex_array = va;
    return 0;
}

int lua_graphics_object_set_shader_param(lua_State *L)
{
    graphics_object *obj = lua_graphics_object_check(L, 1);
    const char *name = luaL_checkstring(L, 2);

    luaL_getmetatable(L, "vec2");
    luaL_getmetatable(L, "vec3");
    luaL_getmetatable(L, "vec4");
    luaL_getmetatable(L, "color");
    luaL_getmetatable(L, "mat4x4");

    if(!lua_isnumber(L, 3) && !lua_getmetatable(L, 3)) {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
        return 0;
    }

    if(lua_isnumber(L, 3)) {
        float value = lua_tonumber(L, 3);
        graphics_object_shader_param_set(obj, name, &value, 1);
    }
    else if(lua_equal(L, -1, -6)) {
        const vec2 *value = lua_math_vec2_check(L, 3);
        graphics_object_shader_param_set(obj, name, (float*)value, 2);
    }
    else if(lua_equal(L, -1, -5)) {
        const vec3 *value = lua_math_vec3_check(L, 3);
        graphics_object_shader_param_set(obj, name, (float*)value, 3);
    }
    else if(lua_equal(L, -1, -4)) {
        const vec4 *value = lua_math_vec4_check(L, 3);
        graphics_object_shader_param_set(obj, name, (float*)value, 4);
    }
    else if(lua_equal(L, -1, -3)) {
        const lua_color *c = lua_color_check(L, 3);
        float value[] = {(float)(*c)[0] / 255.f, (float)(*c)[1] / 255.f, (float)(*c)[2] / 255.f, (float)(*c)[3] / 255.f};
        graphics_object_shader_param_set(obj, name, value, 4);
    }
    else if(lua_equal(L, -1, -2)) {
        const mat4x4 *value = lua_math_mat4x4_check(L, 3);
        graphics_object_shader_param_set(obj, name, value, 4 * 4);
    }
    else {
        luaL_error(L, "Shader param value must be number, vec2, vec3, vec4, color or mat4x4");
    }
    return 0;
}

int lua_graphics_object_delete_shader_param(lua_State *L)
{
    graphics_object *obj = lua_graphics_object_check(L, 1);
    const char *name = luaL_checkstring(L, 2);
    graphics_object_shader_param_delete(obj, name);
    return 0;
}

static const luaL_reg lua_graphics_object_meta[] = {
    {"isValid", lua_util_udata_ptr_is_valid},
    {"shaderProgram", lua_graphics_object_shader_program},
    {"setShaderProgram", lua_graphics_object_set_shader_program},
    {"texture", lua_graphics_object_texture},
    {"setTexture", lua_graphics_object_set_texture},
    {"vertexArray", lua_graphics_object_vertex_array},
    {"setVertexArray", lua_graphics_object_set_vertex_array},
    {"setShaderParam", lua_graphics_object_set_shader_param},
    {"deleteShaderParam", lua_graphics_object_delete_shader_param},
    {0, 0}
};

void lua_graphics_object_load(lua_State *L)
{
    luaL_newmetatable(L, "graphicsObject");
    luaL_openlib(L, 0, lua_graphics_object_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

void lua_graphics_load(lua_State *L)
{
    lua_graphics_ctx_load(L);
    lua_graphics_shader_load(L);
    lua_graphics_shader_program_load(L);
    lua_graphics_texture_load(L);
    lua_graphics_vertex_array_load(L);
    lua_graphics_object_load(L);
}

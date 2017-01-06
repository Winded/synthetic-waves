#include <lua_texture.h>
#include <lua_asset.h>
#include <memory.h>
#include <lua_color.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int lua_texture_load(lua_State *L)
{
    luaL_checkstring(L, 1);
    lua_pushcfunction(L, lua_asset_load);
    lua_pushvalue(L, 1);
    lua_pushboolean(L, 0);
    lua_call(L, 2, 1);

    if(lua_isnil(L, -1)) {
        return 1;
    }

    const lua_asset *asset = lua_asset_check(L, -1);

    int width, height, num_channels;
    char *data = stbi_load_from_memory(asset->buffer, asset->buffer_size, &width, &height, &num_channels, 4);
    if(!data) {
        printf("Lua texture load failure: %s\n", stbi_failure_reason());
        lua_pushnil(L);
        return 1;
    }

    int asset_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_texture *texture = lua_texture_new(L);
    texture->asset_ref = asset_ref;
    texture->width = width;
    texture->height = height;
    texture->num_channels = num_channels;
    texture->data = data;
    texture->data_size = width * height * num_channels;

    return 1;
}

int lua_texture_create(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);

    char *data = (char*)malloc(sizeof(char) * width * height * 4);
    lua_pushnil(L);
    while(lua_next(L, 1)) {
        const lua_color *color = lua_color_check(L, -1);
        int idx = luaL_checkinteger(L, -2) - 1;
        if(idx >= 0 && idx < width * height)
            memcpy(data + idx * 4, color, sizeof(lua_color));
        lua_pop(L, 1);
    }

    lua_texture *texture = lua_texture_new(L);
    texture->width = width;
    texture->height = height;
    texture->num_channels = 4;
    texture->data = data;
    texture->data_size = width * height * 4;
    return 1;
}

static const luaL_reg lua_texture_lib[] = {
    {"load", lua_texture_load},
    {"create", lua_texture_create},
    {0, 0}
};

lua_texture *lua_texture_check(lua_State *L, int index)
{
    lua_texture *texture = (lua_texture*)luaL_checkudata(L, index, "texture");
    if(texture == NULL) luaL_typerror(L, index, "texture");
    return texture;
}

lua_texture *lua_texture_new(lua_State *L)
{
    lua_texture *texture = (lua_texture*)lua_newuserdata(L, sizeof(lua_texture));
    memset(texture, 0, sizeof(lua_texture));
    luaL_getmetatable(L, "texture");
    lua_setmetatable(L, -2);
    return texture;
}

int lua_texture_size(lua_State *L)
{
    const lua_texture *texture = lua_texture_check(L, 1);
    lua_pushinteger(L, texture->width);
    lua_pushinteger(L, texture->height);
    return 2;
}

int lua_texture_num_channels(lua_State *L)
{
    const lua_texture *texture = lua_texture_check(L, 1);
    lua_pushinteger(L, texture->num_channels);
    return 1;
}

int lua_texture_get_pixel(lua_State *L)
{
    const lua_texture *texture = lua_texture_check(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    int data_pos = (y * texture->width + x) * texture->num_channels;
    if(data_pos >= texture->data_size) {
        luaL_error(L, "Pixel coordinates out of range.");
        return 0;
    }

    const char *data = ((char*)texture->data) + data_pos;
    lua_color color;
    memcpy(color, data, sizeof(lua_color));
    lua_color_push(L, color);

    return 1;
}

int lua_texture_tostring(lua_State *L)
{
    lua_texture *texture = lua_texture_check(L, 1);
    lua_pushfstring(L, "texture(%dx%d)", texture->width, texture->height);
    return 1;
}

int lua_texture_gc(lua_State *L)
{
    lua_texture *texture = lua_texture_check(L, 1);
    if(texture->asset_ref) {
        luaL_unref(L, LUA_REGISTRYINDEX, texture->asset_ref);
    }
    else {
        free(texture->data);
    }
    return 0;
}

static const luaL_reg lua_texture_meta[] = {
    {"size", lua_texture_size},
    {"numChannels", lua_texture_num_channels},
    {"getPixel", lua_texture_get_pixel},
    {"__tostring", lua_texture_tostring},
    {"__gc", lua_texture_gc},
    {0, 0}
};

void lua_texture_load_lib(lua_State *L)
{
    luaL_newmetatable(L, "texture");
    luaL_openlib(L, 0, lua_texture_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_newtable(L);
    luaL_openlib(L, 0, lua_texture_lib, 0);
    lua_setfield(L, -2, "texture");
}

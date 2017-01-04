#include <lua_asset.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

#define MAX_ASSET_SOURCES 50

static struct {
        char is_configured;
        lua_asset_source asset_sources[MAX_ASSET_SOURCES];
} lua_asset_config = { 0 };

int lua_asset_configure(lua_State *L)
{
    if(lua_asset_config.is_configured) {
        luaL_error(L, "Lua asset library is already configured.\n");
        return 0;
    }

    luaL_checktype(L, 1, LUA_TTABLE);

    memset(&(lua_asset_config.asset_sources), 0, sizeof(lua_asset_source) * MAX_ASSET_SOURCES);

    lua_getfield(L, 1, "assetSources");
    luaL_checktype(L, -1, LUA_TTABLE);

    lua_pushnil(L);
    while(lua_next(L, -2)) {
        int idx = luaL_checkinteger(L, -2) - 1;
        luaL_checktype(L, -1, LUA_TTABLE);
        lua_getfield(L, -1, "type");
        lua_getfield(L, -2, "path");
        const char *type = luaL_checkstring(L, -2);
        const char *path = luaL_checkstring(L, -1);

        lua_asset_source *s = (lua_asset_source*)(&(lua_asset_config.asset_sources[idx]));

#if defined(_WIN32) || defined(_WIN64) || defined(linux)
        if(strcmp(type, "folder") == 0) {
            s->type = lua_asset_src_folder;
        }
        else if(strcmp(type, "zip") == 0) {
            s->type = lua_asset_src_zip;
        }
        else {
            luaL_error(L, "Invalid asset source type %s\n", type);
            return 0;
        }
#elif defined(__ANDROID_API__)
        s->type = lua_asset_src_android;
#endif

        char fullPath[1024] = "";
#if defined(_WIN32) || defined(_WIN64)
        _fullpath(fullPath, path, 1024);
#elif defined(linux)
        // TODO
#elif defined(__ANDROID_API__)
        // TODO
#endif
        if(strlen(fullPath) == 0) {
            luaL_error(L, "Unable to determine asset source path %s\n", path);
            return 0;
        }

        strcpy(s->source_path, fullPath);

        if(s->type == lua_asset_src_folder) {
#if defined(_WIN32) || defined(_WIN64)
            // TODO
#elif defined(linux)
            // TODO
#endif
        }
        else if(s->type == lua_asset_src_zip) {
            // TODO
        }
        else if(s->type == lua_asset_src_android) {
#if defined(__ANDROID_API__)
            // TODO
#endif
        }

        lua_pop(L, 3);
    }

    return 0;
}

int lua_asset_load(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    int stream = lua_toboolean(L, 2);

    // TODO

    lua_asset *asset = lua_asset_new(L);
    // asset->source = source;
    return 1;
}

static const luaL_reg lua_asset_lib[] = {
    {"configure", lua_asset_configure},
    {"load", lua_asset_load},
    {0, 0}
};

lua_asset *lua_asset_check(lua_State *L, int index)
{
    lua_asset *asset = (lua_asset*)luaL_checkudata(L, index, "asset");
    if(asset == NULL) luaL_typerror(L, index, "asset");
    return asset;
}

lua_asset *lua_asset_new(lua_State *L)
{
    lua_asset *asset = (lua_asset*)lua_newuserdata(L, sizeof(lua_asset));
    memset(asset, 0, sizeof(lua_asset));
    luaL_getmetatable(L, "asset");
    lua_setmetatable(L, -2);
    return asset;
}

static const luaL_reg lua_asset_meta[] = {
    {0, 0}
};

void lua_asset_lib_load(lua_State *L)
{
    lua_newtable(L);
    luaL_openlib(L, 0, lua_asset_lib, 0);
    lua_setglobal(L, "assets");
    luaL_newmetatable(L, "asset");
    luaL_openlib(L, 0, lua_asset_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

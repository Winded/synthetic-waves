#include <lua_asset.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

#define MAX_ASSET_SOURCES 50
#define MAX_LOADED_ASSETS 1000

static struct {
        char is_configured;
        lua_asset_source asset_sources[MAX_ASSET_SOURCES];
} lua_asset_config = { 0 };

void lua_asset_fill_index(lua_State *L, lua_asset_source *asset_source, const char *searchRoot, const char *rootPath, int *idx)
{
    if(asset_source->type == lua_asset_src_folder) {
#if defined(_WIN32) || defined(_WIN64)
        char searchPath[1024];
        sprintf(searchPath, "%s\\*.*", searchRoot);

        WIN32_FIND_DATA fData;
        HANDLE hFind;
        if((hFind = FindFirstFileA(searchPath, &fData)) == INVALID_HANDLE_VALUE) {
            luaL_error(L, "Path not found: %s\n", asset_source->source_path);
            return 0;
        }
        do {
            if(strcmp(fData.cFileName, ".") == 0 || strcmp(fData.cFileName, "..") == 0) continue;

            if((fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char rPath[256];
                sprintf(rPath, "%s%s/", rootPath, fData.cFileName);
                char sPath[256];
                sprintf(sPath, "%s\\%s", searchRoot, fData.cFileName);
                lua_asset_fill_index(L, asset_source, sPath, rPath, idx);
            }
            else {
                char fPath[256];
                strcpy(fPath, fData.cFileName);
                for(int i = 0; i < strlen(fPath); i++) {
                    if(fPath[i] == '\\')
                        fPath[i] = '/';
                }
                sprintf(&(asset_source->index[*idx]), "%s%s", rootPath, fPath);
                (*idx)++;
            }
        } while(FindNextFileA(hFind, &fData));
#elif defined(linux)
        // TODO
#endif
    }
    else if(asset_source->type == lua_asset_src_zip) {
        // TODO
    }
    else if(asset_source->type == lua_asset_src_android) {
#if defined(__ANDROID_API__)
        // TODO
#endif
    }
}

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

        int i = 0;
        lua_asset_fill_index(L, s, s->source_path, "/", &i);

        lua_pop(L, 3);
    }

    lua_asset_config.is_configured = 1;

    return 0;
}

void lua_asset_load_data(lua_asset *asset)
{
    if(asset->source->type == lua_asset_src_folder) {
        char fullPath[1024];
        sprintf(fullPath, "%s%s", asset->source->source_path, asset->path);

#if defined(_WIN32) || defined(_WIN64)
        for(int i = 0; i < strlen(fullPath); i++) {
            if(fullPath[i] == '/')
                fullPath[i] = '\\';
        }
#endif

        if(asset->load_type == lua_asset_loadtype_full) {
            FILE *f = fopen(fullPath, "rb");
            fseek(f, 0, SEEK_END);
            int size = ftell(f);
            fseek(f, 0, SEEK_SET);
            asset->buffer = malloc(size);
            asset->buffer_size = size;
            fread(asset->buffer, sizeof(char), size, f);
            fclose(f);
        }
        else if(asset->load_type == lua_asset_loadtype_stream) {
            // TODO
        }
    }
    else if(asset->source->type == lua_asset_src_zip) {
        // TODO
    }
    else if(asset->source->type == lua_asset_src_android) {
        // TODO
    }
}

int lua_asset_list_assets(lua_State *L)
{
    if(!lua_asset_config.is_configured) {
        luaL_error(L, "Asset library is not configured\n");
        return 0;
    }

    lua_newtable(L);
    int idx = 1;
    for(int i = 0; i < MAX_ASSET_SOURCES; i++) {
        lua_asset_source *s = &(lua_asset_config.asset_sources[i]);
        if(!s->type) continue;
        for(int i2 = 0; i2 < LUA_ASSET_INDEX_SIZE; i2++) {
            const char *p = &(s->index[i2]);
            if(strlen(p) > 0) {
                lua_pushinteger(L, idx);
                lua_pushstring(L, p);
                lua_rawset(L, -3);
                idx++;
            }
        }
    }

    return 1;
}

int lua_asset_load(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    int stream = 0;
    if(lua_isboolean(L, 2))
        stream = lua_toboolean(L, 2);

    // Find correct asset source. If not found, returns nil and does not load an asset
    lua_asset_source *source = 0;
    for(int i = 0; i < MAX_ASSET_SOURCES; i++) {
        lua_asset_source *s = &(lua_asset_config.asset_sources[i]);
        if(!s->type) continue;

        for(int i2 = 0; i2 < LUA_ASSET_INDEX_SIZE; i2++) {
            const char *p = s->index[i2];
            if(strlen(p) == 0) continue;
            if(strcmp(path, p) == 0) {
                source = s;
                break;
            }
        }
        if(source) break;
    }

    if(!source) {
        lua_pushnil(L);
        return 1;
    }

    lua_asset *asset = lua_asset_new(L);
    asset->source = source;
    asset->load_type = stream ? lua_asset_loadtype_stream : lua_asset_loadtype_full;
    strcpy(asset->path, path);
    lua_asset_load_data(asset);

    return 1;
}

static const luaL_reg lua_asset_lib[] = {
    {"configure", lua_asset_configure},
    {"listAssets", lua_asset_list_assets},
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

int lua_asset_path(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);
    lua_pushstring(L, asset->path);
    return 1;
}

int lua_asset_size(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);
    lua_pushinteger(L, asset->buffer_size);
    return 1;
}

int lua_asset_get_reader(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);
    lua_asset_reader *reader = lua_asset_reader_new(L);
    lua_pushvalue(L, 1);
    reader->asset_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}

int lua_asset_tostring(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);
    lua_pushfstring(L, "asset('%s')", asset->path);
    return 1;
}

int lua_asset_gc(lua_State *L)
{
    lua_asset *asset = lua_asset_check(L, 1);
    if(asset->load_type == lua_asset_loadtype_full) {
        free(asset->buffer);
    }
    else if(asset->load_type == lua_asset_loadtype_stream) {
        // TODO
    }
    return 0;
}

static const luaL_reg lua_asset_meta[] = {
    {"path", lua_asset_path},
    {"size", lua_asset_size},
    {"getReader", lua_asset_get_reader},
    {"__tostring", lua_asset_tostring},
    {"__gc", lua_asset_gc},
    {0, 0}
};

lua_asset_reader *lua_asset_reader_check(lua_State *L, int index)
{
    lua_asset_reader *reader = (lua_asset_reader*)luaL_checkudata(L, index, "assetReader");
    if(reader == NULL) luaL_typerror(L, index, "assetReader");
    return reader;
}

lua_asset_reader *lua_asset_reader_new(lua_State *L)
{
    lua_asset_reader *reader = (lua_asset_reader*)lua_newuserdata(L, sizeof(lua_asset_reader));
    memset(reader, 0, sizeof(lua_asset_reader));
    luaL_getmetatable(L, "assetReader");
    lua_setmetatable(L, -2);
    return reader;
}

int lua_asset_reader_read_all(lua_State *L)
{
    lua_asset_reader *reader = lua_asset_reader_check(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, reader->asset_ref);
    const lua_asset *asset = lua_asset_check(L, -1);

    lua_pushlstring(L, ((char*)asset->buffer) + reader->cursor, asset->buffer_size - reader->cursor);
    return 1;
}

int lua_asset_reader_tostring(lua_State *L)
{
    const lua_asset_reader *reader = lua_asset_reader_check(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, reader->asset_ref);
    const lua_asset *asset = lua_asset_check(L, -1);

    lua_pushfstring(L, "assetReader('%s')", asset->path);
    return 1;
}

int lua_asset_reader_gc(lua_State *L)
{
    lua_asset_reader *reader = lua_asset_reader_check(L, 1);
    luaL_unref(L, LUA_REGISTRYINDEX, reader->asset_ref);
    return 0;
}

static const luaL_reg lua_asset_reader_meta[] = {
    {"readAll", lua_asset_reader_read_all},
    {"__tostring", lua_asset_reader_tostring},
    {"__gc", lua_asset_reader_gc},
    {0, 0}
};

void lua_asset_lib_load(lua_State *L)
{
    luaL_newmetatable(L, "asset");
    luaL_openlib(L, 0, lua_asset_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__type");
    lua_pushstring(L, "asset");
    lua_rawset(L, -3);
    lua_pop(L, 1);

    luaL_newmetatable(L, "assetReader");
    luaL_openlib(L, 0, lua_asset_reader_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__type");
    lua_pushstring(L, "assetReader");
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_newtable(L);
    luaL_openlib(L, 0, lua_asset_lib, 0);
    lua_setfield(L, -2, "assets");
}

void lua_asset_lib_test(lua_State *L)
{
    int status = luaL_loadfile(L, "../tests/assets.lua");
    if(status) {
        fprintf(stderr, "Couldn't load file: %s", lua_tostring(L, -1));
        return 0;
    }

    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if(result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        return 0;
    }

    lua_asset_config.is_configured = 0;
}

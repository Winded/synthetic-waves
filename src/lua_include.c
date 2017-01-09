#include <lua_include.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
    #include <Shlwapi.h>
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

static int lua_include_filepath_ref;

int lua_include_do(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_include_filepath_ref);
    const char *curPath = lua_tostring(L, -1);

    char combinedPath[1024];
#if defined(_WIN32) || defined(_WIN64)
    PathCombineA(combinedPath, curPath, path);
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

    if(luaL_loadfile(L, combinedPath)) {
        luaL_error(L, "Could not include file %s: %s", path, lua_tostring(L, -1));
        return 0;
    }

    // Get file's directory
#if defined(_WIN32) || defined(_WIN64)
    for(int i = 0; i < strlen(combinedPath); i++)
        if(combinedPath[i] == '/')
            combinedPath[i] = '\\';
    PathRemoveFileSpecA(combinedPath);
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

    // Update file path and run script
    lua_pushstring(L, combinedPath);
    lua_rawseti(L, LUA_REGISTRYINDEX, lua_include_filepath_ref);
    lua_call(L, 0, 0);

    // Reset file path to our current layer
    lua_pushstring(L, curPath);
    lua_rawseti(L, LUA_REGISTRYINDEX, lua_include_filepath_ref);

    return 0;
}

void lua_include_load(lua_State *L)
{
    lua_pushcfunction(L, lua_include_do);
    lua_setglobal(L, "include");

    char path[1024];
#if defined(_WIN32) || defined(_WIN64)
    _fullpath(path, "", 1024);
#elif defined(linux)
    // TODO
#elif defined(__ANDROID_API__)
    // TODO
#endif

    lua_pushstring(L, path);
    lua_include_filepath_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

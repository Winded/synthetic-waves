/*
 * lua_asset.h
 *
 * Asset loading library. Assets are basically files, but they can be loaded from different sources.
 * On desktop, they can be loaded from a file on disk or a zip file. On mobile, they are loaded from a zip file contained in the asset database.
 * */
#ifndef lua_asset_h
#define lua_asset_h

#include <lua.h>
#include <lauxlib.h>

#define LUA_ASSET_INDEX_SIZE 1000

typedef enum {
    lua_asset_src_none = 0,
    lua_asset_src_folder,
    lua_asset_src_zip,
    lua_asset_src_android,
} lua_asset_source_type;

typedef enum {
    lua_asset_loadtype_full,
    lua_asset_loadtype_stream
} lua_asset_loadtype;

typedef struct lua_asset_source {
        lua_asset_source_type type;
        char source_path[1024];
        char index[LUA_ASSET_INDEX_SIZE][256];
} lua_asset_source;

typedef struct lua_asset {
        lua_asset_source *source;
        lua_asset_loadtype load_type;
        char path[256];
        void *file_descriptor;
        void *buffer;
        int buffer_size;
} lua_asset;

typedef struct lua_asset_reader {
        int asset_ref;
        int cursor;
} lua_asset_reader;

int lua_asset_load(lua_State *L);

lua_asset *lua_asset_check(lua_State *L, int index);
lua_asset *lua_asset_new(lua_State *L);

lua_asset_reader *lua_asset_reader_check(lua_State *L, int index);
lua_asset_reader *lua_asset_reader_new(lua_State *L);

void lua_asset_lib_load(lua_State *L);
void lua_asset_lib_test(lua_State *L);

#endif // lua_asset_h

#include <lua_color.h>
#include <memory.h>

lua_color *lua_color_to(lua_State *L, int index)
{
    lua_color *c = (lua_color*)lua_touserdata(L, index);
    if(c == NULL) luaL_typerror(L, index, "color");
    return c;
}

lua_color *lua_color_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    lua_color *c = (lua_color*)luaL_checkudata(L, index, "color");
    if(c == NULL) luaL_typerror(L, index, "color");
    return c;
}


void lua_color_push(lua_State *L, const lua_color *color)
{
    lua_color *lc = (lua_color*)lua_newuserdata(L, sizeof(lua_color));
    memcpy(lc, color, sizeof(lua_color));
    luaL_getmetatable(L, "color");
    lua_setmetatable(L, -2);
}

int lua_color_create(lua_State *L)
{
    lua_color c;
    memset(c, 0, sizeof(lua_color));

    if(lua_isnumber(L, 1)) {
        c[0] = (unsigned char)luaL_checkinteger(L, 1);
        if(lua_isnumber(L, 2)) {
            c[1] = (unsigned char)luaL_checkinteger(L, 2);
        }
        if(lua_isnumber(L, 3)) {
            c[2] = (unsigned char)luaL_checkinteger(L, 3);
        }
        if(lua_isnumber(L, 4)) {
            c[3] = (unsigned char)luaL_checkinteger(L, 4);
        }
    }
    else if(lua_isstring(L, 1)) {
        const char *sColor = luaL_checkstring(L, 1);
        if(strcmp(sColor, "white") == 0) {
            c[0] = 255;
            c[1] = 255;
            c[2] = 255;
            c[3] = 255;
        }
        else if(strcmp(sColor, "red") == 0) {
            c[0] = 255;
            c[1] = 0;
            c[2] = 0;
            c[3] = 255;
        }
        else if(strcmp(sColor, "green") == 0) {
            c[0] = 0;
            c[1] = 255;
            c[2] = 0;
            c[3] = 255;
        }
        else if(strcmp(sColor, "blue") == 0) {
            c[0] = 0;
            c[1] = 0;
            c[2] = 255;
            c[3] = 255;
        }
        lua_color_push(L, c);
        return 1;
    }

    lua_color_push(L, c);
    return 1;
}

int lua_color_index(lua_State *L)
{
    const lua_color *c = lua_color_check(L, 1);
    const char *idx = luaL_checkstring(L, 2);

    if(strcmp(idx, "r") == 0) {
        lua_pushinteger(L, ((int)(*c)[0]));
        return 1;
    }
    else if(strcmp(idx, "g") == 0) {
        lua_pushinteger(L, ((int)(*c)[1]));
        return 1;
    }
    else if(strcmp(idx, "b") == 0) {
        lua_pushinteger(L, ((int)(*c)[2]));
        return 1;
    }
    else if(strcmp(idx, "a") == 0) {
        lua_pushinteger(L, ((int)(*c)[3]));
        return 1;
    }

    luaL_getmetatable(L, "color");
    lua_getfield(L, -1, idx);
    return 1;
}

int lua_color_newindex(lua_State *L)
{
    lua_color *c = lua_color_check(L, 1);
    const char *idx = luaL_checkstring(L, 2);
    int value = luaL_checkinteger(L, 3);

    if(strcmp(idx, "r") == 0) {
        (*c)[0] = (unsigned char)value;
    }
    else if(strcmp(idx, "g") == 0) {
        (*c)[1] = (unsigned char)value;
    }
    else if(strcmp(idx, "b") == 0) {
        (*c)[2] = (unsigned char)value;
    }
    else if(strcmp(idx, "a") == 0) {
        (*c)[3] = (unsigned char)value;
    }
    else {
        luaL_error(L, "Cannot assign invalid index %s", idx);
    }

    return 0;
}

int lua_color_tostring(lua_State *L)
{
    const lua_color *c = lua_color_check(L, 1);
    lua_pushfstring(L, "color(%d, %d, %d, %d)", (*c)[0], (*c)[1], (*c)[2], (*c)[3]);
    return 1;
}

int lua_color_type(lua_State *L)
{
    const lua_color *c = lua_color_check(L, 1);
    lua_pushstring(L, "color");
    return 1;
}

static const luaL_reg lua_color_meta[] = {
    {"__index", lua_color_index},
    {"__newindex", lua_color_newindex},
    {"__tostring", lua_color_tostring},
    {"__type", lua_color_type},
    {0, 0}
};

void lua_color_load(lua_State *L)
{
    luaL_newmetatable(L, "color");
    luaL_openlib(L, 0, lua_color_meta, 0);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_pushcfunction(L, lua_color_create);
    lua_setfield(L, -2, "color");
}

void lua_color_test(lua_State *L)
{
    int status = luaL_loadfile(L, "../tests/color.lua");
    if(status) {
        fprintf(stderr, "Couldn't load file: %s", lua_tostring(L, -1));
        return 0;
    }

    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if(result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        return 0;
    }
}

#include <lua_font.h>
#include <lua_asset.h>
#include <lua_texture.h>
#include <memory.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

struct lua_font {
        int asset_ref;
        int texture_ref;
        stbtt_bakedchar *baked_chars;
};

lua_font *lua_font_check(lua_State *L, int index)
{
    lua_font *font = (lua_font*)luaL_checkudata(L, index, "font");
    if(font == NULL) luaL_typerror(L, index, "font");
    return font;
}

lua_font *lua_font_new(lua_State *L)
{
    lua_font *font = (lua_font*)lua_newuserdata(L, sizeof(lua_font));
    memset(font, 0, sizeof(lua_font));
    luaL_getmetatable(L, "font");
    lua_setmetatable(L, -2);
    return font;
}

int lua_font_load(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);
    int pixel_height = 32;
    if(lua_isnumber(L, 2))
        pixel_height = luaL_checkinteger(L, 2);

    int pw = 512;
    int ph = 512;
    void *pixels = malloc(pw * ph);
    stbtt_bakedchar *bakedchars = calloc(96, sizeof(stbtt_bakedchar));

    int result = stbtt_BakeFontBitmap(asset->buffer, 0, pixel_height, pixels, pw, ph, 32, 96, bakedchars);
    if(!result) {
        printf("Font loading failed\n");
        free(pixels);
        free(bakedchars);
        lua_pushnil(L);
        return 1;
    }

    lua_font *font = lua_font_new(L);
    font->baked_chars = bakedchars;
    lua_pushvalue(L, 1);
    font->asset_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_texture *texture = lua_texture_new(L);
    texture->width = pw;
    texture->height = ph;
    texture->num_channels = 1;
    texture->data = pixels;
    texture->data_size = pw * ph;
    font->texture_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    return 1;
}

static const luaL_reg lua_font_lib[] = {
    {"load", lua_font_load},
    {0, 0}
};

int lua_font_texture(lua_State *L)
{
    const lua_font *font = lua_font_check(L, 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, font->texture_ref);
    return 1;
}

int lua_font_get_char_info(lua_State *L)
{
    const lua_font *font = lua_font_check(L, 1);
    const char *c = luaL_checkstring(L, 2);
    if(strlen(c) != 1) {
        luaL_error(L, "Character string must be of length 1");
        return 0;
    }

    char ch = *c;
    stbtt_bakedchar *bakedchar = font->baked_chars + (ch - 32);

    lua_newtable(L);
    lua_pushinteger(L, bakedchar->x0);
    lua_setfield(L, -2, "x0");
    lua_pushinteger(L, bakedchar->y0);
    lua_setfield(L, -2, "y0");
    lua_pushinteger(L, bakedchar->x1);
    lua_setfield(L, -2, "x1");
    lua_pushinteger(L, bakedchar->y1);
    lua_setfield(L, -2, "y1");
    lua_pushnumber(L, bakedchar->xoff);
    lua_setfield(L, -2, "xoff");
    lua_pushnumber(L, bakedchar->yoff);
    lua_setfield(L, -2, "yoff");
    lua_pushnumber(L, bakedchar->xadvance);
    lua_setfield(L, -2, "xadvance");
    return 1;
}

int lua_font_make_vertex_array(lua_State *L)
{
    const lua_font *font = lua_font_check(L, 1);
    const char *text = luaL_checkstring(L, 2);

    lua_rawgeti(L, LUA_REGISTRYINDEX, font->texture_ref);
    const lua_texture *texture = lua_texture_check(L, -1);

    int xpos = 0;
    int ypos = 0;
    stbtt_aligned_quad quad;
#ifdef USE_OPENGL
    int fillrule = 1;
#else
    int fillrule = 0;
#endif
    int vidx = 1;
    int eidx = 1;
    int evidx = 0;

    lua_newtable(L); // Returned table
    lua_newtable(L); // vertex buffer
    lua_newtable(L); // element buffer

    for(int i = 0; i < strlen(text); i++) {
        char c = *(text + i);
        stbtt_GetBakedQuad(font->baked_chars, texture->width, texture->height, c - 32, &xpos, &ypos, &quad, 1);

        float vbuf[] = {
            quad.x0, quad.y0, 0, quad.s0, quad.t0,
            quad.x1, quad.y0, 0, quad.s1, quad.t0,
            quad.x1, quad.y1, 0, quad.s1, quad.t1,
            quad.x0, quad.y1, 0, quad.s0, quad.t1,
        };
        int ebuf[] = {
            evidx, evidx + 1, evidx + 3,
            evidx + 1, evidx + 2, evidx + 3,
        };

        for(int i2 = 0; i2 < 5 * 4; i2++) {
            lua_pushinteger(L, vidx);
            lua_pushnumber(L, vbuf[i2]);
            lua_rawset(L, -4);
            vidx++;
        }
        for(int i2 = 0; i2 < 6; i2++) {
            lua_pushinteger(L, eidx);
            lua_pushnumber(L, ebuf[i2]);
            lua_rawset(L, -3);
            eidx++;
        }
        evidx += 4;
    }

    lua_setfield(L, -3, "ebuf");
    lua_setfield(L, -2, "vbuf");
    return 1;
}

int lua_font_gc(lua_State *L)
{
    lua_font *font = lua_font_check(L, 1);
    luaL_unref(L, LUA_REGISTRYINDEX, font->asset_ref);
    luaL_unref(L, LUA_REGISTRYINDEX, font->texture_ref);
    free(font->baked_chars);
    return 0;
}

static const luaL_reg lua_font_meta[] = {
    {"texture", lua_font_texture},
    {"getCharInfo", lua_font_get_char_info},
    {"makeVertexArray", lua_font_make_vertex_array},
    {"__gc", lua_font_gc},
    {0, 0}
};

void lua_font_load_lib(lua_State *L)
{
    luaL_newmetatable(L, "font");
    luaL_openlib(L, 0, lua_font_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_newtable(L);
    luaL_openlib(L, 0, lua_font_lib, 0);
    lua_setfield(L, -2, "font");
}

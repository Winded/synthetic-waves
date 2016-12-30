#include <lua_window.h>
#include <lua_graphics.h>
#include <lua_color.h>
#include <GL/glew.h>

window *lua_window_to(lua_State *L, int index)
{
    window** w = (window**)lua_touserdata(L, index);
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

window *lua_window_check(lua_State *L, int index)
{
    window** w;
    luaL_checktype(L, index, LUA_TUSERDATA);
    w = (window**)luaL_checkudata(L, index, "window");
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

void lua_window_push(lua_State *L, const window *w)
{
    window** lw = (window**)lua_newuserdata(L, sizeof(window*));
    *lw = w;
    luaL_getmetatable(L, "window");
    lua_setmetatable(L, -2);
}

void lua_window_handle_event(window *w, SDL_Event *event, void *data)
{
    lua_State *L = (lua_State*)data;

    // TODO (current is example)
    if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
        int width, height;
        SDL_GetWindowSize(SDL_GetWindowFromID(event->window.windowID), &width, &height);
        // TODO
    }
}

int lua_window_create(lua_State *L)
{
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);

    window *w = window_create(title, width, height);
    window_add_event_callback(w, lua_window_handle_event, L);
    lua_window_push(L, w);
    return 1;
}

int lua_window_set_graphics_context(lua_State *L)
{
    window *w = lua_window_check(L, 1);
    const graphics_context *ctx = lua_graphics_check(L, 2);
    window_set_graphics_context(w, ctx);
    return 0;
}

int lua_window_delta_time(lua_State *L)
{
    window *w_handle = lua_window_check(L, 1);
    float deltaTime = window_get_delta_time(w_handle);
    lua_pushnumber(L, deltaTime);
    return 1;
}

int lua_window_get_clear_color(lua_State *L)
{
    const window *w = lua_window_check(L, 1);
    float fc[4];
    window_get_clear_color(w, fc);
    lua_color c;
    c[0] = (int)(fc[0] * 255.f);
    c[1] = (int)(fc[1] * 255.f);
    c[2] = (int)(fc[2] * 255.f);
    c[3] = (int)(fc[3] * 255.f);
    lua_color_push(L, c);
    return 1;
}

int lua_window_set_clear_color(lua_State *L)
{
    window *w = lua_window_check(L, 1);
    const lua_color *c = lua_color_check(L, 2);
    float fc[4];
    fc[0] = (*c)[0] / 255.f;
    fc[1] = (*c)[1] / 255.f;
    fc[2] = (*c)[2] / 255.f;
    fc[3] = (*c)[3] / 255.f;
    window_set_clear_color(w, fc);
    return 0;
}

int lua_window_should_close(lua_State *L)
{
    window *w_handle = lua_window_check(L, 1);
    int shouldClose = window_should_close(w_handle) ? 1 : 0;
    lua_pushboolean(L, shouldClose);
    return 1;
}

int lua_window_poll_events(lua_State *L)
{
    window *w_handle = lua_window_check(L, 1);
    window_poll_events(w_handle);
    return 0;
}

int lua_window_draw(lua_State *L)
{
    window *w = lua_window_check(L, 1);
    window_draw(w);
    return 0;
}

int lua_window_destroy(lua_State *L)
{
    window *w_handle = lua_window_check(L, 1);
    window_remove_event_callback(w_handle, lua_window_handle_event);
    window_destroy(&w_handle);
    lua_pushboolean(L, 1);
    return 1;
}

int lua_window_tostring(lua_State *L)
{
    const window *w = lua_window_check(L, 1);
    lua_pushstring(L, "window");
    return 1;
}

static const luaL_reg lua_window_meta[] = {
    {"setGraphicsContext", lua_window_set_graphics_context},
    {"deltaTime", lua_window_delta_time},
    {"clearColor", lua_window_get_clear_color},
    {"setClearColor", lua_window_set_clear_color},
    {"shouldClose", lua_window_should_close},
    {"pollEvents", lua_window_poll_events},
    {"draw", lua_window_draw},
    {"destroy", lua_window_destroy},
    {"__tostring", lua_window_tostring},
    {0, 0}
};

void lua_window_load(lua_State *L)
{
    lua_pushcfunction(L, lua_window_create);
    lua_setglobal(L, "window");
    luaL_newmetatable(L, "window");
    luaL_openlib(L, 0, lua_window_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

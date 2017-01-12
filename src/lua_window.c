#include <lua_window.h>
#include <lua_graphics.h>
#include <lua_color.h>
#include <lua_util.h>
#include <GL/glew.h>
#include <string.h>

lua_window *lua_window_to(lua_State *L, int index)
{
    lua_window* w = (lua_window*)lua_touserdata(L, index);
    if (w == NULL) luaL_typerror(L, index, "window");
    return w;
}

lua_window *lua_window_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    lua_window *w = (lua_window*)luaL_checkudata(L, index, "window");
    if (w == NULL) luaL_typerror(L, index, "window");
    return w;
}

lua_window *lua_window_new(lua_State *L)
{
    lua_window* lw = (lua_window*)lua_newuserdata(L, sizeof(lua_window));
    memset(lw, 0, sizeof(lua_window));
    luaL_getmetatable(L, "window");
    lua_setmetatable(L, -2);
    return lw;
}

void lua_window_handle_event(window *w, SDL_Event *event, void *data)
{
    lua_State *L = (lua_State*)data;
    lua_window *lw = lua_window_check(L, 1);

    // TODO
    for(int i = 0; i < LUA_WINDOW_MAX_EVENT_HANDLERS; i++) {
        if(!lw->event_handlers[i]) continue;
        lua_rawgeti(L, LUA_REGISTRYINDEX, lw->event_handlers[i]);
        if(!lua_istable(L, -1)) {
            lua_pop(L, 1);
            continue;
        }

        lua_getfield(L, -1, "name");
        const char *name = luaL_checkstring(L, -1);
        lua_getfield(L, -2, "handler");
        luaL_checktype(L, -1, LUA_TFUNCTION);
        lua_pushvalue(L, 1);

        if(strcmp(name, "onWindowResize") == 0 && event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
            int width, height;
            SDL_GetWindowSize(SDL_GetWindowFromID(event->window.windowID), &width, &height);
            lua_newtable(L);
            lua_pushinteger(L, width);
            lua_setfield(L, -2, "width");
            lua_pushinteger(L, height);
            lua_setfield(L, -2, "height");
        }
        else if(((strcmp(name, "onKeyDown") == 0 && event->type == SDL_KEYDOWN) || (strcmp(name, "onKeyUp") == 0 && event->type == SDL_KEYUP)) && event->key.repeat == 0) {
            const char *keyName = SDL_GetKeyName(event->key.keysym.sym);
            lua_newtable(L);
            lua_pushstring(L, keyName);
            lua_setfield(L, -2, "key");
        }
        else if((strcmp(name, "onMouseDown") == 0 && event->type == SDL_MOUSEBUTTONDOWN) || (strcmp(name, "onMouseUp") == 0 && event->type == SDL_MOUSEBUTTONUP)) {
            lua_newtable(L);
            lua_pushinteger(L, event->button.button);
            lua_setfield(L, -2, "button");
            lua_pushinteger(L, event->button.clicks);
            lua_setfield(L, -2, "clicks");
            lua_pushinteger(L, event->button.x);
            lua_setfield(L, -2, "x");
            lua_pushinteger(L, event->button.y);
            lua_setfield(L, -2, "y");
        }
        else if(strcmp(name, "onMouseMove") == 0 && event->type == SDL_MOUSEMOTION) {
            lua_newtable(L);
            lua_pushinteger(L, event->motion.x);
            lua_setfield(L, -2, "x");
            lua_pushinteger(L, event->motion.y);
            lua_setfield(L, -2, "y");
            lua_pushinteger(L, event->motion.xrel);
            lua_setfield(L, -2, "xrel");
            lua_pushinteger(L, event->motion.yrel);
            lua_setfield(L, -2, "yrel");
        }
        else {
            lua_pop(L, 4);
            continue;
        }

        lua_call(L, 2, 0);
        lua_pop(L, 2);
    }
}

int lua_window_create(lua_State *L)
{
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);

    lua_window *w = lua_window_new(L);
    window_init(&(w->w_handle), title, width, height);
    window_add_event_callback(&(w->w_handle), lua_window_handle_event, L);
    return 1;
}

static luaL_reg lua_window_lib[] = {
    {"create", lua_window_create},
    {0, 0}
};

int lua_window_open(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    window_open(&(w->w_handle));
    return 0;
}

int lua_window_close(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    window_close(&(w->w_handle));
    return 0;
}

int lua_window_graphics_context(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    if(w->graphics_context_ref) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, w->graphics_context_ref);
        return 1;
    }
    else {
        lua_pushnil(L);
        return 1;
    }
}

int lua_window_set_graphics_context(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    const graphics_context *ctx = lua_graphics_check(L, 2);
    window_set_graphics_context(&(w->w_handle), ctx);
    if(w->graphics_context_ref) {
        luaL_unref(L, LUA_REGISTRYINDEX, w->graphics_context_ref);
    }
    lua_pushvalue(L, 2);
    w->graphics_context_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

int lua_window_delta_time(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    float deltaTime = window_get_delta_time(&(w->w_handle));
    lua_pushnumber(L, deltaTime);
    return 1;
}

int lua_window_get_clear_color(lua_State *L)
{
    const lua_window *w = lua_window_check(L, 1);
    float fc[4];
    window_get_clear_color(&(w->graphics_context_ref), fc);
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
    lua_window *w = lua_window_check(L, 1);
    const lua_color *c = lua_color_check(L, 2);
    float fc[4];
    fc[0] = (*c)[0] / 255.f;
    fc[1] = (*c)[1] / 255.f;
    fc[2] = (*c)[2] / 255.f;
    fc[3] = (*c)[3] / 255.f;
    window_set_clear_color(&(w->w_handle), fc);
    return 0;
}

int lua_window_position(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    int x, y;
    window_get_position(&(w->w_handle), &x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
}

int lua_window_set_position(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    window_set_position(&(w->w_handle), x, y);
    return 0;
}

int lua_window_size(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    int width, height;
    window_get_size(&(w->w_handle), &width, &height);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    return 2;
}

int lua_window_set_size(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    window_set_size(&(w->w_handle), width, height);
    return 0;
}

int lua_window_title(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    lua_pushstring(L, window_get_title(&(w->w_handle)));
    return 1;
}

int lua_window_set_title(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    const char *title = luaL_checkstring(L, 2);
    window_set_title(&(w->w_handle), title);
    return 0;
}

int lua_window_maximize(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    SDL_MaximizeWindow(&(w->w_handle));
    return 0;
}

int lua_window_minimize(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    SDL_MinimizeWindow(&(w->w_handle));
    return 0;
}

int lua_window_restore(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    SDL_RestoreWindow(&(w->w_handle));
    return 0;
}

int lua_window_should_close(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    int shouldClose = window_should_close(&(w->w_handle)) ? 1 : 0;
    lua_pushboolean(L, shouldClose);
    return 1;
}

int lua_window_poll_events(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    window_poll_events(&(w->w_handle));
    return 0;
}

int lua_window_draw(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    window_draw(&(w->w_handle));
    return 0;
}

int lua_window_add_event_callback(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);

    int i = 0;
    while(i < LUA_WINDOW_MAX_EVENT_HANDLERS) {
        if(!w->event_handlers[i]) break;
        i++;
    }
    if(i >= LUA_WINDOW_MAX_EVENT_HANDLERS) {
        luaL_error(L, "Out of event handler slots");
        return 0;
    }

    lua_newtable(L);
    lua_pushvalue(L, 2);
    lua_setfield(L, -2, "name");
    lua_pushvalue(L, 3);
    lua_setfield(L, -2, "handler");

    w->event_handlers[i] = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

int lua_window_tostring(lua_State *L)
{
    const lua_window *w = lua_window_check(L, 1);
    lua_pushstring(L, "window");
    return 1;
}

int lua_window_gc(lua_State *L)
{
    lua_window *w = lua_window_check(L, 1);
    if(w->graphics_context_ref)
        luaL_unref(L, LUA_REGISTRYINDEX, w->graphics_context_ref);
    window_destroy(&(w->w_handle));
    return 0;
}

static const luaL_reg lua_window_meta[] = {
    {"isValid", lua_util_udata_is_valid},
    {"open", lua_window_open},
    {"close", lua_window_close},
    {"graphicsContext", lua_window_graphics_context},
    {"setGraphicsContext", lua_window_set_graphics_context},
    {"deltaTime", lua_window_delta_time},
    {"clearColor", lua_window_get_clear_color},
    {"setClearColor", lua_window_set_clear_color},
    {"position", lua_window_position},
    {"setPosition", lua_window_set_position},
    {"size", lua_window_size},
    {"setSize", lua_window_set_size},
    {"title", lua_window_title},
    {"setTitle", lua_window_set_title},
    {"maximize", lua_window_maximize},
    {"minimize", lua_window_minimize},
    {"restore", lua_window_restore},
    {"shouldClose", lua_window_should_close},
    {"pollEvents", lua_window_poll_events},
    {"draw", lua_window_draw},
    {"addEventCallback", lua_window_add_event_callback},
    {"__tostring", lua_window_tostring},
    {"__gc", lua_window_gc},
    {0, 0}
};

void lua_window_load(lua_State *L)
{
    luaL_newmetatable(L, "window");
    luaL_openlib(L, 0, lua_window_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_newtable(L);
    luaL_openlib(L, 0, lua_window_lib, 0);
    lua_setfield(L, -2, "window");
}

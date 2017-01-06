#include <lua_input.h>
#include <SDL.h>

int lua_input_mouse_position(lua_State *L)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
}

int lua_input_is_mouse_button_down(lua_State *L)
{
    int button = luaL_checkinteger(L, 1);
    int state = SDL_GetMouseState(NULL, NULL);
    lua_pushboolean(L, state & SDL_BUTTON(button));
    return 1;
}

int lua_input_get_relative_mouse(lua_State *L)
{
    lua_pushboolean(L, SDL_GetRelativeMouseMode());
    return 1;
}

int lua_input_set_relative_mouse(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int state = lua_toboolean(L, 1);
    SDL_SetRelativeMouseMode(state);
    return 0;
}

int lua_input_is_key_down(lua_State *L)
{
    const char *key = luaL_checkstring(L, 1);
    SDL_Scancode scancode = SDL_GetScancodeFromName(key);
    if(!scancode) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    lua_pushboolean(L, state[scancode]);
    return 1;
}

static const luaL_reg lua_input_lib[] = {
    {"mousePosition", lua_input_mouse_position},
    {"isMouseButtonDown", lua_input_is_mouse_button_down},
    {"getRelativeMouse", lua_input_get_relative_mouse},
    {"setRelativeMouse", lua_input_set_relative_mouse},
    {"isKeyDown", lua_input_is_key_down},
    {0, 0}
};

void lua_input_load(lua_State *L)
{
    lua_newtable(L);
    luaL_openlib(L, 0, lua_input_lib, 0);
    lua_setfield(L, -2, "input");
}

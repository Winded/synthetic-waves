#include <lua_window.h>

GLFWwindow *lua_window_towindow(lua_State *L, int index)
{
    GLFWwindow** w = (GLFWwindow**)lua_touserdata(L, index);
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

GLFWwindow *lua_window_checkwindow(lua_State *L, int index)
{
    GLFWwindow** w;
    luaL_checktype(L, index, LUA_TUSERDATA);
    w = (GLFWwindow**)luaL_checkudata(L, index, "window");
    if (w == NULL) luaL_typerror(L, index, "window");
    return *w;
}

int lua_window_create(lua_State *L)
{
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        printf("FAIL window");
        return 0;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    GLFWwindow** w = (GLFWwindow**)lua_newuserdata(L, sizeof(GLFWwindow*));
    luaL_getmetatable(L, "window");
    lua_setmetatable(L, -2);

    *w = window;

    return 1;
}

int lua_window_shouldClose(lua_State *L)
{
    GLFWwindow *window = lua_window_checkwindow(L, 1);
    int shouldClose = glfwWindowShouldClose(window);
    lua_pushboolean(L, shouldClose);
    return 1;
}

int lua_window_clear(lua_State *L)
{
    GLFWwindow *window = lua_window_checkwindow(L, 1);
    float r = luaL_checknumber(L, 2);
    float g = luaL_checknumber(L, 3);
    float b = luaL_checknumber(L, 4);
    float a = luaL_checknumber(L, 5);
    (void)(window);

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

int lua_window_swapBuffers(lua_State *L)
{
    GLFWwindow *window = lua_window_checkwindow(L, 1);
    glfwSwapBuffers(window);
    return 0;
}

int lua_window_pollEvents(lua_State *L)
{
    GLFWwindow *window = lua_window_checkwindow(L, 1);
    (void)(window);
    glfwPollEvents();
    return 0;
}

int lua_window_destroy(lua_State *L)
{
    GLFWwindow *window = lua_window_checkwindow(L, 1);
    glfwDestroyWindow(window);
    lua_pushboolean(L, 1);
    return 1;
}

int lua_window_tostring(lua_State *L)
{
    char buff[32];
    sprintf(buff, "%p", lua_window_towindow(L, 1));
    lua_pushfstring(L, "Foo (%s)", buff);
    return 1;
}

void lua_window_load(lua_State *L)
{
    luaL_openlib(L, "window", lua_window_methods, 0);  /* create methods table, add it to the globals */
    luaL_newmetatable(L, "window");          /* create metatable for Foo, and add it to the Lua registry */
    luaL_openlib(L, 0, lua_window_meta, 0);    /* fill metatable */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -3);               /* dup methods table*/
    lua_rawset(L, -3);                  /* metatable.__index = methods */
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);               /* dup methods table*/
    lua_rawset(L, -3);                  /* hide metatable:  metatable.__metatable = methods */
    lua_pop(L, 1);                      /* drop metatable */
}

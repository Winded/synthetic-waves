#include <lua_math.h>
#include <memory.h>

const vec3 *lua_math_vec3_to(lua_State *L, int index)
{
    vec3 *v = (vec3*)lua_touserdata(L, index);
    if (v == NULL) luaL_typerror(L, index, "vec3");
    return v;
}

const vec3 *lua_math_vec3_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    vec3 *v = (vec3*)luaL_checkudata(L, index, "vec3");
    if (v == NULL) luaL_typerror(L, index, "vec3");
    return v;
}

void lua_math_vec3_push(lua_State *L, const vec3 *v)
{
    vec3* lv = (vec3*)lua_newuserdata(L, sizeof(vec3));
    memcpy(lv, v, sizeof(vec3));
    luaL_getmetatable(L, "vec3");
    lua_setmetatable(L, -2);
}

int lua_math_vec3_create(lua_State *L)
{
    vec3 v;
    for(int i = 0; i < 3; i++) {
        if(lua_isnumber(L, i + 1)) {
            v[i] = (float)lua_tonumber(L, i + 1);
        }
        else {
            v[i] = 0;
        }
    }

    lua_math_vec3_push(L, v);

    return 1;
}

int lua_math_vec3_length(lua_State *L)
{
    const vec3 *v = lua_math_vec3_check(L, 1);
    lua_pushnumber(L, vec3_len(v));
    return 1;
}

int lua_math_vec3_dot(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *v2 = lua_math_vec3_check(L, 2);
    lua_pushnumber(L, vec3_mul_inner(v1, v2));
    return 1;
}

int lua_math_vec3_equals(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *v2 = lua_math_vec3_check(L, 2);
    int is_equal = 1;
    for(int i = 0; i < 3; i++) {
        if(v1[i] != v2[i]) {
            is_equal = 0;
            break;
        }
    }
    lua_pushboolean(L, is_equal);
    return 1;
}

int lua_math_vec3_unm(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    vec3 v;
    vec3_scale(v, v1, -1.f);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_add(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *v2 = lua_math_vec3_check(L, 2);
    vec3 v;
    vec3_add(v, v1, v2);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_sub(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *v2 = lua_math_vec3_check(L, 2);
    vec3 v;
    vec3_sub(v, v1, v2);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_mul(lua_State *L)
{
    int vecIdx, numIdx;
    if(lua_isuserdata(L, 1) && lua_isnumber(L, 2)) {
        vecIdx = 1;
        numIdx = 2;
    }
    else if(lua_isnumber(L, 1) && lua_isuserdata(L, 2)) {
        vecIdx = 2;
        numIdx = 1;
    }
    else {
        lua_pushstring(L, "vec3 can only be multiplied with a number. "
                          "For dot or cross products, use dotProduct and crossProduct.");
        lua_error(L);
        return 0;
    }

    const vec3 *v1 = lua_math_vec3_check(L, vecIdx);
    float num = luaL_checknumber(L, numIdx);

    vec3 v;
    vec3_scale(v, v1, num);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_div(lua_State *L)
{
    int vecIdx, numIdx;
    if(lua_isuserdata(L, 1) && lua_isnumber(L, 2)) {
        vecIdx = 1;
        numIdx = 2;
    }
    else if(lua_isnumber(L, 1) && lua_isuserdata(L, 2)) {
        vecIdx = 2;
        numIdx = 1;
    }
    else {
        lua_pushstring(L, "vec3 can only be divided with a number");
        lua_error(L);
        return 0;
    }

    const vec3 *v1 = lua_math_vec3_check(L, vecIdx);
    float num = luaL_checknumber(L, numIdx);

    vec3 v;
    vec3_scale(v, v1, 1.f / num);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_index(lua_State *L)
{
    const vec3 *v = lua_math_vec3_check(L, 1);
    const char *idx = luaL_checkstring(L, 2);
    if(3 > 0 && strcmp(idx, "x") == 0) {
        float value = (*v)[0];
        lua_pushnumber(L, value);
        return 1;
    }
    else if(3 > 1 && strcmp(idx, "y") == 0) {
        float value = (*v)[1];
        lua_pushnumber(L, value);
        return 1;
    }
    else if(3 > 2 && strcmp(idx, "z") == 0) {
        float value = (*v)[2];
        lua_pushnumber(L, value);
        return 1;
    }
    else if(3 > 3 && strcmp(idx, "w") == 0) {
        float value = (*v)[3];
        lua_pushnumber(L, value);
        return 1;
    }

    if(lua_getmetatable(L, 1) && lua_isstring(L, 2)) {
        const char *idx = lua_tostring(L, 2);
        lua_pushstring(L, idx);
        lua_gettable(L, -2);
        return 1;
    }

    lua_pushstring(L, "Cannot access invalid vec3 property");
    lua_error(L);
    return 0;
}

int lua_math_vec3_newindex(lua_State *L)
{
    vec3 *v = (vec3*)lua_math_vec3_check(L, 1);
    const char *idx = luaL_checkstring(L, 2);
    float value = luaL_checknumber(L, 3);
    if(3 > 0 && strcmp(idx, "x") == 0) {
        (*v)[0] = value;
        return 0;
    }
    else if(3 > 1 && strcmp(idx, "y") == 0) {
        (*v)[1] = value;
        return 0;
    }
    else if(3 > 2 && strcmp(idx, "z") == 0) {
        (*v)[2] = value;
        return 0;
    }
    else if(3 > 3 && strcmp(idx, "w") == 0) {
        (*v)[3] = value;
        return 0;
    }

    lua_pushstring(L, "Cannot assign invalid vec3 property");
    lua_error(L);
    return 0;
}

int lua_math_vec3_tostring(lua_State *L)
{
    const vec3 *v = lua_math_vec3_check(L, 1);
    switch(3)
    {
    case 2:
        lua_pushfstring(L, "[%f, %f]", (*v)[0], (*v)[1]);
        break;
    case 3:
        lua_pushfstring(L, "[%f, %f, %f]", (*v)[0], (*v)[1], (*v)[2]);
        break;
    case 4:
        lua_pushfstring(L, "[%f, %f, %f, %f]", (*v)[0], (*v)[1], (*v)[2], (*v)[3]);
        break;
    }
    return 1;
}

static const luaL_reg lua_math_vec3_meta[] = {
    {"length", lua_math_vec3_length},
    {"dotProduct", lua_math_vec3_dot},
    {"__eq", lua_math_vec3_equals},
    {"__unm", lua_math_vec3_unm},
    {"__add", lua_math_vec3_add},
    {"__sub", lua_math_vec3_sub},
    {"__mul", lua_math_vec3_mul},
    {"__div", lua_math_vec3_div},
    {"__index", lua_math_vec3_index},
    {"__newindex", lua_math_vec3_newindex},
    {"__tostring", lua_math_vec3_tostring},
    {0, 0}
};

void lua_math_vec3_load(lua_State *L)
{
    lua_pushcfunction(L, lua_math_vec3_create);
    lua_setglobal(L, "vec3");
    luaL_newmetatable(L, "vec3");
    luaL_openlib(L, 0, lua_math_vec3_meta, 0);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

void lua_math_load(lua_State *L)
{
    lua_math_vec3_load(L);
}

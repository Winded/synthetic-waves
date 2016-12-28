#include <lua_math.h>
#include <memory.h>

#define LUA_MATH_DEFINE_VEC(n) \
\
vec##n *lua_math_vec##n##_to(lua_State *L, int index)\
{\
    vec##n *v = (vec##n*)lua_touserdata(L, index);\
    if (v == NULL) luaL_typerror(L, index, "vec"#n);\
    return v;\
}\
\
vec##n *lua_math_vec##n##_check(lua_State *L, int index)\
{\
    luaL_checktype(L, index, LUA_TUSERDATA);\
    vec##n *v = (vec##n*)luaL_checkudata(L, index, "vec"#n);\
    if (v == NULL) luaL_typerror(L, index, "vec"#n);\
    return v;\
}\
\
void lua_math_vec##n##_push(lua_State *L, const vec##n *v)\
{\
    vec##n* lv = (vec##n*)lua_newuserdata(L, sizeof(vec##n));\
    memcpy(lv, v, sizeof(vec##n));\
    luaL_getmetatable(L, "vec"#n);\
    lua_setmetatable(L, -2);\
}\
\
int lua_math_vec##n##_create(lua_State *L)\
{\
    vec##n v;\
    for(int i = 0; i < n; i++) {\
        if(lua_isnumber(L, i + 1)) {\
            v[i] = (float)lua_tonumber(L, i + 1);\
        }\
        else {\
            v[i] = 0;\
        }\
    }\
\
    lua_math_vec##n##_push(L, v);\
\
    return 1;\
}\
\
int lua_math_vec##n##_length(lua_State *L)\
{\
    const vec##n *v = lua_math_vec##n##_check(L, 1);\
    lua_pushnumber(L, vec##n##_len(v));\
    return 1;\
}\
\
int lua_math_vec##n##_dot(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    const vec##n *v2 = lua_math_vec##n##_check(L, 2);\
    lua_pushnumber(L, vec##n##_mul_inner(v1, v2));\
    return 1;\
}\
\
int lua_math_vec##n##_normalize(lua_State *L)\
{\
    vec##n *v = lua_math_vec##n##_check(L, 1);\
    vec##n##_norm(v, v);\
    return 0;\
}\
\
int lua_math_vec##n##_normalized(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    vec##n v;\
    vec##n##_norm(v, v1);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_equals(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    const vec##n *v2 = lua_math_vec##n##_check(L, 2);\
    int is_equal = 1;\
    for(int i = 0; i < 3; i++) {\
        if(v1[i] != v2[i]) {\
            is_equal = 0;\
            break;\
        }\
    }\
    lua_pushboolean(L, is_equal);\
    return 1;\
}\
\
int lua_math_vec##n##_unm(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    vec##n v;\
    vec##n##_scale(v, v1, -1.f);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_add(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    const vec##n *v2 = lua_math_vec##n##_check(L, 2);\
    vec##n v;\
    vec##n##_add(v, v1, v2);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_sub(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    const vec##n *v2 = lua_math_vec##n##_check(L, 2);\
    vec##n v;\
    vec##n##_sub(v, v1, v2);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_mul(lua_State *L)\
{\
    int vecIdx, numIdx;\
    if(lua_isuserdata(L, 1) && lua_isnumber(L, 2)) {\
        vecIdx = 1;\
        numIdx = 2;\
    }\
    else if(lua_isnumber(L, 1) && lua_isuserdata(L, 2)) {\
        vecIdx = 2;\
        numIdx = 1;\
    }\
    else {\
        lua_pushstring(L, "vec##n can only be multiplied with a number. "\
                          "For dot or cross products, use dotProduct and crossProduct.");\
        lua_error(L);\
        return 0;\
    }\
\
    const vec##n *v1 = lua_math_vec##n##_check(L, vecIdx);\
    float num = luaL_checknumber(L, numIdx);\
\
    vec##n v;\
    vec##n##_scale(v, v1, num);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_div(lua_State *L)\
{\
    int vecIdx, numIdx;\
    if(lua_isuserdata(L, 1) && lua_isnumber(L, 2)) {\
        vecIdx = 1;\
        numIdx = 2;\
    }\
    else if(lua_isnumber(L, 1) && lua_isuserdata(L, 2)) {\
        vecIdx = 2;\
        numIdx = 1;\
    }\
    else {\
        lua_pushstring(L, "vec##n can only be divided with a number");\
        lua_error(L);\
        return 0;\
    }\
\
    const vec##n *v1 = lua_math_vec##n##_check(L, vecIdx);\
    float num = luaL_checknumber(L, numIdx);\
\
    vec##n v;\
    vec##n##_scale(v, v1, 1.f / num);\
    lua_math_vec##n##_push(L, v);\
    return 1;\
}\
\
int lua_math_vec##n##_index(lua_State *L)\
{\
    const vec##n *v = lua_math_vec##n##_check(L, 1);\
    const char *idx = luaL_checkstring(L, 2);\
    if(n > 0 && strcmp(idx, "x") == 0) {\
        float value = (*v)[0];\
        lua_pushnumber(L, value);\
        return 1;\
    }\
    else if(n > 1 && strcmp(idx, "y") == 0) {\
        float value = (*v)[1];\
        lua_pushnumber(L, value);\
        return 1;\
    }\
    else if(n > 2 && strcmp(idx, "z") == 0) {\
        float value = (*v)[2];\
        lua_pushnumber(L, value);\
        return 1;\
    }\
    else if(n > 3 && strcmp(idx, "w") == 0) {\
        float value = (*v)[3];\
        lua_pushnumber(L, value);\
        return 1;\
    }\
\
    if(lua_getmetatable(L, 1) && lua_isstring(L, 2)) {\
        const char *idx = lua_tostring(L, 2);\
        lua_pushstring(L, idx);\
        lua_gettable(L, -2);\
        return 1;\
    }\
\
    lua_pushstring(L, "Cannot access invalid vec##n property");\
    lua_error(L);\
    return 0;\
}\
\
int lua_math_vec##n##_newindex(lua_State *L)\
{\
    vec##n *v = lua_math_vec##n##_check(L, 1);\
    const char *idx = luaL_checkstring(L, 2);\
    float value = luaL_checknumber(L, 3);\
    if(n > 0 && strcmp(idx, "x") == 0) {\
        (*v)[0] = value;\
        return 0;\
    }\
    else if(n > 1 && strcmp(idx, "y") == 0) {\
        (*v)[1] = value;\
        return 0;\
    }\
    else if(n > 2 && strcmp(idx, "z") == 0) {\
        (*v)[2] = value;\
        return 0;\
    }\
    else if(n > 3 && strcmp(idx, "w") == 0) {\
        (*v)[3] = value;\
        return 0;\
    }\
\
    lua_pushstring(L, "Cannot assign invalid vec##n property");\
    lua_error(L);\
    return 0;\
}\
\
int lua_math_vec##n##_tostring(lua_State *L)\
{\
    const vec##n *v = lua_math_vec##n##_check(L, 1);\
    switch(n)\
    {\
    case 2:\
        lua_pushfstring(L, "[%f, %f]", (*v)[0], (*v)[1]);\
        break;\
    case 3:\
        lua_pushfstring(L, "[%f, %f, %f]", (*v)[0], (*v)[1], (*v)[2]);\
        break;\
    case 4:\
        lua_pushfstring(L, "[%f, %f, %f, %f]", (*v)[0], (*v)[1], (*v)[2], (*v)[3]);\
        break;\
    }\
    return 1;\
}\
\
int lua_math_vec##n##_type(lua_State *L)\
{\
    const vec##n *v1 = lua_math_vec##n##_check(L, 1);\
    lua_pushstring(L, "vec"#n);\
    return 1;\
}\
\
static const luaL_reg lua_math_vec##n##_meta[] = {\
    {"length", lua_math_vec##n##_length},\
    {"dotProduct", lua_math_vec##n##_dot},\
    {"normalize", lua_math_vec##n##_normalize},\
    {"normalized", lua_math_vec##n##_normalized},\
    {"__eq", lua_math_vec##n##_equals},\
    {"__unm", lua_math_vec##n##_unm},\
    {"__add", lua_math_vec##n##_add},\
    {"__sub", lua_math_vec##n##_sub},\
    {"__mul", lua_math_vec##n##_mul},\
    {"__div", lua_math_vec##n##_div},\
    {"__index", lua_math_vec##n##_index},\
    {"__newindex", lua_math_vec##n##_newindex},\
    {"__tostring", lua_math_vec##n##_tostring},\
    {"__type", lua_math_vec##n##_type},\
    {0, 0}\
};\
\
void lua_math_vec##n##_load(lua_State *L)\
{\
    lua_pushcfunction(L, lua_math_vec##n##_create);\
    lua_setglobal(L, "vec"#n);\
    luaL_newmetatable(L, "vec"#n);\
    luaL_openlib(L, 0, lua_math_vec##n##_meta, 0);\
    lua_pushliteral(L, "__metatable");\
    lua_pushvalue(L, -3);\
    lua_rawset(L, -3);\
    lua_pop(L, 1);\
}

LUA_MATH_DEFINE_VEC(2)
LUA_MATH_DEFINE_VEC(3)
LUA_MATH_DEFINE_VEC(4)

int lua_math_vec3_cross(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *v2 = lua_math_vec3_check(L, 2);
    vec3 v;
    vec3_mul_cross(v, v1, v2);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec3_reflect(lua_State *L)
{
    const vec3 *v1 = lua_math_vec3_check(L, 1);
    const vec3 *vn = lua_math_vec3_check(L, 2);
    vec3 v;
    vec3_reflect(v, v1, vn);
    lua_math_vec3_push(L, v);
    return 1;
}

int lua_math_vec4_cross(lua_State *L)
{
    const vec4 *v1 = lua_math_vec3_check(L, 1);
    const vec4 *v2 = lua_math_vec3_check(L, 2);
    vec4 v;
    vec4_mul_cross(v, v1, v2);
    lua_math_vec4_push(L, v);
    return 1;
}

int lua_math_vec4_reflect(lua_State *L)
{
    const vec3 *v1 = lua_math_vec4_check(L, 1);
    const vec3 *vn = lua_math_vec4_check(L, 2);
    vec4 v;
    vec4_reflect(v, v1, vn);
    lua_math_vec4_push(L, v);
    return 1;
}

void lua_math_vec_ext_load(lua_State *L)
{
    luaL_getmetatable(L, "vec3");
    lua_pushliteral(L, "crossProduct");
    lua_pushcfunction(L, lua_math_vec3_cross);
    lua_rawset(L, -3);
    lua_pushliteral(L, "reflect");
    lua_pushcfunction(L, lua_math_vec3_reflect);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    luaL_getmetatable(L, "vec4");
    lua_pushliteral(L, "crossProduct");
    lua_pushcfunction(L, lua_math_vec4_cross);
    lua_rawset(L, -3);
    lua_pushliteral(L, "reflect");
    lua_pushcfunction(L, lua_math_vec4_reflect);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

//static const luaL_reg lua_math_mat4x4_meta[] = {
//    {"row", lua_math_mat4x4_row},
//    {"column", lua_math_4x4_column},
//    {"invert", lua_math_mat4x4_invert},
//    {"inverted", lua_math_mat4x4_inverted},
//    {0, 0}
//};

void lua_math_mat4x4_load(lua_State *L)
{
    // TODO
}

void lua_math_load(lua_State *L)
{
    lua_math_vec2_load(L);
    lua_math_vec3_load(L);
    lua_math_vec4_load(L);

    lua_math_vec_ext_load(L);

    lua_math_mat4x4_load(L);
}

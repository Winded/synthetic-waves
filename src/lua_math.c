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
            v[i] = lua_tonumber(L, i + 1);\
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
    luaL_newmetatable(L, "vec"#n);\
    luaL_openlib(L, 0, lua_math_vec##n##_meta, 0);\
    lua_pushliteral(L, "__metatable");\
    lua_pushvalue(L, -2);\
    lua_rawset(L, -3);\
    lua_pop(L, 1);\
    \
    lua_pushcfunction(L, lua_math_vec##n##_create);\
    lua_setfield(L, -2, "vec"#n);\
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

mat4x4 *lua_math_mat4x4_to(lua_State *L, int index)
{
    mat4x4 *m = (mat4x4*)lua_touserdata(L, index);
    if(m == NULL) luaL_typerror(L, index, "mat4x4");
    return m;
}

mat4x4 *lua_math_mat4x4_check(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TUSERDATA);
    mat4x4 *m = (mat4x4*)luaL_checkudata(L, index, "mat4x4");
    if(m == NULL) luaL_typerror(L, index, "mat4x4");
    return m;
}

void lua_math_mat4x4_push(lua_State *L, const mat4x4 *m)
{
    mat4x4 *lm = (mat4x4*)lua_newuserdata(L, sizeof(mat4x4));
    memcpy(lm, m, sizeof(mat4x4));
    luaL_getmetatable(L, "mat4x4");
    lua_setmetatable(L, -2);
}

int lua_math_mat4x4_create(lua_State *L)
{
    mat4x4 m;
    mat4x4_identity(m);
    lua_math_mat4x4_push(L, m);
    return 1;
}

int lua_math_mat4x4_get(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    int rowIdx = luaL_checkinteger(L, 2);
    int colIdx = luaL_checkinteger(L, 3);
    if(rowIdx <= 0 || rowIdx > 4 || colIdx <= 0 || colIdx > 4) return 0;

    float value = (*m)[rowIdx - 1][colIdx - 1];
    lua_pushnumber(L, value);
    return 1;
}

int lua_math_mat4x4_set(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    int rowIdx = luaL_checkinteger(L, 2);
    int colIdx = luaL_checkinteger(L, 3);
    float value = luaL_checknumber(L, 4);
    if(rowIdx <= 0 || rowIdx > 4 || colIdx <= 0 || colIdx > 4) return 0;

    (*m)[rowIdx - 1][colIdx - 1] = value;
    return 0;
}

int lua_math_mat4x4_row(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    int rowIdx = luaL_checkinteger(L, 2);
    if(rowIdx <= 0 || rowIdx > 4) return 0;

    vec4 v;
    mat4x4_row(v, m, rowIdx - 1);
    lua_math_vec4_push(L, v);
    return 1;
}

int lua_math_mat4x4_col(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    int colIdx = luaL_checkinteger(L, 2);
    if(colIdx <= 0 || colIdx > 4) return 0;

    vec4 v;
    mat4x4_col(v, m, colIdx - 1);
    lua_math_vec4_push(L, v);
    return 1;
}

int lua_math_mat4x4_copy(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    mat4x4 t;
    mat4x4_dup(t, m);
    lua_math_mat4x4_push(L, t);
    return 1;
}

int lua_math_mat4x4_invert(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    mat4x4_invert(m, m);
    return 0;
}

int lua_math_mat4x4_inverted(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    mat4x4 t;
    mat4x4_invert(t, m);
    lua_math_mat4x4_push(L, t);
    return 1;
}

int lua_math_mat4x4_translate(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    const vec3 *v = lua_math_vec3_check(L, 2);

    mat4x4_translate(m, (*v)[0], (*v)[1], (*v)[2]);
    return 0;
}

int lua_math_mat4x4_translate_in_place(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    const vec3 *v = lua_math_vec3_check(L, 2);

    mat4x4_translate_in_place(m, (*v)[0], (*v)[1], (*v)[2]);
    return 0;
}

int lua_math_mat4x4_rotate(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    const vec3 *v = lua_math_vec3_check(L, 2);
    float angle = luaL_checknumber(L, 3);
    angle *= DEG2RAD;

    mat4x4_rotate(m, m, (*v)[0], (*v)[1], (*v)[2], angle);
    return 0;
}

int lua_math_mat4x4_rotate_euler(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    const vec3 *v = lua_math_vec3_check(L, 2);

    mat4x4_rotate_X(m, m, (*v)[0] * DEG2RAD);
    mat4x4_rotate_Y(m, m, (*v)[1] * DEG2RAD);
    mat4x4_rotate_Z(m, m, (*v)[2] * DEG2RAD);
    return 0;
}

int lua_math_mat4x4_scale(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);

    if(lua_isnumber(L, 2)) {
        float value = luaL_checknumber(L, 2);
        mat4x4_scale(m, m, value);
        return 0;
    }

    const vec3 *v = lua_math_mat4x4_check(L, 2);
    mat4x4_scale_aniso(m, m, (*v)[0], (*v)[1], (*v)[2]);
    return 0;
}

int lua_math_mat4x4_ortho(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    float l = luaL_checknumber(L, 2);
    float r = luaL_checknumber(L, 3);
    float b = luaL_checknumber(L, 4);
    float t = luaL_checknumber(L, 5);
    float n = luaL_checknumber(L, 6);
    float f = luaL_checknumber(L, 7);

    mat4x4_ortho(m, l, r, b, t, n, f);
    return 0;
}

int lua_math_mat4x4_perspective(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    float y_fov = luaL_checknumber(L, 2);
    float aspect = luaL_checknumber(L, 3);
    float n = luaL_checknumber(L, 4);
    float f = luaL_checknumber(L, 5);

    mat4x4_perspective(m, y_fov * DEG2RAD, aspect, n, f);
    return 0;
}

int lua_math_mat4x4_look_at(lua_State *L)
{
    mat4x4 *m = lua_math_mat4x4_check(L, 1);
    vec3 *eye = lua_math_vec3_check(L, 2);
    vec3 *center = lua_math_vec3_check(L, 3);
    vec3 *up = lua_math_vec3_check(L, 4);

    mat4x4_look_at(m, eye, center, up);
    return 0;
}

int lua_math_mat4x4_equals(lua_State *L)
{
    const mat4x4 *m1 = lua_math_mat4x4_check(L, 1);
    const mat4x4 *m2 = lua_math_mat4x4_check(L, 2);

    int equal = 1;
    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++) {
            if((*m1)[x][y] != (*m2)[x][y]) {
                equal = 0;
            }
        }
    }

    lua_pushboolean(L, equal);
    return 1;
}

int lua_math_mat4x4_add(lua_State *L)
{
    const mat4x4 *m1 = lua_math_mat4x4_check(L, 1);
    const mat4x4 *m2 = lua_math_mat4x4_check(L, 2);

    mat4x4 m;
    mat4x4_add(m, m1, m2);
    lua_math_mat4x4_push(L, m);
    return 1;
}

int lua_math_mat4x4_sub(lua_State *L)
{
    const mat4x4 *m1 = lua_math_mat4x4_check(L, 1);
    const mat4x4 *m2 = lua_math_mat4x4_check(L, 2);

    mat4x4 m;
    mat4x4_sub(m, m1, m2);
    lua_math_mat4x4_push(L, m);
    return 1;
}

int lua_math_mat4x4_mul(lua_State *L)
{
    const mat4x4 *m1 = lua_math_mat4x4_check(L, 1);
    if(!lua_isuserdata(L, 2)) {
        luaL_error(L, "Argument #2 is not vec3 or mat4x4\n");
        return 0;
    }

    lua_getmetatable(L, 2);
    luaL_getmetatable(L, "vec4");
    luaL_getmetatable(L, "mat4x4");
    if(lua_equal(L, -2, -3)) {
        const vec4 *v1 = lua_math_vec4_check(L, 2);
        vec4 v;
        mat4x4_mul_vec4(v, m1, v1);
        lua_math_vec4_push(L, v);
        return 1;
    }
    else if(lua_equal(L, -1, -3)) {
        const mat4x4 *m2 = lua_math_mat4x4_check(L, 2);
        mat4x4 m;
        mat4x4_mul(m, m1, m2);
        lua_math_mat4x4_push(L, m);
        return 1;
    }
    else {
        luaL_error(L, "Argument #2 is not vec3 or mat4x4\n");
        return 0;
    }
}

int lua_math_mat4x4_tostring(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    lua_pushfstring(L, "[%f, %f, %f, %f]\n"
                       "[%f, %f, %f, %f]\n"
                       "[%f, %f, %f, %f]\n"
                       "[%f, %f, %f, %f]\n",
                    (*m)[0][0], (*m)[0][1], (*m)[0][2], (*m)[0][3],
                    (*m)[1][0], (*m)[1][1], (*m)[1][2], (*m)[1][3],
                    (*m)[2][0], (*m)[2][1], (*m)[2][2], (*m)[2][3],
                    (*m)[3][0], (*m)[3][1], (*m)[3][2], (*m)[3][3]);
    return 1;
}

int lua_math_mat4x4_type(lua_State *L)
{
    const mat4x4 *m = lua_math_mat4x4_check(L, 1);
    lua_pushstring(L, "mat4x4");
    return 1;
}

static const luaL_reg lua_math_mat4x4_meta[] = {
    {"get", lua_math_mat4x4_get},
    {"set", lua_math_mat4x4_set},
    {"row", lua_math_mat4x4_row},
    {"col", lua_math_mat4x4_col},
    {"copy", lua_math_mat4x4_copy},
    {"invert", lua_math_mat4x4_invert},
    {"inverted", lua_math_mat4x4_inverted},
    {"translate", lua_math_mat4x4_translate},
    {"translateInPlace", lua_math_mat4x4_translate_in_place},
    {"rotate", lua_math_mat4x4_rotate},
    {"rotateEuler", lua_math_mat4x4_rotate_euler},
    {"scale", lua_math_mat4x4_scale},
    {"ortho", lua_math_mat4x4_ortho},
    {"perspective", lua_math_mat4x4_perspective},
    {"lookAt", lua_math_mat4x4_look_at},
    {"__eq", lua_math_mat4x4_equals},
    {"__add", lua_math_mat4x4_add},
    {"__sub", lua_math_mat4x4_sub},
    {"__mul", lua_math_mat4x4_mul},
    {"__tostring", lua_math_mat4x4_tostring},
    {"__type", lua_math_mat4x4_type},
    {0, 0}
};

void lua_math_mat4x4_load(lua_State *L)
{
    luaL_newmetatable(L, "mat4x4");
    luaL_openlib(L, 0, lua_math_mat4x4_meta, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    lua_pushcfunction(L, lua_math_mat4x4_create);
    lua_setfield(L, -2, "mat4x4");
}

void lua_math_load(lua_State *L)
{
    lua_math_vec2_load(L);
    lua_math_vec3_load(L);
    lua_math_vec4_load(L);

    lua_math_vec_ext_load(L);

    lua_math_mat4x4_load(L);

    // math globals
    lua_pushnumber(L, DEG2RAD);
    lua_setfield(L, -2, "DEG2RAD");
    lua_pushnumber(L, RAD2DEG);
    lua_setfield(L, -2, "RAD2DEG");
}

void lua_math_test(lua_State *L)
{
    int status = luaL_loadfile(L, "../tests/math.lua");
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

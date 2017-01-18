#include <graphics.h>
#include <util.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(USE_OPENGL)
#include <GL/glew.h>
#elif defined(USE_DIRECTX)
// TODO
#endif

#define MAX_VERTEX_ARRAYS 1000
#define MAX_VERTEX_ARRAY_ATTRIBUTES 10

static struct {
        char initialized;
        graphics_version version;
} graphics_config = {0};

typedef struct {
        char is_valid;
        int id, size;
} vertex_array_attribute;
typedef struct {
        int id, vbo, ebo;
        int element_size;
        vertex_array_attribute attributes[MAX_VERTEX_ARRAY_ATTRIBUTES];
} vertex_array;

static vertex_array vertex_arrays[MAX_VERTEX_ARRAYS];
static int vertex_array_max_id = 0;
static int element_size = 0;

int graphics_initialized()
{
    return graphics_config.initialized;
}

void graphics_init(graphics_version version)
{
    if(graphics_config.initialized) {
        fprintf(stderr, "Graphics init error: already initialized\n");
        return;
    }

#if defined(USE_OPENGL)
    glewExperimental = GL_TRUE;
    glewInit();
#elif defined(USE_DIRECTX)
    // TODO
#endif

    memset(&graphics_config, 0, sizeof(graphics_config));
    memset(&vertex_arrays, 0, sizeof(vertex_arrays));
    vertex_array_max_id = 0;
    graphics_config.version = version;
    graphics_config.initialized = 1;
}

int graphics_is_feature_enabled(graphics_feature feature)
{
    switch(feature) {
    case graphics_feature_depth_test:
#ifdef USE_OPENGL
        return glIsEnabled(GL_DEPTH_TEST);
#endif
    case graphics_feature_blend:
#ifdef USE_OPENGL
        return glIsEnabled(GL_BLEND);
#endif
    default:
        return 0;
    }
}

void graphics_set_feature_enabled(graphics_feature feature, int enabled)
{
    switch(feature) {
    case graphics_feature_depth_test:
#ifdef USE_OPENGL
        if(enabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
        }
        else
            glDisable(GL_DEPTH_TEST);
#endif
        break;
    case graphics_feature_blend:
#ifdef USE_OPENGL
        if(enabled) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
            glDisable(GL_BLEND);
#endif
        break;
    default:
        break;
    }
}

int graphics_shader_create(const char *code, graphics_shader_type type, char *errBuf)
{
#if defined(USE_OPENGL)
    GLenum stype = GL_VERTEX_SHADER;
    switch(type) {
    case graphics_vertex_shader:
        stype = GL_VERTEX_SHADER;
        break;
    case graphics_fragment_shader:
        stype = GL_FRAGMENT_SHADER;
        break;
    }

    GLuint s = glCreateShader(stype);
    GLint code_size = strlen(code);
    glShaderSource(s, 1, &code, &code_size);
    glCompileShader(s);

    int length;
    char log[2048];
    glGetShaderInfoLog(s, 2048, &length, log);
    if(length > 0) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL shader error: %s", log);
        }
        if(s) {
            glDeleteShader(s);
        }
        return 0;
    }

    return s;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_shader_destroy(int handle)
{
#if defined(USE_OPENGL)
    glDeleteShader(handle);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_shader_program_create(int vertex_shader_handle, int fragment_shader_handle, char *errBuf)
{
#if defined(USE_OPENGL)
    GLuint p = glCreateProgram();
    glAttachShader(p, vertex_shader_handle);
    glAttachShader(p, fragment_shader_handle);
    glLinkProgram(p);

    int length;
    char log[2048];
    glGetProgramInfoLog(p, 2048, &length, log);
    if(length > 0) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL shader program error: %s", log);
        }
        if(p) {
            glDeleteProgram(p);
        }
        return 0;
    }

    return p;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_shader_program_destroy(int handle)
{
#if defined(USE_OPENGL)
    glDeleteProgram(handle);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_shader_program_use(int handle)
{
#if defined(USE_OPENGL)
    glUseProgram(handle);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_shader_param_name_to_id(int shader_program_handle, const char *name)
{
#if defined(USE_OPENGL)
    return glGetUniformLocation(shader_program_handle, name);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_shader_param_set(int id, const float *value, int size, char *errBuf)
{
#if defined(USE_OPENGL)
    switch(size) {
    case 1:
        glUniform1fv(id, 1, value);
        break;
    case 2:
        glUniform2fv(id, 1, value);
        break;
    case 3:
        glUniform3fv(id, 1, value);
        break;
    case 4:
        glUniform4fv(id, 1, value);
        break;
    case 4 * 4:
        glUniformMatrix4fv(id, 1, GL_FALSE, value);
        break;
    default:
        assert("Unsupported param size" && 0);
        break;
    }

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL shader param error: %s", glewGetErrorString(err));
        }
        return 0;
    }

    return 1;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_texture_create(const void *data, int width, int height, int num_channels, char *errBuf)
{
#if defined(USE_OPENGL)
    GLuint tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);

    float borderColor[] = { 1.f, 1.f, 1.f ,1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)GL_NEAREST);

    GLenum format = GL_NONE;
    switch(num_channels) {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        if(errBuf) {
            sprintf(errBuf, "Invalid amount of texture channels: %i", num_channels);
        }
        if(tex) {
            glDeleteTextures(1, &tex);
        }
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL texture error: %i\n", err);
        }
        if(tex) {
            glDeleteTextures(1, &tex);
        }
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_texture_update(int handle, const void *data, int width, int height, int num_channels, char *errBuf)
{
#if defined(USE_OPENGL)
    glBindTexture(GL_TEXTURE_2D, handle);

    float borderColor[] = { 1.f, 1.f, 1.f ,1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)GL_NEAREST);

    GLenum format = GL_NONE;
    switch(num_channels) {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        if(errBuf) {
            sprintf(errBuf, "Invalid amount of texture channels: %i", num_channels);
        }
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL texture error: %i\n", err);
        }
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return 1;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_texture_destroy(int handle)
{
#if defined(USE_OPENGL)
    glDeleteTextures(1, &handle);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_texture_use(int handle)
{
#if defined(USE_OPENGL)
    glBindTexture(GL_TEXTURE_2D, handle);
    glActiveTexture(GL_TEXTURE0);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_vertex_array_create(float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size, char *errBuf)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id) continue;
        va = v;
        break;
    }
    va->id = vertex_array_max_id + 1;
    va->element_size = element_buffer_size;

#if defined(USE_OPENGL)
    glGenBuffers(1, &(va->vbo));
    glGenBuffers(1, &(va->ebo));

    glBindBuffer(GL_ARRAY_BUFFER, va->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, va->ebo);

    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(float), vertex_buffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size * sizeof(int), element_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif defined(USE_DIRECTX)
    // TODO
#endif

    vertex_array_max_id++;
    return va->id;
}

int graphics_vertex_array_update(int handle, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size, char *errBuf)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id == handle) {
            va = v;
            break;
        }
    }
    va->element_size = element_buffer_size;

#if defined(USE_OPENGL)
    glBindBuffer(GL_ARRAY_BUFFER, va->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, va->ebo);

    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(float), vertex_buffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size * sizeof(int), element_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        if(errBuf) {
            sprintf(errBuf, "OpenGL object error: %i\n", err);
        }
        return 0;
    }

    return 1;
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

int graphics_vertex_array_set_attribute(int handle, int id, int size)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id == handle) {
            va = v;
            break;
        }
    }
    if(!va) return 0;

    vertex_array_attribute *att = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAY_ATTRIBUTES; i++) {
        vertex_array_attribute *a = &(va->attributes[i]);
        if(a->is_valid && a->id == id) {
            att = a;
            break;
        }
    }
    if(!att) {
        for(int i = 0; i < MAX_VERTEX_ARRAY_ATTRIBUTES; i++) {
            vertex_array_attribute *a = &(va->attributes[i]);
            if(!a->is_valid) {
                att = a;
                break;
            }
        }
    }

    att->is_valid = 1;
    att->id = id;
    att->size = size;

    return 1;
}

int graphics_vertex_array_delete_attribute(int handle, int id)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id == handle) {
            va = v;
            break;
        }
    }
    if(!va) return 0;

    vertex_array_attribute *att = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAY_ATTRIBUTES; i++) {
        vertex_array_attribute *a = &(va->attributes[i]);
        if(a->is_valid && a->id == id) {
            att = a;
            break;
        }
    }

    if(!att) {
        return 0;
    }

    memset(att, 0, sizeof(vertex_array_attribute));
    return 1;
}

int graphics_vertex_array_destroy(int handle)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id == handle) {
            va = v;
            break;
        }
    }
    if(!va) return 0;

#if defined(USE_OPENGL)
    glDeleteBuffers(1, &(va->vbo));
    glDeleteBuffers(1, &(va->ebo));
#elif defined(USE_DIRECTX)
    // TODO
#endif

    memset(va, 0, sizeof(vertex_array));
    return 1;
}

int graphics_vertex_array_use(int handle)
{
    vertex_array *va = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array *v = &(vertex_arrays[i]);
        if(v->id == handle) {
            va = v;
            break;
        }
    }
    if(!va) return 0;

#if defined(USE_OPENGL)
    // Bind vertex array
    glBindBuffer(GL_ARRAY_BUFFER, va->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, va->ebo);
    int totalAttribSize = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array_attribute *att = &(va->attributes[i]);
        if(!att->is_valid) continue;
        totalAttribSize += att->size;
    }
    int offset = 0;
    for(int i = 0; i < MAX_VERTEX_ARRAYS; i++) {
        vertex_array_attribute *att = &(va->attributes[i]);
        if(!att->is_valid) continue;
        glVertexAttribPointer(att->id, att->size, GL_FLOAT, GL_FALSE, totalAttribSize * sizeof(float), (GLvoid*)(offset * sizeof(float)));
        glEnableVertexAttribArray(att->id);
        offset += att->size;
    }
    element_size = va->element_size;
    GLenum err = glGetError();
    if(err != GL_FALSE) {
        return 0;
    }
#elif defined(USE_DIRECTX)
    // TODO
#endif

    return 1;
}

void graphics_draw()
{
#if defined(USE_OPENGL)
    // Draw
    glDrawElements(GL_TRIANGLES, element_size, GL_UNSIGNED_INT, 0);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

void graphics_clear(const float *color)
{
#if defined(USE_OPENGL)
    glClearColor(color[0], color[1], color[2], color[3]);
    GLbitfield clearFlags = GL_COLOR_BUFFER_BIT;
    if(graphics_is_feature_enabled(graphics_feature_depth_test))
        clearFlags |= GL_DEPTH_BUFFER_BIT;
    glClear(clearFlags);
#elif defined(USE_DIRECTX)
    // TODO
#endif
}

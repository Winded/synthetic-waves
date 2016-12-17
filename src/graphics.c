#include "graphics.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>

graphics_context *graphics_context_create()
{
    graphics_context *ctx = (graphics_context*)calloc(1, sizeof(graphics_context));
    return ctx;
}

void graphics_context_clear_objects(graphics_context *context)
{
    graphics_object *nxt = context->objects;
    while(nxt) {
        graphics_object *cur = nxt;
        nxt = nxt->next;
        graphics_object_destroy(context, cur);
    }
}

void graphics_context_clear_textures(graphics_context *context)
{
    graphics_texture *nxt = context->textures;
    while(nxt) {
        graphics_texture *cur = nxt;
        nxt = nxt->next;
        graphics_texture_destroy(context, cur);
    }
}

void graphics_context_clear_shader_params(graphics_context *context)
{
    graphics_shader_param *nxt = context->shader_params;
    while(nxt) {
        graphics_shader_param *cur = nxt;
        nxt = nxt->next;
        graphics_shader_param_delete(context, cur->name);
    }
}

void graphics_context_clear_shader_programs(graphics_context *context)
{
    graphics_shader_program *nxt = context->shader_programs;
    while(nxt) {
        graphics_shader_program *cur = nxt;
        nxt = nxt->next;
        graphics_shader_program_destroy(context, cur);
    }
}

void graphics_context_clear_shaders(graphics_context *context)
{
    graphics_shader *nxt = context->shaders;
    while(nxt) {
        graphics_shader *cur = nxt;
        nxt = nxt->next;
        graphics_shader_destroy(context, cur);
    }
}

void graphics_context_destroy(graphics_context **context)
{
    graphics_context *ctx = *context;

    graphics_context_clear_objects(ctx);
    graphics_context_clear_textures(ctx);
    graphics_context_clear_shader_params(ctx);
    graphics_context_clear_shader_programs(ctx);
    graphics_context_clear_shaders(ctx);

    free(ctx);
    *context = 0;
}

graphics_shader *graphics_shader_create(graphics_context *context, const char *code, graphics_shader_type type)
{
    graphics_shader *shader = (graphics_shader*)calloc(1, sizeof(graphics_shader));
    shader->type = type;
    shader->code = strcpy((char*)calloc(1, strlen(code) + 1), code);

    if(!context->shaders) {
        context->shaders = shader;
    }
    else {
        graphics_shader *s = context->shaders;
        while(s->next) {
            s = s->next;
        }
        shader->prev = s;
        s->next = shader;
    }

#ifdef USE_OPENGL
    GLenum stype = GL_VERTEX_SHADER;
    switch(type) {
    case graphics_vertex_shader:
        stype = GL_VERTEX_SHADER;
        break;
    case graphics_fragment_shader:
        stype = GL_FRAGMENT_SHADER;
        break;
    }

    GLuint g_sh = glCreateShader(stype);
    GLint code_size = strlen(code);
    glShaderSource(g_sh, 1, &shader->code, &code_size);
    glCompileShader(g_sh);

    int length;
    char log[2048];
    glGetShaderInfoLog(g_sh, 2048, &length, log);
    if(length > 0) {
        fprintf(stderr, "OpenGL shader error: %s\n", log);
    }

    shader->handle = g_sh;
#endif

    return shader;
}

void graphics_shader_destroy(graphics_context *context, graphics_shader *shader)
{
    if(shader->handle) {
#ifdef USE_OPENGL
        glDeleteShader(shader->handle);
#endif
    }

    if(context->shaders == shader) {
        context->shaders = shader->next ? shader->next : 0;
    }

    if(shader->next) {
        shader->next->prev = shader->prev;
    }
    if(shader->prev) {
        shader->prev->next = shader->next;
    }

    free(shader->code);
    free(shader);
}

graphics_shader_program *graphics_shader_program_create(graphics_context *context, graphics_shader *vertex_shader, graphics_shader *fragment_shader)
{
    graphics_shader_program *program = (graphics_shader_program*)calloc(1, sizeof(graphics_shader_program));

    if(!context->shader_programs) {
        context->shader_programs = program;
    }
    else {
        graphics_shader_program *p = context->shader_programs;
        while(p->next) {
            p = p->next;
        }
        program->prev = p;
        p->next = program;
    }

    assert(vertex_shader && fragment_shader && vertex_shader->type == graphics_vertex_shader && fragment_shader->type == graphics_fragment_shader);
    program->vertex_shader = vertex_shader;
    program->fragment_shader = fragment_shader;

#ifdef USE_OPENGL
    GLuint p = glCreateProgram();
    glAttachShader(p, vertex_shader->handle);
    glAttachShader(p, fragment_shader->handle);
    glLinkProgram(p);

    int length;
    char log[2048];
    glGetProgramInfoLog(p, 2048, &length, log);
    if(length > 0) {
        fprintf(stderr, "OpenGL shader program error: %s\n", log);
    }

    program->handle = p;
#endif

    return program;
}

void graphics_shader_program_destroy(graphics_context *context, graphics_shader_program *program)
{
    if(program->handle) {
#ifdef USE_OPENGL
        glDeleteProgram(program->handle);
#endif
    }

    if(context->shader_programs == program) {
        context->shader_params = program->next;
    }

    if(program->next) {
        program->next->prev = program->prev;
    }
    if(program->prev) {
        program->prev->next = program->next;
    }

    free(program);
}

int graphics_shader_param_get(graphics_context *context, const char *name, float *value, int *size)
{
    graphics_shader_param *param = 0;
    graphics_shader_param *p = context->shader_params;
    while(p) {
        if(strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
        p = p->next;
    }

    if(param) {
        memcpy(value, p->value, p->size * sizeof(float));
        *size = p->size;
        return 1;
    }
    else {
        return 0;
    }
}

void graphics_shader_param_set(graphics_context *context, const char *name, float *value, int size)
{
    // Check if we already have this parameter
    graphics_shader_param *param = 0;
    graphics_shader_param *p = context->shader_params;
    while(p) {
        if(strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
        p = p->next;
    }

    // if not, add it to our param list
    if(!param) {
        param = (graphics_shader_param*)calloc(1, sizeof(graphics_shader_param));
        strcpy(param->name, name);
        if(!context->shader_params) {
            context->shader_params = param;
        }
        else {
            p = context->shader_params;
            while(p->next) {
                p = p->next;
            }
            p->next = param;
            param->prev = p;
        }
    }

    memcpy(param->value, value, size * sizeof(float));
    param->size = size;
}

void graphics_shader_param_delete(graphics_context *context, const char *name)
{
    graphics_shader_param *param = 0;
    graphics_shader_param *p = context->shader_params;
    while(p) {
        if(strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
        p = p->next;
    }

    if(param) {
        if(context->shader_params == param) {
            context->shader_params = param->next;
        }

        if(param->next) {
            param->next->prev = param->prev;
        }
        if(param->prev) {
            param->prev->next = param->next;
        }

        free(param);
    }
}

graphics_texture *graphics_texture_create(graphics_context *context, const void *data, int width, int height)
{
    graphics_texture *texture = (graphics_texture*)calloc(1, sizeof(graphics_texture));
    texture->data = data;
    texture->width = width;
    texture->height = height;

    if(!context->textures) {
        context->textures = texture;
    }
    else {
        graphics_texture *t = context->textures;
        while(t->next) {
            t = t->next;
        }
        t->next = texture;
        texture->prev = t;
    }

#ifdef USE_OPENGL
    GLuint glTex;
    glGenTextures(1, &glTex);

    glBindTexture(GL_TEXTURE_2D, glTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL texture error: %i\n", err);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    texture->handle = glTex;
#endif

    return texture;
}

void graphics_texture_destroy(graphics_context *context, graphics_texture *texture)
{
    if(texture->handle) {
#ifdef USE_OPENGL
        glDeleteTextures(1, &texture->handle);
#endif
    }

    if(context->textures == texture) {
        context->textures = texture->next;
    }

    if(texture->next) {
        texture->next->prev = texture->prev;
    }
    if(texture->prev) {
        texture->prev->next = texture->next;
    }

    free(texture->data);
    free(texture);
}

graphics_object *graphics_object_create(graphics_context *context, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size)
{
    graphics_object *object = (graphics_object*)calloc(1, sizeof(graphics_object));

    object->vertex_buffer = malloc(vertex_buffer_size * sizeof(float));
    memcpy(object->vertex_buffer, vertex_buffer, vertex_buffer_size * sizeof(float));
    object->vertex_buffer_size = vertex_buffer_size;

    object->element_buffer = malloc(element_buffer_size * sizeof(int));
    memcpy(object->element_buffer, element_buffer, element_buffer_size * sizeof(int));
    object->element_buffer_size = element_buffer_size;

    if(!context->objects) {
        context->objects = object;
    }
    else {
        graphics_object *o = context->objects;
        while(o->next) {
            o = o->next;
        }
        o->next = object;
        object->prev = o;
    }

#ifdef USE_OPENGL
    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, object->vertex_buffer_size * sizeof(float), object->vertex_buffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->element_buffer_size * sizeof(int), object->element_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL object error: %i\n", err);
    }

    object->vbo_handle = vbo;
    object->ebo_handle = ebo;
#endif

    return object;
}

void graphics_object_update(graphics_object *object, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size)
{
    if(vertex_buffer && vertex_buffer_size > 0) {
        if(vertex_buffer_size != object->vertex_buffer_size) {
            object->vertex_buffer = realloc(object->vertex_buffer, vertex_buffer_size);
        }
        memcpy(object->vertex_buffer, vertex_buffer, vertex_buffer_size * sizeof(float));
        object->vertex_buffer_size = vertex_buffer_size;
    }
    if(element_buffer && element_buffer_size > 0) {
        if(element_buffer_size != object->element_buffer_size) {
            object->element_buffer = realloc(object->element_buffer, element_buffer_size);
        }
        memcpy(object->element_buffer, element_buffer, element_buffer_size * sizeof(float));
        object->element_buffer_size = element_buffer_size;
    }

#ifdef USE_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, object->vbo_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo_handle);

    glBufferData(GL_ARRAY_BUFFER, object->vertex_buffer_size * sizeof(float), object->vertex_buffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->element_buffer_size * sizeof(int), object->element_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL object error: %i\n", err);
    }
#endif
}

void graphics_object_set_attribute(graphics_object *object, int id, int size)
{
    graphics_object_attribute *attribute = 0;
    graphics_object_attribute *a = object->attributes;
    while(a) {
        if(a->id == id) {
            attribute = a;
            break;
        }
        a = a->next;
    }

    if(!attribute) {
        attribute = (graphics_object_attribute*)calloc(1, sizeof(graphics_object_attribute));
        attribute->id = id;
        if(!object->attributes) {
            object->attributes = attribute;
        }
        else {
            a = object->attributes;
            while(a->next) {
                a = a->next;
            }
            a->next = attribute;
            attribute->prev = a;
        }
    }

    attribute->size = size;
}

void graphics_object_destroy(graphics_context *context, graphics_object *object)
{
    if(object->vao_handle) {
#ifdef USE_OPENGL
        glDeleteVertexArrays(1, &object->vao_handle);
#endif
    }
    if(object->vbo_handle) {
#ifdef USE_OPENGL
        glDeleteBuffers(1, &object->vbo_handle);
#endif
    }
    if(object->ebo_handle) {
#ifdef USE_OPENGL
        glDeleteBuffers(1, &object->ebo_handle);
#endif
    }

    if(context->objects == object) {
        context->objects = object->next;
    }

    if(object->next) {
        object->next->prev = object->prev;
    }
    if(object->prev) {
        object->prev->next = object->next;
    }

    free(object->vertex_buffer);
    free(object->element_buffer);
    free(object);
}

void graphics_use_program(graphics_context *context, graphics_shader_program *program)
{
    context->current_program = program;
#ifdef USE_OPENGL
    glUseProgram(program->handle);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL program bind error: %i\n", err);
    }
#endif
}

void graphics_use_texture(graphics_context *context, graphics_texture *texture)
{
#ifdef USE_OPENGL
    glBindTexture(GL_TEXTURE_2D, texture->handle);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL texture bind error: %i\n", err);
    }
#endif
}

void graphics_use_shader_params(graphics_context *context)
{
#ifdef USE_OPENGL
    graphics_shader_param *param = context->shader_params;
    while(param) {
        int location = glGetUniformLocation(context->current_program->handle, param->name);
        if(location != -1) {
            switch(param->size) {
            case 1:
                glUniform1fv(location, 1, param->value);
                break;
            case 2:
                glUniform2fv(location, 1, param->value);
                break;
            case 3:
                glUniform3fv(location, 1, param->value);
                break;
            case 4:
                glUniform4fv(location, 1, param->value);
                break;
            case 4 * 4:
                glUniformMatrix4fv(location, 1, GL_FALSE, param->value);
                break;
            default:
                assert("Unsupported param size" && 0);
                break;
            }
        }

        GLenum err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL shader param error: %i\n", err);
        }

        param = param->next;
    }
#endif
}

void graphics_use_object(graphics_context *context, graphics_object *object)
{
    context->current_object = object;

#ifdef USE_OPENGL
    if(object) {
        glBindBuffer(GL_ARRAY_BUFFER, object->vbo_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo_handle);

        GLenum err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL object error: %i\n", err);
        }

        int totalAttribSize = 0;
        graphics_object_attribute *attribute = object->attributes;
        while(attribute) {
            totalAttribSize += attribute->size;
            attribute = attribute->next;
        }
        attribute = object->attributes;
        int offset = 0;
        while(attribute) {
            glVertexAttribPointer(attribute->id, attribute->size, GL_FLOAT, GL_FALSE, totalAttribSize * sizeof(float), (GLvoid*)(offset * sizeof(float)));
            glEnableVertexAttribArray(attribute->id);
            offset += attribute->size;
            attribute = attribute->next;
        }

        err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL object error: %i\n", err);
        }
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
#endif
}

void graphics_draw(graphics_context *context)
{
    if(!context->current_object) return;

#ifdef USE_OPENGL
    glDrawElements(GL_TRIANGLES, context->current_object->element_buffer_size, GL_UNSIGNED_INT, 0);
#endif
}

void graphics_clear(graphics_context *context, float *color)
{
#ifdef USE_OPENGL
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

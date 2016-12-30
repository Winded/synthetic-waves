#include <graphics.h>
#include <util.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>

graphics_context *graphics_context_create()
{
    graphics_context *ctx = (graphics_context*)calloc(1, sizeof(graphics_context));
    return ctx;
}

void graphics_context_destroy(graphics_context **context)
{
    graphics_context *ctx = *context;

    for(int i = 0; i < ARRSIZE_RESERVE; i++) {
        if(ctx->shaders[i]) {
            for(int ii = 0; ii < ARRSIZE_SHADER; ii++) {
                graphics_shader *s = &(ctx->shaders[i][ii]);
                if(s->code)
                    free(s->code);
            }
            free(ctx->shaders[i]);
        }
        if(ctx->shader_programs[i])
            free(ctx->shader_programs[i]);
        if(ctx->shader_params[i])
            free(ctx->shader_params[i]);
        if(ctx->textures[i]) {
            for(int ii = 0; ii < ARRSIZE_SHADER; ii++) {
                graphics_texture *t = &(ctx->textures[i][ii]);
                if(t->data)
                    free(t->data);
            }
            free(ctx->textures[i]);
        }
        if(ctx->vertex_arrays[i]) {
            for(int ii = 0; ii < ARRSIZE_SHADER; ii++) {
                graphics_vertex_array *va = &(ctx->vertex_arrays[i][ii]);
                if(va->vertex_buffer)
                    free(va->vertex_buffer);
                if(va->element_buffer)
                    free(va->element_buffer);
            }
            free(ctx->vertex_arrays[i]);
        }
        if(ctx->objects[i])
            free(ctx->objects[i]);
    }

    free(ctx);
    *context = 0;
}

graphics_shader *graphics_shader_create(graphics_context *context, const char *code, graphics_shader_type type)
{
    graphics_shader *shader = (graphics_shader*)util_reserve_item_exp(context->shaders, ARRSIZE_RESERVE, ARRSIZE_SHADER, sizeof(graphics_shader));

    shader->type = type;
    shader->code = strcpy((char*)calloc(1, strlen(code) + 1), code);

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

void graphics_shader_destroy(graphics_shader *shader)
{
    if(!shader->is_valid) return;

    if(shader->handle) {
#ifdef USE_OPENGL
        glDeleteShader(shader->handle);
#endif
    }

    free(shader->code);
    memset(shader, 0, sizeof(graphics_shader));
}

graphics_shader_program *graphics_shader_program_create(graphics_context *context, const graphics_shader *vertex_shader, const graphics_shader *fragment_shader)
{
    graphics_shader_program *program = (graphics_shader_program*)util_reserve_item_exp(context->shader_programs, ARRSIZE_RESERVE, ARRSIZE_SHADER_PROGRAM,
                                                                                       sizeof(graphics_shader_program));

    assert(vertex_shader->is_valid && fragment_shader->is_valid && vertex_shader->type == graphics_vertex_shader && fragment_shader->type == graphics_fragment_shader);
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

void graphics_shader_program_destroy(graphics_shader_program *program)
{
    if(!program->is_valid) return;

    if(program->handle) {
#ifdef USE_OPENGL
        glDeleteProgram(program->handle);
#endif
    }

    memset(program, 0, sizeof(graphics_shader_program));
}

int graphics_shader_param_get(graphics_context *context, const char *name, float *value, int *size)
{
    graphics_shader_param *param = 0;
    for(int i = 0; i < ARRSIZE_RESERVE; i++) {
        if(param || !context->shader_params[i]) break;
        for(int ii = 0; ii < ARRSIZE_SHADER_PARAM; ii++) {
            graphics_shader_param *p = &(context->shader_params[i][ii]);
            if(p->is_valid && p->name == name) {
                param = p;
                break;
            }
        }
    }

    if(param) {
        memcpy(value, param->value, param->size * sizeof(float));
        *size = param->size;
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
    for(int i = 0; i < ARRSIZE_RESERVE; i++) {
        if(param || !context->shader_params[i]) break;
        for(int ii = 0; ii < ARRSIZE_SHADER_PARAM; ii++) {
            graphics_shader_param *p = &(context->shader_params[i][ii]);
            if(p->is_valid && strcmp(p->name, name) == 0) {
                param = p;
                break;
            }
        }
    }

    // if not, add it to our param list
    if(!param) {
        param = (graphics_shader_param*)util_reserve_item_exp(context->shader_params, ARRSIZE_RESERVE, ARRSIZE_SHADER_PARAM, sizeof(graphics_shader_param));
        strcpy(param->name, name);
    }

    memcpy(param->value, value, size * sizeof(float));
    param->size = size;
}

void graphics_shader_param_delete(graphics_context *context, const char *name)
{
    graphics_shader_param *param = 0;
    for(int i = 0; i < ARRSIZE_RESERVE; i++) {
        if(param || !context->shader_params[i]) break;
        for(int ii = 0; ii < ARRSIZE_SHADER_PARAM; ii++) {
            graphics_shader_param *p = &(context->shader_params[i][ii]);
            if(p->is_valid && p->name == name) {
                param = p;
                break;
            }
        }
    }

    if(param) {
        memset(param, 0, sizeof(graphics_shader_param));
    }
}

graphics_texture *graphics_texture_create(graphics_context *context, const void *data, int width, int height)
{
    graphics_texture *texture = (graphics_texture*)util_reserve_item_exp(context->textures, ARRSIZE_RESERVE, ARRSIZE_TEXTURE, sizeof(graphics_texture));
    texture->data = data;
    texture->width = width;
    texture->height = height;

#ifdef USE_OPENGL
    GLuint glTex;
    glGenTextures(1, &glTex);

    glBindTexture(GL_TEXTURE_2D, glTex);

    float borderColor[] = { 1.f, 1.f, 1.f ,1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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

void graphics_texture_destroy(graphics_texture *texture)
{
    if(!texture->is_valid) return;

    if(texture->handle) {
#ifdef USE_OPENGL
        glDeleteTextures(1, &texture->handle);
#endif
    }

    free(texture->data);
    memset(texture, 0, sizeof(graphics_texture));
}

graphics_vertex_array *graphics_vertex_array_create(graphics_context *context, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size)
{
    graphics_vertex_array *object = (graphics_vertex_array*)util_reserve_item_exp(context->vertex_arrays, ARRSIZE_RESERVE, ARRSIZE_VA, sizeof(graphics_vertex_array));

    object->vertex_buffer = malloc(vertex_buffer_size * sizeof(float));
    memcpy(object->vertex_buffer, vertex_buffer, vertex_buffer_size * sizeof(float));
    object->vertex_buffer_size = vertex_buffer_size;

    object->element_buffer = malloc(element_buffer_size * sizeof(int));
    memcpy(object->element_buffer, element_buffer, element_buffer_size * sizeof(int));
    object->element_buffer_size = element_buffer_size;

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

void graphics_vertex_array_update(graphics_vertex_array *vertex_array, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size)
{
    if(!vertex_array->is_valid) return;

    if(vertex_buffer && vertex_buffer_size > 0) {
        if(vertex_buffer_size != vertex_array->vertex_buffer_size) {
            vertex_array->vertex_buffer = realloc(vertex_array->vertex_buffer, vertex_buffer_size);
        }
        memcpy(vertex_array->vertex_buffer, vertex_buffer, vertex_buffer_size * sizeof(float));
        vertex_array->vertex_buffer_size = vertex_buffer_size;
    }
    if(element_buffer && element_buffer_size > 0) {
        if(element_buffer_size != vertex_array->element_buffer_size) {
            vertex_array->element_buffer = realloc(vertex_array->element_buffer, element_buffer_size);
        }
        memcpy(vertex_array->element_buffer, element_buffer, element_buffer_size * sizeof(float));
        vertex_array->element_buffer_size = element_buffer_size;
    }

#ifdef USE_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array->vbo_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_array->ebo_handle);

    glBufferData(GL_ARRAY_BUFFER, vertex_array->vertex_buffer_size * sizeof(float), vertex_array->vertex_buffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_array->element_buffer_size * sizeof(int), vertex_array->element_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLenum err = glGetError();
    if(err != GL_FALSE) {
        fprintf(stderr, "OpenGL object error: %i\n", err);
    }
#endif
}

void graphics_vertex_array_set_attribute(graphics_vertex_array *vertex_array, int id, int size)
{
    graphics_vertex_array_attribute *attribute = 0;
    for(int i = 0; i < ARRSIZE_VA_ATTRIBUTE; i++) {
        if(vertex_array->attributes[i].is_valid && vertex_array->attributes[i].id == id) {
            attribute = &(vertex_array->attributes[i]);
            break;
        }
    }

    if(!attribute) {
        attribute = (graphics_vertex_array_attribute*)util_reserve_item_arr(vertex_array->attributes, ARRSIZE_VA_ATTRIBUTE,
                                                                            sizeof(graphics_vertex_array_attribute));
        attribute->id = id;
    }

    attribute->size = size;
}

void graphics_vertex_array_destroy(graphics_vertex_array *vertex_array)
{
    if(vertex_array->vao_handle) {
#ifdef USE_OPENGL
        glDeleteVertexArrays(1, &vertex_array->vao_handle);
#endif
    }
    if(vertex_array->vbo_handle) {
#ifdef USE_OPENGL
        glDeleteBuffers(1, &vertex_array->vbo_handle);
#endif
    }
    if(vertex_array->ebo_handle) {
#ifdef USE_OPENGL
        glDeleteBuffers(1, &vertex_array->ebo_handle);
#endif
    }

    free(vertex_array->vertex_buffer);
    free(vertex_array->element_buffer);
    memset(vertex_array, 0, sizeof(graphics_vertex_array));
}

graphics_object *graphics_object_create(graphics_context *context)
{
    graphics_object *object = (graphics_object*)util_reserve_item_exp(context->objects, ARRSIZE_RESERVE, ARRSIZE_OBJECT, sizeof(graphics_object));
    return object;
}

int graphics_object_shader_param_get(graphics_object *object, const char *name, float *value, int *size)
{
    graphics_shader_param *param = 0;
    for(int i = 0;  i < ARRSIZE_SHADER_PARAM; i++) {
        graphics_shader_param *p = &(object->shader_params[i]);
        if(p->is_valid && strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
    }

    if(param) {
        memcpy(value, param->value, param->size * sizeof(float));
        *size = param->size;
        return 1;
    }
    else {
        return 0;
    }
}

void graphics_object_shader_param_set(graphics_object *object, const char *name, float *value, int size)
{
    graphics_shader_param *param = 0;
    for(int i = 0;  i < ARRSIZE_SHADER_PARAM; i++) {
        graphics_shader_param *p = &(object->shader_params[i]);
        if(p->is_valid && strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
    }

    if(!param) {
        param = (graphics_shader_param*)util_reserve_item_arr(object->shader_params, ARRSIZE_SHADER_PARAM, sizeof(graphics_shader_param));
        strcpy(param->name, name);
    }

    memcpy(param->value, value, size * sizeof(float));
    param->size = size;
}

void graphics_object_shader_param_delete(graphics_object *object, const char *name)
{
    graphics_shader_param *param = 0;
    for(int i = 0;  i < ARRSIZE_SHADER_PARAM; i++) {
        graphics_shader_param *p = &(object->shader_params[i]);
        if(p->is_valid && strcmp(p->name, name) == 0) {
            param = p;
            break;
        }
    }

    if(param) {
        if(param->value) {
            free(param->value);
        }
        memset(param, 0, sizeof(graphics_shader_param));
    }
}

void graphics_object_destroy(graphics_object *object)
{
    memset(object, 0, sizeof(graphics_object));
}

void graphics_refresh_draw_order(graphics_context *context)
{
    memset(context->object_draw_array, 0, sizeof(graphics_object*) * ARRSIZE_RESERVE * ARRSIZE_OBJECT);

    int idx = 0;
    for(int i = 0; i < ARRSIZE_RESERVE; i++) {
        if(!context->objects[i]) break;
        for(int ii = 0; ii < ARRSIZE_OBJECT; ii++) {
            graphics_object *object = &(context->objects[i][ii]);
            if(object->is_valid) {
                context->object_draw_array[idx] = object;
                idx++;
            }
        }
    }
}

void graphics_draw(graphics_context *context)
{
    // Draw objects
    for(int i = 0; i < ARRSIZE_RESERVE * ARRSIZE_OBJECT; i++) {
        graphics_object *object = context->object_draw_array[i];
        if(!object) break;
        if(!object->is_valid ||
                !object->shader_program || !object->shader_program->is_valid ||
                !object->texture || !object->texture->is_valid ||
                !object->vertex_array || !object->vertex_array->is_valid)
            continue;

#ifdef USE_OPENGL

        GLenum err;

        // Apply shader program
        glUseProgram(object->shader_program->handle);
        err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL program bind error: %i\n", err);
        }

        // Apply texture
        glBindTexture(GL_TEXTURE_2D, object->texture->handle);
        err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL texture bind error: %i\n", err);
        }

        // Apply global shader params
        for(int ii = 0; ii < ARRSIZE_RESERVE; ii++) {
            if(!context->shader_params[ii]) break;
            for(int ii2 = 0; ii2 < ARRSIZE_SHADER_PARAM; ii2++) {
                graphics_shader_param *param = &(context->shader_params[ii][ii2]);
                if(!param->is_valid) continue;

                int location = glGetUniformLocation(object->shader_program->handle, param->name);
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

                err = glGetError();
                if(err != GL_FALSE) {
                    fprintf(stderr, "OpenGL shader param error: %i\n", err);
                }
            }
        }

        // Apply object shader params
        for(int ii2 = 0; ii2 < ARRSIZE_SHADER_PARAM; ii2++) {
            graphics_shader_param *param = &(object->shader_params[ii2]);
            if(!param->is_valid) continue;

            int location = glGetUniformLocation(object->shader_program->handle, param->name);
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

            err = glGetError();
            if(err != GL_FALSE) {
                fprintf(stderr, "OpenGL shader param error: %i\n", err);
            }
        }

        // Bind vertex array
        glBindBuffer(GL_ARRAY_BUFFER, object->vertex_array->vbo_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->vertex_array->ebo_handle);
        err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL object error: %i\n", err);
        }
        int totalAttribSize = 0;
        for(int ii = 0; ii < ARRSIZE_VA_ATTRIBUTE; ii++) {
            graphics_vertex_array_attribute *attribute = &(object->vertex_array->attributes[ii]);
            if(!attribute->is_valid) continue;
            totalAttribSize += attribute->size;
        }
        int offset = 0;
        for(int ii = 0; ii < ARRSIZE_VA_ATTRIBUTE; ii++) {
            graphics_vertex_array_attribute *attribute = &(object->vertex_array->attributes[ii]);
            if(!attribute->is_valid) continue;
            glVertexAttribPointer(attribute->id, attribute->size, GL_FLOAT, GL_FALSE, totalAttribSize * sizeof(float), (GLvoid*)(offset * sizeof(float)));
            glEnableVertexAttribArray(attribute->id);
            offset += attribute->size;
        }
        err = glGetError();
        if(err != GL_FALSE) {
            fprintf(stderr, "OpenGL object error: %i\n", err);
        }

        // Draw
        glDrawElements(GL_TRIANGLES, object->vertex_array->element_buffer_size, GL_UNSIGNED_INT, 0);
#endif
    }

#ifdef USE_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
#endif
}

void graphics_clear(graphics_context *context, float *color)
{
#ifdef USE_OPENGL
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

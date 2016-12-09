#include "graphics.h"
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>

graphics_context *graphics_context_create()
{
    graphics_context *ctx = (graphics_context*)calloc(1, sizeof(graphics_context));
    return ctx;
}

void graphics_context_clear_objects(graphics_context *context)
{
    if(context->objects) {
        graphics_object *nxt = context->objects;
        while(nxt) {
            graphics_object *cur = nxt;
            nxt = nxt->next;
            graphics_object_destroy(cur);
        }
    }
}

void graphics_context_clear_textures(graphics_context *context)
{
    if(context->textures) {
        graphics_texture *nxt = context->textures;
        while(nxt) {
            graphics_texture *cur = nxt;
            nxt = nxt->next;
            graphics_texture_destroy(cur);
        }
    }
}

void graphics_context_clear_shader_params(graphics_context *context)
{
    if(context->shader_params) {
        graphics_shader_param *nxt = context->shader_params;
        while(nxt) {
            graphics_shader_param *cur = nxt;
            nxt = nxt->next;
            graphics_shader_param_destroy(cur);
        }
    }
}

void graphics_context_clear_shader_programs(graphics_context *context)
{
    if(context->shader_programs) {
        graphics_shader_program *nxt = context->shader_programs;
        while(nxt) {
            graphics_shader_program *cur = nxt;
            nxt = nxt->next;
            graphics_shader_program_destroy(cur);
        }
    }
}

void graphics_context_clear_shaders(graphics_context *context)
{
    if(context->shaders) {
        graphics_shader *nxt = context->shaders;
        while(nxt) {
            graphics_shader *cur = nxt;
            nxt = nxt->next;
            graphics_shader_destroy(cur);
        }
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
    shader->code = code;

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
    int code_size = strlen(code);
    glShaderSource(g_sh, 1, &shader->code, &code_size);
    glCompileShader(g_sh);

    // TODO error check
    shader->handle = g_sh;
#endif

    return shader;
}

void graphics_shader_destroy(graphics_shader *shader)
{
    if(shader->handle) {
        glDeleteShader(shader->handle);
    }

    if(shader->next) {
        shader->next->prev = shader->prev;
    }
    if(shader->prev) {
        shader->prev->next = shader->next;
    }

    free(shader);
}

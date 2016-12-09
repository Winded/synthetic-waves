/*
 * graphics.h
 *
 * Graphics module that supports multiple rendering APIs
 * */

#ifndef graphics_h
#define graphics_h

#include <stdlib.h>

typedef enum graphics_shader_type {
    graphics_vertex_shader,
    graphics_fragment_shader
} graphics_shader_type;

typedef struct graphics_shader {
        int handle;

        graphics_shader_type type;
        const char *code;

        struct graphics_shader *prev;
        struct graphics_shader *next;
} graphics_shader;

typedef struct graphics_shader_program {
        int handle;

        graphics_shader *vertex_shader;
        graphics_shader *fragment_shader;

        struct graphics_shader_program *prev;
        struct graphics_shader_program *next;
} graphics_shader_program;

typedef struct graphics_shader_param {
        char *name;
        float *value;
        int size;

        struct graphics_shader_param *prev;
        struct graphics_shader_param *next;
} graphics_shader_param;

typedef struct graphics_texture {
        int handle;

        void *data;
        int width;
        int height;

        struct graphics_texture *prev;
        struct graphics_texture *next;
} graphics_texture;

typedef struct graphics_object_attribute {
        int id, size;

        struct graphics_object_attribute *prev;
        struct graphics_object_attribute *next;
} graphics_object_attribute;

typedef struct graphics_object {
        int vao_handle, vbo_handle, ebo_handle;

        float *vertex_buffer;
        int vertex_buffer_size;

        int *element_buffer;
        int element_buffer_size;

        graphics_object_attribute *attributes;

        struct graphics_object *prev;
        struct graphics_object *next;
} graphics_object;

typedef struct graphics_context {
        graphics_shader *shaders;
        graphics_shader_program *shader_programs;
        graphics_shader_param *shader_params;

        graphics_texture *textures;

        graphics_object *objects;
} graphics_context;

graphics_context *graphics_context_create();

void graphics_context_clear_objects(graphics_context *context);
void graphics_context_clear_textures(graphics_context *context);
void graphics_context_clear_shader_params(graphics_context *context);
void graphics_context_clear_shader_programs(graphics_context *context);
void graphics_context_clear_shaders(graphics_context *context);

void graphics_context_destroy(graphics_context **context);

graphics_shader *graphics_shader_create(graphics_context *context, const char *code, graphics_shader_type type);
void graphics_shader_destroy(graphics_shader *shader);

#endif // graphics_h

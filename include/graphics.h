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
        char name[32];
        float value[32];
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

        graphics_shader_program *current_program;
        graphics_object *current_object;
} graphics_context;

graphics_context *graphics_context_create();

void graphics_context_clear_objects(graphics_context *context);
void graphics_context_clear_textures(graphics_context *context);
void graphics_context_clear_shader_params(graphics_context *context);
void graphics_context_clear_shader_programs(graphics_context *context);
void graphics_context_clear_shaders(graphics_context *context);

void graphics_context_destroy(graphics_context **context);

graphics_shader *graphics_shader_create(graphics_context *context, const char *code, graphics_shader_type type);
void graphics_shader_destroy(graphics_context *context, graphics_shader *shader);

graphics_shader_program *graphics_shader_program_create(graphics_context *context, graphics_shader *vertex_shader, graphics_shader *fragment_shader);
void graphics_shader_program_destroy(graphics_context *context, graphics_shader_program *program);

int graphics_shader_param_get(graphics_context *context, const char *name, float *value, int *size);
void graphics_shader_param_set(graphics_context *context, const char *name, float *value, int size);
void graphics_shader_param_delete(graphics_context *context, const char *name);

graphics_texture *graphics_texture_create(graphics_context *context, const void *data, int width, int height);
void graphics_texture_destroy(graphics_context *context, graphics_texture *texture);

graphics_object *graphics_object_create(graphics_context *context, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size);
void graphics_object_update(graphics_object *object, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size);
void graphics_object_set_attribute(graphics_object *object, int id, int size);
void graphics_object_destroy(graphics_context *context, graphics_object *object);

void graphics_use_program(graphics_context *context, graphics_shader_program *program);
void graphics_use_texture(graphics_context *context, graphics_texture *texture);
void graphics_use_shader_params(graphics_context *context);
void graphics_use_object(graphics_context *context, graphics_object *object);

void graphics_draw(graphics_context *context);

void graphics_clear(graphics_context *context, float *color);

#endif // graphics_h

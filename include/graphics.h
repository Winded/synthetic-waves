/*
 * graphics.h
 *
 * Graphics module that supports multiple rendering APIs
 * */

#ifndef graphics_h
#define graphics_h

#define ARRSIZE_SHADER 100
#define ARRSIZE_SHADER_PROGRAM 100
#define ARRSIZE_SHADER_PARAM 100
#define ARRSIZE_TEXTURE 100
#define ARRSIZE_VA 100
#define ARRSIZE_VA_ATTRIBUTE 20
#define ARRSIZE_OBJECT 100
#define ARRSIZE_RESERVE 20

#include <stdlib.h>

typedef enum graphics_shader_type {
    graphics_vertex_shader,
    graphics_fragment_shader
} graphics_shader_type;

typedef struct graphics_shader {
        char is_valid;

        int handle;

        graphics_shader_type type;
        const char *code;
} graphics_shader;

typedef struct graphics_shader_program {
        char is_valid;

        int handle;

        graphics_shader *vertex_shader;
        graphics_shader *fragment_shader;
} graphics_shader_program;

typedef struct graphics_shader_param {
        char is_valid;

        char name[32];
        float value[32];
        int size;
} graphics_shader_param;

typedef struct graphics_texture {
        char is_valid;

        int handle;

        void *data;
        int width;
        int height;
} graphics_texture;

typedef struct graphics_vertex_array_attribute {
        char is_valid;
        int id, size;
} graphics_vertex_array_attribute;

typedef struct graphics_vertex_array {
        char is_valid;

        int vao_handle, vbo_handle, ebo_handle;

        float *vertex_buffer;
        int vertex_buffer_size;

        int *element_buffer;
        int element_buffer_size;

        graphics_vertex_array_attribute attributes[ARRSIZE_VA_ATTRIBUTE];
} graphics_vertex_array;

typedef struct graphics_object {
        char is_valid;

        graphics_shader_program *shader_program;
        graphics_shader_param shader_params[ARRSIZE_SHADER_PARAM];
        graphics_texture *texture;
        graphics_vertex_array *vertex_array;
} graphics_object;

typedef struct graphics_context {
        graphics_shader *shaders[ARRSIZE_RESERVE];
        graphics_shader_program *shader_programs[ARRSIZE_RESERVE];
        graphics_shader_param *shader_params[ARRSIZE_RESERVE];

        graphics_texture *textures[ARRSIZE_RESERVE];

        graphics_vertex_array *vertex_arrays[ARRSIZE_RESERVE];

        graphics_object *objects[ARRSIZE_RESERVE];
        graphics_object *object_draw_array[ARRSIZE_RESERVE * ARRSIZE_OBJECT];
} graphics_context;

graphics_context *graphics_context_create();

void graphics_context_destroy(graphics_context **context);

graphics_shader *graphics_shader_create(graphics_context *context, const char *code, graphics_shader_type type);
void graphics_shader_destroy(graphics_shader *shader);

graphics_shader_program *graphics_shader_program_create(graphics_context *context, graphics_shader *vertex_shader, graphics_shader *fragment_shader);
void graphics_shader_program_destroy(graphics_shader_program *program);

int graphics_shader_param_get(graphics_context *context, const char *name, float *value, int *size);
void graphics_shader_param_set(graphics_context *context, const char *name, float *value, int size);
void graphics_shader_param_delete(graphics_context *context, const char *name);

graphics_texture *graphics_texture_create(graphics_context *context, const void *data, int width, int height);
void graphics_texture_destroy(graphics_texture *texture);

graphics_vertex_array *graphics_vertex_array_create(graphics_context *context, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size);
void graphics_vertex_array_update(graphics_vertex_array *vertex_array, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size);
void graphics_vertex_array_set_attribute(graphics_vertex_array *vertex_array, int id, int size);
void graphics_vertex_array_destroy(graphics_vertex_array *vertex_array);

graphics_object *graphics_object_create(graphics_context *context);
void graphics_object_shader_param_get(graphics_object *object, const char *name, float *value, int *size);
void graphics_object_shader_param_set(graphics_object *object, const char *name, float *value, int size);
void graphics_object_destroy(graphics_object *object);

void graphics_refresh_draw_order(graphics_context *context);

void graphics_draw(graphics_context *context);

void graphics_clear(graphics_context *context, float *color);

#endif // graphics_h

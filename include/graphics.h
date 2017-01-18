/*
 * graphics.h
 *
 * Graphics module that supports multiple rendering APIs
 * */

#ifndef graphics_h
#define graphics_h

#include <stdlib.h>

typedef enum graphics_version {
    graphics_opengl_3_2,
    // TODO add more support
} graphics_version;

typedef enum {
    graphics_feature_depth_test,
    graphics_feature_blend,
} graphics_feature;

typedef enum graphics_shader_type {
    graphics_vertex_shader,
    graphics_fragment_shader
} graphics_shader_type;

int graphics_initialized();
void graphics_init(graphics_version version);

int graphics_is_feature_enabled(graphics_feature feature);
void graphics_set_feature_enabled(graphics_feature feature, int enabled);

int graphics_shader_create(const char *code, graphics_shader_type type, char *errBuf);
void graphics_shader_destroy(int handle);

int graphics_shader_program_create(int vertex_shader_handle, int fragment_shader_handle, char *errBuf);
void graphics_shader_program_destroy(int handle);
void graphics_shader_program_use(int handle);

int graphics_shader_param_name_to_id(int shader_program_handle, const char *name);
int graphics_shader_param_set(int id, const float *value, int size, char *errBuf);

int graphics_texture_create(const void *data, int width, int height, int num_channels, char *errBuf);
int graphics_texture_update(int handle, const void *data, int width, int height, int num_channels, char *errBuf);
void graphics_texture_destroy(int handle);
void graphics_texture_use(int handle);

int graphics_vertex_array_create(float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size, char *errBuf);
int graphics_vertex_array_update(int handle, float *vertex_buffer, int vertex_buffer_size, int *element_buffer, int element_buffer_size, char *errBuf);
int graphics_vertex_array_set_attribute(int handle, int id, int size);
int graphics_vertex_array_delete_attribute(int handle, int id);
int graphics_vertex_array_destroy(int handle);
int graphics_vertex_array_use(int handle);

void graphics_draw();

void graphics_clear(const float *color);

#endif // graphics_h

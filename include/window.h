/*
 * window.h
 *
 * window handling module
 * */

#ifndef window_h
#define window_h

#include <SDL.h>
#include <stdbool.h>
#include <graphics.h>

#define WINDOW_MAX_CALLBACKS 20

typedef struct window_event_callback
{
    void(*callback)(struct window*, SDL_Event*, void*);
    void *data;
} window_event_callback;

typedef struct window
{
        char is_valid;

        SDL_Window *sdl_window;
        void *sdl_render_context;

        float clear_color[4];

        int should_close;

        Uint64 last_time;
        Uint64 time;

        window_event_callback event_callbacks[WINDOW_MAX_CALLBACKS];
} window;

typedef void(*window_event_cb_func)(window*, SDL_Event*, void*);

void window_init(window *w_handle, const char *title, int width, int height);

void window_open(window *w_handle);
void window_close(window *w_handle);

void window_get_clear_color(const window *w_handle, float *color);
void window_set_clear_color(window *w_handle, const float *color);

void window_get_position(window *w_handle, int *x, int *y);
void window_set_position(window *w_handle, int x, int y);

void window_get_size(window *w_handle, int *width, int *height);
void window_set_size(window *w_handle, int width, int height);

const char *window_get_title(window *w_handle);
void window_set_title(window *w_handle, const char *title);

float window_get_delta_time(window *w_handle);
float window_get_time(window *w_handle);

void window_swap_buffers(window *w_handle);

void window_poll_events(window *w_handle);

int window_should_close(window *w_handle);

void window_add_event_callback(window *w_handle, window_event_cb_func cb, void *data);
void window_remove_event_callback(window *w_handle, window_event_cb_func cb);

void window_destroy(window *w_handle);

#endif // window_h

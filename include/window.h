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
        graphics_context *g_context;

        float clear_color[4];

        int should_close;

        Uint64 last_time;
        Uint64 time;

        window_event_callback event_callbacks[WINDOW_MAX_CALLBACKS];
} window;

typedef void(*window_event_cb_func)(window*, SDL_Event*, void*);

void window_init(window *w_handle, const char *title, int width, int height);

void window_set_graphics_context(window *w_handle, const graphics_context *context);

void window_get_clear_color(const window *w_handle, float *color);
void window_set_clear_color(window *w_handle, const float *color);

float window_get_delta_time(window *w_handle);

void window_draw(window *w_handle);

void window_poll_events(window *w_handle);

int window_should_close(window *w_handle);

void window_add_event_callback(window *w_handle, window_event_cb_func cb, void *data);
void window_remove_event_callback(window *w_handle, window_event_cb_func cb);

void window_destroy(window *w_handle);

#endif // window_h

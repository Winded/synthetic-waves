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

typedef struct window
{
        SDL_Window *sdl_window;
        void *sdl_render_context;
        graphics_context *g_context;

        float clear_color[4];

        int should_close;

        Uint64 last_time;
        Uint64 time;
} window;

window *window_create(const char *title, int width, int height);

void window_set_graphics_context(window *w_handle, const graphics_context *context);

void window_get_clear_color(const window *w_handle, float *color);
void window_set_clear_color(window *w_handle, const float *color);

float window_get_delta_time(window *w_handle);

void window_draw(window *w_handle);

void window_poll_events(window *w_handle);

int window_should_close(window *w_handle);

void window_destroy(window **w_handle);

#endif // window_h

/*
 * window.h
 *
 * window handling module
 * */

#ifndef window_h
#define window_h

#include <SDL.h>
#include <stdbool.h>

typedef struct window
{
        SDL_Window *sdl_window;
        void *sdl_render_context;
        bool should_close;
} window;

window *window_create(const char *title, int width, int height);

void window_swap_buffers(window *w_handle);

void window_poll_events(window *w_handle);

bool window_should_close(window *w_handle);

void window_destroy(window **w_handle);

#endif // window_h

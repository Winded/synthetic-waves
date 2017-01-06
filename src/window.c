#include "window.h"
#include <stdio.h>
#include <memory.h>
#include <GL/glew.h>
#include <time.h>
#include <assert.h>
#include <string.h>

void window_init(window *w_handle, const char *title, int width, int height)
{
    memset(w_handle, 0, sizeof(window));

    Uint32 wFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
#ifdef USE_OPENGL
    wFlags |= SDL_WINDOW_OPENGL;
#endif

    SDL_Window *w = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, wFlags);
    if(!w) {
        fprintf(stderr, "window.c: Failed to create SDL window");
        return 0;
    }

#ifdef USE_OPENGL
    SDL_GLContext ctx = SDL_GL_CreateContext(w);

    // Set our OpenGL version.
    // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int v;
    if(SDL_GL_GetAttribute(GL_VERSION_3_2, &v) && v) {
        // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        // Turn on double buffering with a 24bit Z buffer.
        // You may need to change this to 16 or 32 for your system
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        w_handle->graphics_api = window_opengl_3_2;
    }
    // TODO more versions support
    if(!v) {
        fprintf(stderr, "Failed to create window; No supported graphics API\n");
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(w);
        return;
    }

    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, width, height);
#endif

    w_handle->is_valid = 1;
    w_handle->sdl_window = w;
    w_handle->sdl_render_context = ctx;
    w_handle->should_close = 0;
}

void window_open(window *w_handle)
{
    // Also resets delta time
    w_handle->time = SDL_GetPerformanceCounter();
    w_handle->last_time = w_handle->time;
    if(w_handle->is_valid && (SDL_GetWindowFlags(w_handle->sdl_window) & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN) {
        SDL_ShowWindow(w_handle->sdl_window);
    }
}

void window_close(window *w_handle)
{
    if(w_handle->is_valid && (SDL_GetWindowFlags(w_handle->sdl_window) & SDL_WINDOW_SHOWN) == SDL_WINDOW_SHOWN) {
        SDL_HideWindow(w_handle->sdl_window);
    }
}

void window_set_graphics_context(window *w_handle, const graphics_context *context)
{
    w_handle->g_context = context;
}

void window_get_clear_color(const window *w_handle, float *color)
{
    memcpy(color, w_handle->clear_color, sizeof(float) * 4);
}

void window_set_clear_color(window *w_handle, const float *color)
{
    memcpy(w_handle->clear_color, color, sizeof(float) * 4);
}

void window_get_position(window *w_handle, int *x, int *y)
{
    SDL_GetWindowPosition(w_handle->sdl_window, x, y);
}

void window_set_position(window *w_handle, int x, int y)
{
    SDL_SetWindowPosition(w_handle->sdl_window, x, y);
}

void window_get_size(window *w_handle, int *width, int *height)
{
    SDL_GetWindowSize(w_handle->sdl_window, width, height);
}

void window_set_size(window *w_handle, int width, int height)
{
    SDL_SetWindowSize(w_handle->sdl_window, width, height);
}

float window_get_delta_time(window *w_handle)
{
    return (float)((w_handle->time - w_handle->last_time) * 1000 / (double)SDL_GetPerformanceFrequency() * 0.001f);
}

float window_get_time(window *w_handle)
{
    return (float)(w_handle->time * 1000 / (double)SDL_GetPerformanceFrequency() * 0.001f);
}

void window_draw(window *w_handle)
{
    if(w_handle->g_context) {
        graphics_clear(w_handle->g_context, w_handle->clear_color);
        graphics_draw(w_handle->g_context);
    }

#ifdef USE_OPENGL
    SDL_GL_SwapWindow(w_handle->sdl_window);
#endif
}

void window_poll_events(window *w_handle)
{
    w_handle->last_time = w_handle->time;
    w_handle->time = SDL_GetPerformanceCounter();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            w_handle->should_close = 1;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width, height;
            SDL_GetWindowSize(w_handle->sdl_window, &width, &height);
#ifdef USE_OPENGL
            glViewport(0, 0, width, height);
#endif
        }

        // Run callbacks
        for(int i = 0; i < WINDOW_MAX_CALLBACKS; i++) {
            if(w_handle->event_callbacks[i].callback) {
                w_handle->event_callbacks[i].callback(w_handle, &event, w_handle->event_callbacks[i].data);
            }
        }
    }
}

int window_should_close(window *w_handle)
{
    return w_handle->should_close;
}

void window_add_event_callback(window *w_handle, window_event_cb_func cb, void *data)
{
    for(int i = 0; i < WINDOW_MAX_CALLBACKS; i++) {
        if(w_handle->event_callbacks[i].callback) continue;
        w_handle->event_callbacks[i].callback = cb;
        w_handle->event_callbacks[i].data = data;
        return;
    }
    assert("Callback array is full" && 0);
}

void window_remove_event_callback(window *w_handle, window_event_cb_func cb)
{
    for(int i = 0; i < WINDOW_MAX_CALLBACKS; i++) {
        if(w_handle->event_callbacks[i].callback == cb) {
            memset(&(w_handle->event_callbacks[i]), 0, sizeof(window_event_callback));
            return;
        }
    }
}

void window_destroy(window *w_handle)
{
    if(!w_handle->is_valid) return;

#ifdef USE_OPENGL
    SDL_GL_DeleteContext(w_handle->sdl_render_context);
#endif
    SDL_DestroyWindow(w_handle->sdl_window);

    memset(w_handle, 0, sizeof(window));
}

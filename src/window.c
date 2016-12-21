#include "window.h"
#include <stdio.h>
#include <GL/glew.h>
#include <time.h>

window *window_create(const char *title, int width, int height)
{
    Uint32 wFlags = SDL_WINDOW_RESIZABLE;
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

    // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, width, height);
#endif

    window *w_handle = (window*)calloc(1, sizeof(window));
    w_handle->sdl_window = w;
    w_handle->sdl_render_context = ctx;
    w_handle->should_close = false;
    return w_handle;
}

float window_get_delta_time(window *w_handle)
{
    return (float)((w_handle->time - w_handle->last_time) * 1000 / (double)SDL_GetPerformanceFrequency() * 0.001f);
}

void window_swap_buffers(window *w_handle)
{
#ifdef USE_OPENGL
    SDL_GL_SwapWindow(w_handle->sdl_window);
#endif
}

void window_destroy(window **w_handle)
{
    window *w = *w_handle;

#ifdef USE_OPENGL
    SDL_GL_DeleteContext(w->sdl_render_context);
#endif
    SDL_DestroyWindow(w->sdl_window);

    free(w);

    *w_handle = 0;
}

void window_poll_events(window *w_handle)
{
    w_handle->last_time = w_handle->time;
    w_handle->time = SDL_GetPerformanceCounter();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            w_handle->should_close = true;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int width, height;
            SDL_GetWindowSize(w_handle->sdl_window, &width, &height);
#ifdef USE_OPENGL
            glViewport(0, 0, width, height);
#endif
        }
    }
}

bool window_should_close(window *w_handle)
{
    return w_handle->should_close;
}

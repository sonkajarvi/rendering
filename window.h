/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdbool.h>
#include <stdint.h>

#include <X11/X.h>

#include <glad/glx.h>

#define INFO    "[INFO] "
#define ERROR   "[ERROR] "

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

char *read_file(const char *path);

/**
 * struct window - The window structure
 *
 * TODO: Separate the X11 and GLX stuff from the generic stuff.
 */
struct window {
    struct renderer *renderer;
    bool should_close;
    float delta_time;
    int fps;

    /* X11 */
    Display *display;
    Window root;
    Window window;
    uint64_t time_offset;

    /* GLX */
    GLXContext context;
    GLXFBConfig fbconfig;
};

int x11_window_create(struct window *window, int width, int height);
void x11_window_destroy(struct window *window);
void x11_window_show(struct window *window);
void x11_window_swap_buffers(struct window *window);
void x11_window_poll_events(struct window *window);

double x11_window_get_time(struct window *window);

/*
 * GLX context
 */
XVisualInfo *glx_get_visual(struct window *window);
int glx_create_context(struct window *window);
void glx_destroy_context(struct window *window);

/*
 * GL context
 */
GLADapiproc gl_get_proc_address(const char *name);
int gl_load_functions(struct window *window);
int gl_open_handle(void);
void gl_close_handle(void);

#endif /* _WINDOW_H */

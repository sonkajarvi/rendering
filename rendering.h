/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _RENDERING_H
#define _RENDERING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <X11/X.h>

#include <cglm/types.h>
#include <glad/gl.h>
#include <glad/glx.h>

#define INFO    "[info] "
#define ERROR   "[error] "

#define vec4_unwrap(v) {v[0], v[1], v[2], v[3]}

struct window;

struct vertex {
    vec3 position;
    vec4 color;
};

struct renderer {
    struct vertex *v_buf;       /* also used as a buffer for shader logs */
    struct vertex *v_ptr;

    /* OpenGL */
    GLuint program;
    GLuint vao;
    GLuint vbo;
};

struct window {
    struct renderer renderer;
    bool should_close;

    /* X11 */
    Display *display;
    Window root;
    Window window;

    /* GLX */
    GLXContext context;
    GLXFBConfig fbconfig;
};

int window_create(struct window *win, int width, int height);
void window_destroy(struct window *win);
void window_show(struct window *win);
bool window_should_close(struct window *win);
void window_poll_events(struct window *win);
void window_swap_buffers(struct window *win);

void window_set_color(struct window *win, float r, float g, float b);

void renderer_draw_quad(struct renderer *rend, vec3 position, vec3 scale, vec4 color);

/* X11 window */

int X11_window_create(struct window *w, int width, int height);
void X11_window_destroy(struct window *win);
void X11_window_show(struct window *win);
void X11_window_swap_buffers(struct window *win);
void X11_window_poll_events(struct window *win);

/* GLX context */

XVisualInfo *GLX_get_visual(struct window *win);
int GLX_create_context(struct window *win);
void GLX_destroy_context(struct window *win);

/* GL context */

int GL_open_handle(void);
void GL_close_handle(void);
GLADapiproc GL_get_proc_address(const char *name);
int GL_load_functions(struct window *win);

/* GL renderer */

int GL_renderer_create(struct renderer *rend);
void GL_renderer_destroy(struct renderer *rend);
void GL_renderer_clear_color(struct renderer *rend, float r, float g, float b);
void GL_renderer_new_frame(struct renderer *rend, struct window *win);
void GL_renderer_flush_vertices(struct renderer *rend);
void GL_renderer_push_vertices(struct renderer *rend, struct vertex *vertices, size_t count);

char *read_file(const char *path);

#endif /* _RENDERING_H */

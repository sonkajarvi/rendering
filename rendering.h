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
    /* OpenGL */
    struct vertex *v_buf;       /* Also used as the shader log buffer */
    struct vertex *v_ptr;
    GLuint program;
    GLuint vao;
    GLuint vbo;

    int (*create)(struct renderer *);
    void (*destroy)(struct renderer *);
    void (*clear_color)(struct renderer *, float, float, float);
    void (*new_frame)(struct renderer *, struct window *);
    void (*flush_vertices)(struct renderer *);
    void (*push_vertices)(struct renderer *, struct vertex *, size_t);
};

struct window {
    struct renderer rend;
    bool should_close;
    bool should_redraw;
    int width;
    int height;
    /* X11 */
    Display *display;
    Window root;
    Window window;
    /* GLX */
    GLXContext context;
    GLXFBConfig fbconfig;

    int (*create)(struct window *, int, int);
    void (*destroy)(struct window *);
    void (*show)(struct window *);
    void (*swap_buffers)(struct window *);
    void (*poll_events)(struct window *);
};

int window_create(struct window *w, int width, int height);
int window_exec(struct window *w);
void window_set_color(struct window *w, float r, float g, float b);

void renderer_draw_quad(struct renderer *rend, vec3 position, vec3 scale, vec4 color);

/* X11 window */

int X11_window_create(struct window *w, int width, int height);
void X11_window_destroy(struct window *w);
void X11_window_show(struct window *w);
void X11_window_swap_buffers(struct window *w);
void X11_window_poll_events(struct window *w);

/* GLX context */

XVisualInfo *GLX_get_visual(struct window *w);
int GLX_create_context(struct window *w);
void GLX_destroy_context(struct window *w);

/* GL context */

int GL_open_handle(void);
void GL_close_handle(void);
GLADapiproc GL_get_proc_address(const char *name);
int GL_load_functions(struct window *w);

/* GL renderer */

int GL_renderer_create(struct renderer *rend);
void GL_renderer_destroy(struct renderer *rend);
void GL_renderer_clear_color(struct renderer *rend, float r, float g, float b);
void GL_renderer_new_frame(struct renderer *rend, struct window *w);
void GL_renderer_flush_vertices(struct renderer *rend);
void GL_renderer_push_vertices(struct renderer *rend, struct vertex *vertices, size_t count);

char *read_file(const char *path);

#endif /* !_RENDERING_H */

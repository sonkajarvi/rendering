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
#include <cglm/vec2.h>
#include <cglm/vec4.h>
#include <glad/gl.h>
#include <glad/glx.h>

#define INFO    "[info] "
#define ERROR   "[error] "

#define vec2_unwrap(v) {v[0], v[1]}
#define vec3_unwrap(v) {v[0], v[1], v[2]}
#define vec4_unwrap(v) {v[0], v[1], v[2], v[3]}

#define ARRAYSIZE(x) (sizeof(x) / sizeof(*x))

#define COLOR_R(c) (((c) >> 24) & 255)
#define COLOR_G(c) (((c) >> 16) & 255)
#define COLOR_B(c) (((c) >>  8) & 255)
#define COLOR_A(c) (((c)      ) & 255)

#define COLOR_TO_VEC4(c)            \
    (vec4){                         \
        (float)COLOR_R(c) / 255.0f, \
        (float)COLOR_G(c) / 255.0f, \
        (float)COLOR_B(c) / 255.0f, \
        (float)COLOR_A(c) / 255.0f  \
    }

#define min(a, b)                \
    ({                           \
        __typeof__(a) __a = (a); \
        __typeof__(a) __b = (b); \
        __a < __b ? __a : __b;   \
    })

#define RECT_DEFAULT_INIT               \
    {                                   \
        .position = GLM_VEC2_ZERO_INIT, \
        .size = GLM_VEC2_ZERO_INIT,     \
        .radius = GLM_VEC4_ZERO_INIT    \
    }

#define STYLE_DEFAULT_INIT              \
    {                                   \
        .style = STYLE_FILL,            \
        .color = 0xffffffff,            \
        .tx_texture = NULL,             \
        .tx_coords = GLM_VEC4_ZERO_INIT \
    }

#define TEXTURE_FULL(tex)    \
    (vec4){                  \
        0.0f,                \
        0.0f,                \
        (float)(tex)->width, \
        (float)(tex)->height \
    }

#define STYLE_FILL 0
#define STYLE_STROKE 1
#define STYLE_TEXTURED_FILL 2

struct rect {
    vec2 position;
    vec2 size;
    vec4 radius;
};

struct style {
    int style;
    uint32_t color;

    /* stroke */
    float st_width;

    /* textured fill */
    struct texture *tx_texture;
    vec4 tx_coords;
};

struct vertex {
    vec2 position;
    vec4 color;
    vec2 center;
    vec2 size;
    vec4 radius;
    vec2 texture_coords;
    float texture_index;
    float stroke_width;
};

struct image {
    uint8_t *data;
    int width;
    int height;
};

struct texture {
    int width;
    int height;
    int index;

    /* OpenGL */
    GLuint id;
};

struct renderer {
    struct vertex *v_buf;       /* also used as a buffer for shader logs */
    struct vertex *v_ptr;

    struct texture *tex_array[4];
    int tex_count;
    struct texture white_texture;

    /* OpenGL */
    GLuint program;
    GLuint vao;
    GLuint vbo;
};

struct window {
    struct renderer renderer;
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

/* image */

int image_load_from_file(struct image *img, const char *path);
void image_free(struct image *img);

/* GL texture */

int GL_texture_create(struct texture *tex, struct image *img);
void GL_texture_destroy(struct texture *tex);

/* window */

int window_create(struct window *win, int width, int height);
void window_destroy(struct window *win);
void window_show(struct window *win);
bool window_should_close(struct window *win);
void window_poll_events(struct window *win);
void window_swap_buffers(struct window *win);

void window_set_color(struct window *win, float r, float g, float b);
double window_get_time(struct window *win);

/* renderer */

void renderer_draw_rect(struct renderer *rdr, struct rect *rect, struct style *style);

/* X11 window */

int X11_window_create(struct window *w, int width, int height);
void X11_window_destroy(struct window *win);
void X11_window_show(struct window *win);
void X11_window_swap_buffers(struct window *win);
void X11_window_poll_events(struct window *win);

double X11_window_get_time(struct window *win);

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
void GL_renderer_use_texture(struct renderer *rdr, struct texture *tex);

char *read_file(const char *path);

#endif /* _RENDERING_H */

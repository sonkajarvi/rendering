/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _RENDERER_H
#define _RENDERER_H

#include <stddef.h>

#include <cglm/types.h>
#include <cglm/vec2.h>
#include <cglm/vec4.h>
#include <glad/gl.h>

#include "image.h"
#include "window.h"

/**
 * struct vertex - Vertex structure
 */
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

#define TEXTURE_COORDS(texture)  \
    (vec4){                      \
        0.0f,                    \
        0.0f,                    \
        (float)(texture)->width, \
        (float)(texture)->height \
    }

/**
 * struct texture - Texture handle
 *
 * TODO: Separate the OpenGL stuff from the generic stuff.
 */
struct texture {
    GLuint id;
    int index;
    unsigned int width, height;
};

struct texture *gl_texture_create(struct image *image);
void gl_texture_destroy(struct texture *texture);

#define RECT_INIT_DEFAULT               \
    {                                   \
        .position = GLM_VEC2_ZERO_INIT, \
        .size = GLM_VEC2_ZERO_INIT,     \
        .radius = GLM_VEC4_ZERO_INIT    \
    }

/**
 * struct rect - Describes the physical attributes of drawn shapes
 */
struct rect {
    vec2 position;
    vec2 size;
    vec4 radius;
};

enum {
    STYLE_FILL,
    STYLE_STROKE,
    STYLE_TEXTURED_FILL,
};

#define STYLE_INIT_DEFAULT                   \
    {                                        \
        .type = STYLE_FILL,                  \
        .color = 0xffffffff,                 \
        .texture = NULL,                     \
        .texture_coords = GLM_VEC4_ZERO_INIT \
    }

/**
 * struct style - Describes the visual attributes of drawn shapes
 */
struct style {
    int type;
    unsigned int color;

    /* for strokes */
    float stroke_width;

    /* for textured fills */
    struct texture *texture;
    vec4 texture_coords;
};

/**
 * struct renderer - The base renderer structure
 */
struct renderer {
    unsigned int clear_color;

    void (*destroy)(struct renderer *renderer);
    void (*new_frame)(struct renderer *renderer, struct window *window);
    void (*push_vertices)(struct renderer *renderer,
                          struct vertex *vertices, size_t count);
    void (*flush_vertices)(struct renderer *renderer);
    struct texture *(*use_texture)(struct renderer *renderer,
                                   struct texture *texture);
};

#define renderer_priv(renderer) ((void *)(renderer) + sizeof(*(renderer)))

struct renderer *renderer_alloc(size_t size);
void renderer_destroy(struct renderer *renderer);

static inline void renderer_new_frame(struct renderer *renderer,
                                     struct window *window)
{
    if (renderer)
        renderer->new_frame(renderer, window);
}

static inline void renderer_push_vertices(struct renderer *renderer,
                                         struct vertex *vertices, size_t count)
{
    if (renderer)
        renderer->push_vertices(renderer, vertices, count);
}

static inline void renderer_flush_vertices(struct renderer *renderer)
{
    if (renderer)
        renderer->flush_vertices(renderer);
}

static inline struct texture *renderer_use_texture(struct renderer *renderer,
                                                   struct texture *texture)
{
    if (renderer)
        return renderer->use_texture(renderer, texture);

    return NULL;
}

void renderer_draw_rect(struct renderer *renderer,
                        struct rect *rect, struct style *style);

#endif /* !_RENDERER_H */

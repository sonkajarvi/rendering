/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "color.h"
#include "renderer.h"
#include "window.h"

/**
 * renderer_alloc() - Initialize a renderer struct, including private data
 * @size:       Size of private data, in bytes
 */
struct renderer *renderer_alloc(size_t size)
{
    struct renderer *renderer;

    /* renderer + private data */
    renderer = malloc(sizeof(*renderer) + size);
    if (!renderer)
        return NULL;

    memset(renderer, 0, sizeof(*renderer));

    return renderer;
}

void renderer_destroy(struct renderer *renderer)
{
    if (!renderer)
        return;

    renderer->destroy(renderer);
    free(renderer);
}

#define MIN(a, b)                   \
    ({                              \
        __typeof__(a) __a = (a);    \
        __typeof__(a) __b = (b);    \
        __a < __b ? __a : __b;      \
    })

#define COLOR_TO_VEC4(c)                \
    (vec4){                             \
        (float)COLOR_R(c) / 255.0f,     \
        (float)COLOR_G(c) / 255.0f,     \
        (float)COLOR_B(c) / 255.0f,     \
        (float)COLOR_A(c) / 255.0f      \
    }

#define COPY_REST_OF_FIELDS(i)                      \
    glm_vec4_copy(color, vertices[i].color);        \
    glm_vec2_copy(center, vertices[i].center);      \
    glm_vec2_copy(rect->size, vertices[i].size);    \
    glm_vec4_copy(radius, vertices[i].radius);      \
    vertices[i].texture_index = texture->index;     \
    vertices[i].stroke_width = width;               \
    vertices[i].rotation = rect->rotation;

void renderer_draw_rect(struct renderer *renderer, struct rect *rect,
                        struct style *style)
{
    struct vertex vertices[6];
    struct texture *texture;
    vec4 color, radius, coords;
    vec2 center, half_size;
    float smallest, width;

    if (!renderer || !rect || !style)
        return;

    if (COLOR_A(style->color) == 0)
        return;

    if (style->type == STYLE_STROKE && style->stroke_width == 0.0f)
        return;

    glm_vec2_copy((vec2){rect->size[0] / 2.0f, rect->size[1] / 2.0f},
                  half_size);
    glm_vec2_copy((vec2){rect->position[0] + half_size[0],
                  rect->position[1] + half_size[1]}, center);

    smallest = MIN(half_size[0], half_size[1]);
    radius[0] = MIN(smallest, rect->radius[0]);
    radius[1] = MIN(smallest, rect->radius[1]);
    radius[2] = MIN(smallest, rect->radius[2]);
    radius[3] = MIN(smallest, rect->radius[3]);

    /* transform color from uint32_t to vec4 */
    glm_vec4_copy(COLOR_TO_VEC4(style->color), color);

    texture = renderer_use_texture(renderer,
                                   style->type == STYLE_TEXTURED_FILL ?
                                   style->texture : NULL);
    if (!texture) {
        printf("renderer: invalid texture\n");
        return;
    }

    coords[0] = style->texture_coords[0] / texture->width;
    coords[1] = style->texture_coords[1] / texture->height;
    coords[2] = style->texture_coords[2] / texture->width + coords[0];
    coords[3] = style->texture_coords[3] / texture->height + coords[1];

    width = style->type == STYLE_STROKE ? style->stroke_width : 0.0f;

    /*  top left (0, 0)
     *      |      top right (width, 0)
     *      v         v
     *      +---------+
     *      |         |
     *      |         |
     *      |         |
     *      +---------+
     *      ^         ^
     *      |   bottom right (width, height)
     *  bottom left (0, height)
     */

    /* top right */
    glm_vec2_copy((vec2){rect->position[0] + rect->size[0] + width,
                         rect->position[1] + width},
                  vertices[0].position);
    glm_vec2_copy((vec2){coords[2], coords[1]}, vertices[0].texture_coords);
    COPY_REST_OF_FIELDS(0);

    /* bottom right */
    glm_vec2_copy((vec2){rect->position[0] + rect->size[0] + width,
                         rect->position[1] - rect->size[1] - width},
                  vertices[1].position);
    glm_vec2_copy((vec2){coords[2], coords[3]}, vertices[1].texture_coords);
    COPY_REST_OF_FIELDS(1);

    /* top left */
    glm_vec2_copy((vec2){rect->position[0] - width,
                         rect->position[1] + width},
                  vertices[2].position);
    glm_vec2_copy((vec2){coords[0], coords[1]}, vertices[2].texture_coords);
    COPY_REST_OF_FIELDS(2);

    /* bottom right */
    glm_vec2_copy((vec2){rect->position[0] + rect->size[0] + width,
                         rect->position[1] - rect->size[1] - width},
                  vertices[3].position);
    glm_vec2_copy((vec2){coords[2], coords[3]}, vertices[3].texture_coords);
    COPY_REST_OF_FIELDS(3);

    /* bottom left */
    glm_vec2_copy((vec2){rect->position[0] - width,
                         rect->position[1] - rect->size[1] - width},
                  vertices[4].position);
    glm_vec2_copy((vec2){coords[0], coords[3]}, vertices[4].texture_coords);
    COPY_REST_OF_FIELDS(4);

    /* top left */
    glm_vec2_copy((vec2){rect->position[0] - width,
                         rect->position[1] + width},
                  vertices[5].position);
    glm_vec2_copy((vec2){coords[0], coords[1]}, vertices[5].texture_coords);
    COPY_REST_OF_FIELDS(5);

    renderer_push_vertices(renderer, vertices, ARRAY_SIZE(vertices));
}

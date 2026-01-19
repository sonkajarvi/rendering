/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "rendering.h"

#define _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(i) \
    glm_vec4_copy(c, v[i].color);                       \
    glm_vec2_copy(rect->position, v[i].center);         \
    glm_vec2_copy(rect->size, v[i].size);               \
    glm_vec4_copy(r, v[i].radius);                      \
    v[i].texture_index = tmp->index;                    \
    v[i].stroke_width = s;

void renderer_draw_rect(struct renderer *rdr, struct rect *rect, struct style *style)
{
    struct vertex v[6];
    struct texture *tmp;
    vec4 c, r, t;       /* color, bounded radii, normalized texture coords */
    vec2 h, p;          /* halved size, top-left */
    float m, s;         /* smallest side length, stroke width */

    if (COLOR_A(style->color) == 0)
        return;
    if (style->style == STYLE_STROKE && style->st_width == 0.0f)
        return;

    glm_vec2_copy((vec2){rect->size[0] / 2.0f, rect->size[1] / 2.0f}, h);
    glm_vec2_copy((vec2){rect->position[0] - h[0], rect->position[1] + h[1]}, p);

    m = min(h[0], h[1]);
    r[0] = min(m, rect->radius[0]);
    r[1] = min(m, rect->radius[1]);
    r[2] = min(m, rect->radius[2]);
    r[3] = min(m, rect->radius[3]);

    /* transform color from uint32_t to vec4 */
    glm_vec4_copy(COLOR_TO_VEC4(style->color), c);

    tmp = style->style == STYLE_TEXTURED_FILL ? style->tx_texture : &rdr->white_texture;
    GL_renderer_use_texture(rdr, tmp);

    t[0] = style->tx_coords[0] / tmp->width;
    t[1] = style->tx_coords[1] / tmp->height;
    t[2] = style->tx_coords[2] / tmp->width + t[0];
    t[3] = style->tx_coords[3] / tmp->height + t[1];

    s = style->style == STYLE_STROKE ? style->st_width : 0.0f;

    /* top right */
    glm_vec2_copy((vec2){p[0] + rect->size[0] + s, p[1] + s}, v[0].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(0);
    glm_vec2_copy((vec2){t[2], t[3]}, v[0].texture_coords);

    /* bottom right */
    glm_vec2_copy((vec2){p[0] + rect->size[0] + s, p[1] - rect->size[1] - s}, v[1].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(1);
    glm_vec2_copy((vec2){t[2], t[1]}, v[1].texture_coords);

    /* top left */
    glm_vec2_copy((vec2){p[0] - s, p[1] + s}, v[2].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(2);
    glm_vec2_copy((vec2){t[0], t[3]}, v[2].texture_coords);

    /* bottom right */
    glm_vec2_copy((vec2){p[0] + rect->size[0] + s, p[1] - rect->size[1] - s}, v[3].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(3);
    glm_vec2_copy((vec2){t[2], t[1]}, v[3].texture_coords);

    /* bottom left */
    glm_vec2_copy((vec2){p[0] - s, p[1] - rect->size[1] - s}, v[4].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(4);
    glm_vec2_copy((vec2){t[0], t[1]}, v[4].texture_coords);

    /* top left */
    glm_vec2_copy((vec2){p[0] - s, p[1] + s}, v[5].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_TEXTURE_WIDTH(5);
    glm_vec2_copy((vec2){t[0], t[3]}, v[5].texture_coords);

    GL_renderer_push_vertices(rdr, v, ARRAYSIZE(v));
}

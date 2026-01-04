/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/affine.h>
#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "rendering.h"

void renderer_draw_rect(struct renderer *rdr, vec2 position, vec2 size, vec4 color)
{
    /* TODO: make this const */
    static vec4 r = GLM_VEC4_ZERO_INIT;

    renderer_draw_roundrect(rdr, position, size, r, color);
}

void renderer_draw_circle(struct renderer *rdr, vec2 position, float radius, vec4 color)
{
    vec2 s = {radius * 2.0f, radius * 2.0f};
    vec4 r = {radius, radius, radius, radius};

    renderer_draw_roundrect(rdr, position, s, r, color);
}

void renderer_draw_roundrect(struct renderer *rdr, vec2 position, vec2 size, vec4 radius, vec4 color)
{
    renderer_draw_textured_roundrect(rdr, position, size, radius,
        &rdr->white_texture, (vec4){0.0f, 0.0f, 1.0f, 1.0f}, color);
}

#define _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(i) \
    glm_vec4_copy(color, v[i].color);           \
    glm_vec2_copy(position, v[i].center);       \
    glm_vec2_copy(size, v[i].size);             \
    glm_vec4_copy(r, v[i].radius);              \
    v[i].texture_index = tex->index;

void renderer_draw_textured_roundrect(struct renderer *rdr,
    vec2 position, vec2 size, vec4 radius,
    struct texture *tex, vec4 tex_coords, vec4 color)
{
    struct vertex v[6];
    vec4 r, t;
    vec2 h, p, o;
    float m;

    glm_vec2_copy((vec2){size[0] / 2.0f, size[1] / 2.0f}, h);
    glm_vec2_copy((vec2){position[0] - h[0], position[1] + h[1]}, p);

    m = min(h[0], h[1]);
    r[0] = min(m, radius[0]);
    r[1] = min(m, radius[1]);
    r[2] = min(m, radius[2]);
    r[3] = min(m, radius[3]);

    o[0] = 0.125f / tex->width;
    o[1] = 0.125f / tex->height;
    t[0] = tex_coords[0] / tex->width + o[0];
    t[1] = tex_coords[1] / tex->height + o[1];
    t[2] = tex_coords[2] / tex->width + t[0] - o[0];
    t[3] = tex_coords[3] / tex->height + t[1] - o[1];

    GL_renderer_use_texture(rdr, tex);

    /* top right */
    glm_vec2_copy((vec2){p[0] + size[0], p[1]}, v[0].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(0);
    glm_vec2_copy((vec2){t[2], t[3]}, v[0].texture_coords);

    /* bottom right */
    glm_vec2_copy((vec2){p[0] + size[0], p[1] - size[1]}, v[1].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(1);
    glm_vec2_copy((vec2){t[2], t[1]}, v[1].texture_coords);

    /* top left */
    glm_vec2_copy((vec2){p[0], p[1]}, v[2].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(2);
    glm_vec2_copy((vec2){t[0], t[3]}, v[2].texture_coords);

    /* bottom right */
    glm_vec2_copy((vec2){p[0] + size[0], p[1] - size[1]}, v[3].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(3);
    glm_vec2_copy((vec2){t[2], t[1]}, v[3].texture_coords);

    /* bottom left */
    glm_vec2_copy((vec2){p[0], p[1] - size[1]}, v[4].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(4);
    glm_vec2_copy((vec2){t[0], t[1]}, v[4].texture_coords);

    /* top left */
    glm_vec2_copy((vec2){p[0], p[1]}, v[5].position);
    _COPY_COLOR_CENTER_SIZE_RADIUS_INDEX(5);
    glm_vec2_copy((vec2){t[0], t[3]}, v[5].texture_coords);

    GL_renderer_push_vertices(rdr, v, ARRAYSIZE(v));
}

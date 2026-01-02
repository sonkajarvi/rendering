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
    vec2 h = {size[0] / 2.0f, size[1] / 2.0f};
    vec2 p = {position[0] - h[0], position[1] + h[1]};
    vec4 r;

    float m = min(h[0], h[1]);
    r[0] = min(m, radius[0]);
    r[1] = min(m, radius[1]);
    r[2] = min(m, radius[2]);
    r[3] = min(m, radius[3]);

    struct vertex v[] = {
        {{p[0] + size[0], p[1],         }, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)},   /* top right */
        {{p[0] + size[0], p[1] - size[1]}, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)},   /* bottom right */
        {{p[0],           p[1],         }, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)},   /* top left */
        {{p[0] + size[0], p[1] - size[1]}, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)},   /* bottom right */
        {{p[0],           p[1] - size[1]}, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)},   /* bottom left */
        {{p[0],           p[1],         }, vec4_unwrap(color), vec2_unwrap(position), vec2_unwrap(size), vec4_unwrap(r)}    /* top left */
    };

    GL_renderer_push_vertices(rdr, v, ARRAYSIZE(v));
}

/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/affine.h>

#include "rendering.h"

void renderer_draw_circle(struct renderer *r, vec2 position, float radius, vec4 color)
{
    vec2 p = {position[0] - radius, position[1] + radius};
    vec2 c = {position[0], position[1]};
    float r2 = 2.0f * radius;

    struct vertex vertices[] = {
        {{p[0] + r2, p[1],    }, vec4_unwrap(color), vec2_unwrap(c), radius},   /* top right */
        {{p[0] + r2, p[1] - r2}, vec4_unwrap(color), vec2_unwrap(c), radius},   /* bottom right */
        {{p[0],      p[1],    }, vec4_unwrap(color), vec2_unwrap(c), radius},   /* top left */
        {{p[0] + r2, p[1] - r2}, vec4_unwrap(color), vec2_unwrap(c), radius},   /* bottom right */
        {{p[0],      p[1] - r2}, vec4_unwrap(color), vec2_unwrap(c), radius},   /* bottom left */
        {{p[0],      p[1],    }, vec4_unwrap(color), vec2_unwrap(c), radius}    /* top left */
    };

    GL_renderer_push_vertices(r, vertices, 6);
}

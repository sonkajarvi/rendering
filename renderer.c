/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/affine.h>

#include "rendering.h"

void renderer_draw_quad(struct renderer *rend, vec3 position, vec3 scale, vec4 color)
{
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    struct vertex vertices[] = {
        { {  1.0f,  1.0f, 0.0f }, vec4_unwrap(color) },     /* top right */
        { {  1.0f, -1.0f, 0.0f }, vec4_unwrap(color) },     /* bottom right */
        { { -1.0f,  1.0f, 0.0f }, vec4_unwrap(color) },     /* top left */
        { {  1.0f, -1.0f, 0.0f }, vec4_unwrap(color) },     /* bottom right */
        { { -1.0f, -1.0f, 0.0f }, vec4_unwrap(color) },     /* bottom left */
        { { -1.0f,  1.0f, 0.0f }, vec4_unwrap(color) }      /* top left */
    };

    glm_translate(model, position);
    glm_vec3_scale(scale, 0.5f, scale);
    glm_scale(model, scale);

    /* Apply model to vertices */
    for (int i = 0; i < 6; i++)
        glm_mat4_mulv3(model, vertices[i].position, 1.0f, vertices[i].position);

    GL_renderer_push_vertices(rend, vertices, 6);
}

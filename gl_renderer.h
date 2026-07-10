/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _GL_RENDERER_H
#define _GL_RENDERER_H

#include <glad/gl.h>

#include "renderer.h"

#define GL_RENDERER_TEXTURE_COUNT 8

struct gl_renderer {
    struct renderer renderer;

    /* @vert_buf is also used as the log buffer during shader compilation. */
    struct vertex *vert_buf;
    struct vertex *vert_ptr;

    struct texture *text_white;
    struct texture *text_array[GL_RENDERER_TEXTURE_COUNT];
    int text_used;

    GLuint program;
    GLuint vao;
    GLuint vbo;
};

struct renderer *gl_renderer_create(void);

#endif /* !_GL_RENDERER_H */

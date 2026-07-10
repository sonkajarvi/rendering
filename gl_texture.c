/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>

#include "image.h"
#include "renderer.h"

struct texture *gl_texture_create(struct image *image)
{
    struct texture *texture;

    texture = malloc(sizeof(*texture));
    if (!texture)
        return NULL;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    texture->width = image->width;
    texture->height = image->height;
    texture->index = -1;

    printf(INFO "GL TEXTURE: created texture: id %u\n", texture->id);

    return texture;
}

void gl_texture_destroy(struct texture *texture)
{
    GLuint id = texture->id;

    glDeleteTextures(1, &texture->id);

    free(texture);

    printf(INFO "GL TEXTURE: destroyed texture: id %u\n", id);
}

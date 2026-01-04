/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdbool.h>
#include <stdio.h>

#include <glad/gl.h>

#include "rendering.h"

int GL_texture_create(struct texture *tex, struct image *img)
{
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    tex->width = img->width;
    tex->height = img->height;
    tex->index = -1;

    printf(INFO "GL: created texture, id %u\n", tex->id);

    return 0;
}

void GL_texture_destroy(struct texture *tex)
{
    glDeleteTextures(1, &tex->id);
}

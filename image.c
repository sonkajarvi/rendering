/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_FAILURE_STRINGS
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb/stb_image.h>

#include "rendering.h"

int image_load_from_file(struct image *img, const char *path)
{
    int comp;

    stbi_set_flip_vertically_on_load(1);

    img->data = stbi_load(path, &img->width, &img->height, &comp, 4);
    if (!img->data) {
        printf(ERROR "image: failed to open file: %s\n", path);
        return -1;
    }

    printf(INFO "image: loaded image, width %d, height %d\n", img->width, img->height);
    return 0;
}

void image_free(struct image *img)
{
    stbi_image_free(img->data);
}

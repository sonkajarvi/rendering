/**
 * Copyright (c) 2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#ifndef _IMAGE_H
#define _IMAGE_H

/**
 * struct image - Image structure
 */
struct image {
    int width, height;
    unsigned char *data;
};

struct image *image_load_from_file(const char *path);
void image_free(struct image *image);

#endif /* _IMAGE_H */

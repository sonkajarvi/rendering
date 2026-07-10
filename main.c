/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "image.h"
#include "window.h"

int main(void)
{
    struct window window;
    struct image *image;

    if (x11_window_create(&window, 1000, 800) != 0)
        return -1;

    x11_window_show(&window);

    image = image_load_from_file(RESOURCES_PATH "/obama.jpg");
    if (!image)
        return -1;

    image_free(image);

    while (!window.should_close) {
        x11_window_poll_events(&window);
        x11_window_swap_buffers(&window);
    }

    x11_window_destroy(&window);

    return 0;
}

/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "gl_renderer.h"
#include "image.h"
#include "renderer.h"
#include "window.h"

#define RENDERER_DRAW_RECT()                                \
    for (int i = 0; i < 5; i++) {                           \
        rect.position[0] = 50 + i * 200;                    \
        rect.radius[0] = 60 - i * 15;                       \
        rect.radius[1] = 60 - i * 15 + 4;                   \
        rect.radius[2] = 60 - i * 15 + 8;                   \
        rect.radius[3] = 60 - i * 15 + 12;                  \
        renderer_draw_rect(window.renderer, &rect, &style); \
    }

int main(void)
{
    struct window window;
    struct texture *texture;
    struct image *image;
    struct rect rect;
    struct style style;

    if (x11_window_create(&window, 1000, 800) != 0)
        return -1;

    window.renderer = gl_renderer_create();
    if (!window.renderer)
        return -1;

    window.renderer->clear_color = 0x111111ff;
    x11_window_show(&window);

    image = image_load_from_file(RESOURCES_PATH "/obama.jpg");
    if (!image)
        return -1;

    texture = gl_texture_create(image);
    if (!texture)
        return -1;

    image_free(image);

    while (!window.should_close) {
        renderer_new_frame(window.renderer, &window);

        rect = (struct rect)RECT_INIT_DEFAULT;
        style = (struct style)STYLE_INIT_DEFAULT;
        glm_vec2_copy((vec2){100, 100}, rect.size);

        rect.position[1] = 100;
        style.type = STYLE_STROKE;
        style.color = 0xff0000ff;
        style.stroke_width = 10;
        RENDERER_DRAW_RECT();

        rect.position[1] += 200;
        style.type = STYLE_FILL;
        style.color = 0x0000ffff;
        RENDERER_DRAW_RECT();

        rect.position[1] += 200;
        style.type = STYLE_TEXTURED_FILL;
        style.color = 0x00ff00ff;
        style.texture = texture;
        glm_vec4_copy(TEXTURE_COORDS(texture), style.texture_coords);
        RENDERER_DRAW_RECT();

        renderer_flush_vertices(window.renderer);

        x11_window_poll_events(&window);
        x11_window_swap_buffers(&window);
    }

    gl_texture_destroy(texture);
    x11_window_destroy(&window);

    return 0;
}

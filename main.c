#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "rendering.h"

#define RENDERER_DRAW_RECT                                \
    for (int i = 0; i < 5; i++) {                         \
        rect.position[0] = i * 200 - 400;                 \
        rect.radius[0] = 60 - i * 15;                     \
        rect.radius[1] = 60 - i * 15 + 4;                 \
        rect.radius[2] = 60 - i * 15 + 8;                 \
        rect.radius[3] = 60 - i * 15 + 12;                \
        renderer_draw_rect(&win.renderer, &rect, &style); \
    }

int main(void)
{
    struct window win;
    struct image img;
    struct texture tex;
    struct rect rect;
    struct style style;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    // window_set_color(&win, 1.0f, 1.0f, 1.0f);
    window_show(&win);

    image_load_from_file(&img, RESOURCES_PATH "/obama.jpg");
    GL_texture_create(&tex, &img);
    image_free(&img);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        rect = (struct rect)RECT_DEFAULT_INIT;
        style = (struct style)STYLE_DEFAULT_INIT;
        glm_vec2_copy((vec2){100, 100}, rect.size);

        rect.position[1] = 200;
        style.style = STYLE_STROKE;
        style.color = 0xff0000ff;
        style.st_width = 10;
        RENDERER_DRAW_RECT;

        rect.position[1] = 0;
        style.style = STYLE_FILL;
        style.color = 0x00ff00ff;
        RENDERER_DRAW_RECT;

        rect.position[1] = -200;
        style.style = STYLE_TEXTURED_FILL;
        style.color = 0x0000ffff;
        style.tx_texture = &tex;
        glm_vec4_copy(TEXTURE_FULL(&tex), style.tx_coords);
        RENDERER_DRAW_RECT;

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    GL_texture_destroy(&tex);
    window_destroy(&win);
    return 0;
}

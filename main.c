// #include <math.h>
// #include <stdio.h>
#include <stdlib.h>

#include "rendering.h"

#define randf() ((float)rand() / (float)RAND_MAX)

int main(void)
{
    struct window win;
    struct image img;
    struct texture tex;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    // window_set_color(&win, 0.0f, 0.0f, 0.0f);
    window_show(&win);

    image_load_from_file(&img, RESOURCES_PATH "/obama.jpg");
    GL_texture_create(&tex, &img);
    image_free(&img);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        // printf("%.1f\n", 1.0f / win.delta_time);
        srand(0);

        // renderer_draw_rect(
        //     &win.renderer,
        //     (vec2){-400.0f, 0.0f},
        //     (vec2){300.0f, 300.0f},
        //     (vec4){randf(), randf(), randf(), 1.0f}
        // );

        // renderer_draw_circle(
        //     &win.renderer,
        //     (vec2){0.0f, 0.0f},
        //     155.0f,
        //     (vec4){randf(), randf(), randf(), 1.0f}
        // );

        renderer_draw_roundrect(
            &win.renderer,
            (vec2){-200.0f, 0.0f},
            (vec2){300.0f, 300.0f},
            (vec4){50.0f, 50.0f, 50.0f, 50.0f},
            (vec4){randf(), randf(), randf(), 1.0f}
        );

        renderer_draw_textured_roundrect(
            &win.renderer,
            (vec2){200.0f, 0.0f},
            (vec2){300.0f, 300.0f},
            (vec4){50.0f, 50.0f, 50.0f, 50.0f},
            &tex,
            (vec4){0.0f, 0.0f, 960.0f, 1290.0f},
            (vec4){randf(), randf(), randf(), 1.0f}
        );

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    GL_texture_destroy(&tex);
    window_destroy(&win);
    return 0;
}

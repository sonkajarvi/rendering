// #include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "rendering.h"

#define randf() ((float)rand() / (float)RAND_MAX)

int main(void)
{
    struct window win;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    // window_set_color(&win, 0.0f, 0.0f, 0.0f);
    window_show(&win);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        printf("%.1f\n", 1.0f / win.delta_time);
        srand(0);

        renderer_draw_rect(
            &win.renderer,
            (vec2){-300.0f, 0.0f},
            (vec2){200.0f, 200.0f},
            (vec4){randf(), randf(), randf(), 1.0f}
        );

        renderer_draw_circle(
            &win.renderer,
            (vec2){0.0f, 0.0f},
            105.0f,
            (vec4){randf(), randf(), randf(), 1.0f}
        );

        renderer_draw_roundrect(
            &win.renderer,
            (vec2){300.0f, 0.0f},
            (vec2){200.0f, 200.0f},
            (vec4){50.0f, 50.0f, 50.0f, 50.0f},
            (vec4){randf(), randf(), randf(), 1.0f}
        );

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    window_destroy(&win);
    return 0;
}

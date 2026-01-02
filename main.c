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

    // window_set_color(&win, 0.1f, 0.2f, 0.3f);
    window_set_color(&win, 0.0f, 0.0f, 0.0f);
    window_show(&win);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        printf("%.1f\n", 1.0f / win.delta_time);

        srand(0);

        for (int i = 0; i < 10000; i++) {
            renderer_draw_roundrect(
                &win.renderer,
                (vec3){randf() * 1200.0f - 600.0f, randf() * 800.0f - 400.0f},
                (vec2){80.0f, 80.0f},
                (vec4){20.0f, 20.0f, 20.0f, 20.0f},
                (vec4){randf(), randf(), randf(), 1.0f}
            );
        }

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    window_destroy(&win);
    return 0;
}

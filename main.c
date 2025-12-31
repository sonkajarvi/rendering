#include <stdlib.h>

#include "rendering.h"

#define randf() ((float)rand() / (float)RAND_MAX)

int main(void)
{
    struct window win;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    window_show(&win);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        srand(0);

        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                renderer_draw_circle(
                    &win.renderer,
                    (vec3){x * 70 - 300, y * 70 - 300, 0.0f},
                    randf() * 30.0f + 10.0f,
                    (vec4){randf(), randf(), randf(), 1.0f}
                );
            }
        }

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    window_destroy(&win);
    return 0;
}

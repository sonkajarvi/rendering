#include "rendering.h"

int main(void)
{
    struct window win;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    window_show(&win);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);
        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    window_destroy(&win);
    return 0;
}

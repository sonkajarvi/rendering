#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

#include "rendering.h"

int window_create(struct window *w, int width, int height)
{
    if (!w)
        return -1;

    w->create = X11_window_create;
    w->destroy = X11_window_destroy;
    w->show = X11_window_show;
    w->swap_buffers = X11_window_swap_buffers;
    w->poll_events = X11_window_poll_events;

    w->rend.create = GL_renderer_create;
    w->rend.destroy = GL_renderer_destroy;
    w->rend.clear_color = GL_renderer_clear_color;
    w->rend.new_frame = GL_renderer_new_frame;
    w->rend.flush_vertices = GL_renderer_flush_vertices;
    w->rend.push_vertices = GL_renderer_push_vertices;

    if (GL_open_handle() != 0)
        return -1;
    if (w->create(w, width, height) != 0)
        return -1;

    if (GL_load_functions(w) != 0)
        return -1;
    if (w->rend.create(&w->rend) != 0)
        return -1;

    w->should_close = true;

    printf(INFO "Created new window: width %d, height %d\n", width, height);
    return 0;
}

int window_exec(struct window *win)
{
    win->show(win);

    win->should_redraw = true;

    while (!win->should_close) {
        win->poll_events(win);

        if (win->should_redraw) {
            win->rend.new_frame(&win->rend, win);

            win->rend.flush_vertices(&win->rend);
            win->swap_buffers(win);

            win->should_redraw = false;
        }
    }

    return 0;
}

void window_destroy(struct window *w)
{
    if (!w)
        return;

    w->destroy(w);
    GL_close_handle();
}

void window_set_color(struct window *w, float r, float g, float b)
{
    w->rend.clear_color(&w->rend, r, g, b);
}

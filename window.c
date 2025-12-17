/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

#include "rendering.h"

int window_create(struct window *win, int width, int height)
{
    if (!win)
        return -1;

    if (GL_open_handle() != 0)
        return -1;
    if (X11_window_create(win, width, height) != 0)
        return -1;

    if (GL_load_functions(win) != 0)
        return -1;
    if (GL_renderer_create(&win->renderer) != 0)
        return -1;

    win->should_close = true;

    printf(INFO "Created new window: width %d, height %d\n", width, height);
    return 0;
}

void window_destroy(struct window *win)
{
    if (!win)
        return;

    X11_window_destroy(win);
    GL_close_handle();
}

void window_show(struct window *win)
{
    if (win)
        X11_window_show(win);
}

bool window_should_close(struct window *win)
{
    if (win)
        return win->should_close;

    return true;
}

void window_poll_events(struct window *win)
{
    if (win)
        X11_window_poll_events(win);
}

void window_swap_buffers(struct window *win)
{
    if (win)
        X11_window_swap_buffers(win);
}

void window_set_color(struct window *win, float r, float g, float b)
{
    if (win)
        GL_renderer_clear_color(&win->renderer, r, g, b);
}

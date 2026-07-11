/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <X11/Xlib.h>

#include "window.h"

static Atom wm_delete_window;

/* time is in nanoseconds */
static uint64_t get_time(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec;
}

/* time is in seconds */
double x11_window_get_time(struct window *window)
{
    if (window)
        return (double)(get_time() - window->time_offset) / 1000000000.0;
    else
        return 0.0;
}

static int __x11_window_create(struct window *window, int width, int height)
{
    XVisualInfo *vi;
    XSetWindowAttributes swa;

    if (!window)
        return -1;

    window->display = XOpenDisplay(NULL);
    if (!window->display) {
        printf(INFO "X11 WINDOW: failed to open display\n");
        return -1;
    }

    vi = glx_get_visual(window);
    if (!vi) {
        printf(ERROR "X11 WINDOW: failed to get VisualInfo from FB config\n");
        XCloseDisplay(window->display);
    }

    window->root = DefaultRootWindow(window->display);

    memset(&swa, 0, sizeof(swa));
    swa.colormap = XCreateColormap(window->display,
                                   window->root, vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    window->window = XCreateWindow(
        window->display,
        window->root,
        0,
        0,
        width,
        height,
        0,
        vi->depth,
        InputOutput,
        vi->visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &swa
    );
    XFree(vi);

    if (glx_create_context(window) != 0) {
        XDestroyWindow(window->display, window->window);
        XCloseDisplay(window->display);
        return -1;
    }

    wm_delete_window = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->display, window->window, &wm_delete_window, 1);

    window->time_offset = get_time();
    return 0;
}

int x11_window_create(struct window *window, int width, int height)
{
    if (!window)
        return -1;

    memset(window, 0, sizeof(*window));
    window->should_close = true;

    if (gl_open_handle() != 0)
        return -1;

    if (__x11_window_create(window, width, height) != 0)
        return -1;

    if (gl_load_functions() != 0)
        return -1;

    printf(INFO "X11 WINDOW: created new window: width %d, height %d\n", width,
           height);

    return 0;
}

void x11_window_destroy(struct window *window)
{
    if (!window)
        return;

    glx_destroy_context(window);

    XDestroyWindow(window->display, window->window);
    XCloseDisplay(window->display);

    gl_close_handle();

    printf(INFO "X11 WINDOW: window destroyed\n");
}

void x11_window_show(struct window *window)
{
    if (!window)
        return;

    XMapWindow(window->display, window->window);
    window->should_close = false;
}

static void __x11_window_poll_events(struct window *window)
{
    XEvent event;

    if (!window)
        return;

    while (QLength(window->display)) {
        XNextEvent(window->display, &event);

        switch (event.type) {
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                window->should_close = true;
                return;
            }
            break;
        }
    }

    XFlush(window->display);
}

void x11_window_poll_events(struct window *window)
{
    static double last = 0.0;

    if (!window)
        return;

    double now = x11_window_get_time(window);
    window->delta_time = (float)(now - last);
    last = now;

    __x11_window_poll_events(window);
}

void x11_window_swap_buffers(struct window *window)
{
    if (window)
        glXSwapBuffers(window->display, window->window);
}

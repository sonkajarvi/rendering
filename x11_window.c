#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>

#include "rendering.h"

static Atom wm_delete_window;

int X11_window_create(struct window *win, int width, int height)
{
    XVisualInfo *vi;
    XSetWindowAttributes swa;

    if (!win)
        return -1;

    win->display = XOpenDisplay(NULL);
    if (!win->display) {
        printf(INFO "window: failed to open display\n");
        return -1;
    }

    vi = GLX_get_visual(win);
    if (!vi) {
        printf(ERROR "window: failed to get VisualInfo from FB config\n");
        XCloseDisplay(win->display);
    }

    win->root = DefaultRootWindow(win->display);

    memset(&swa, 0, sizeof(swa));
    swa.colormap = XCreateColormap(win->display, win->root, vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    win->window = XCreateWindow(
        win->display,
        win->root,
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

    if (GLX_create_context(win) != 0) {
        printf(ERROR "GLX: failed to create context\n");
        XDestroyWindow(win->display, win->window);
        XCloseDisplay(win->display);
        return -1;
    }

    wm_delete_window = XInternAtom(win->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(win->display, win->window, &wm_delete_window, 1);
    return 0;
}

void X11_window_destroy(struct window *w)
{
    GLX_destroy_context(w);

    XDestroyWindow(w->display, w->window);
    XCloseDisplay(w->display);
}

void X11_window_show(struct window *w)
{
    XMapWindow(w->display, w->window);
    w->should_close = false;
}

void X11_window_swap_buffers(struct window *w)
{
    glXSwapBuffers(w->display, w->window);
}

void X11_window_poll_events(struct window *w)
{
    XEvent event;

    if (!w)
        return;

    while (QLength(w->display)) {
        XNextEvent(w->display, &event);

        switch (event.type) {
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                w->should_close = true;
                return;
            }
            break;
        }
    }

    XFlush(w->display);
}

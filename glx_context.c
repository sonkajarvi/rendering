/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>

#include <X11/Xlib.h>

#include <glad/gl.h>
#include <glad/glx.h>

#include "window.h"

static int glx_load_functions(struct window *window, int screen)
{
    int version;

    version = gladLoadGLX(window->display, screen, gl_get_proc_address);
    if (!version) {
        printf(ERROR "GLX: failed to load functions\n");
        return -1;
    }

    printf(INFO "GLX: loaded functions for version %d.%d\n",
        GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    return 0;
}

static const int fbconfig_attribs[] = {
    GLX_X_RENDERABLE,   True,
    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
    GLX_DOUBLEBUFFER,   True,
    None
};

static int glx_choose_fbconfig(struct window *window, int screen,
                               GLXFBConfig *fbc)
{
    GLXFBConfig *configs;
    XVisualInfo *vi;
    int count, buffers, samples;
    int best, best_samples, worst, worst_samples;

    configs = glXChooseFBConfig(window->display, screen, fbconfig_attribs, &count);
    if (!configs) {
        printf(ERROR "GLX: failed to choose FB config\n");
        return -1;
    }

    best = -1, worst = -1, best_samples = -1, worst_samples = 999;
    for (int i = 0; i < count; i++) {
        vi = glXGetVisualFromFBConfig(window->display, configs[i]);
        if (vi) {
            glXGetFBConfigAttrib(window->display, configs[i],
                                 GLX_SAMPLE_BUFFERS, &buffers);
            glXGetFBConfigAttrib(window->display, configs[i], GLX_SAMPLES,
                                 &samples);

            if (best < 0 || (buffers && samples > best_samples)) {
                best = i;
                best_samples = samples;
            }
            if (worst < 0 || (buffers && samples < worst_samples)) {
                worst = i;
                worst_samples = samples;
            }

            XFree(vi);
        }
    }

    *fbc = configs[best];
    XFree(configs);
    return 0;
}

static const int context_attribs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB,  4,
    GLX_CONTEXT_MINOR_VERSION_ARB,  6,
    GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
};

int glx_create_context(struct window *window)
{
    window->context = glXCreateContextAttribsARB(window->display,
                                                 window->fbconfig, NULL, True,
                                                 context_attribs);
    if (!window->context)
        goto err;

    if (!glXMakeCurrent(window->display, window->window, window->context))
        goto err;

    return 0;

err:
    printf(ERROR "GLX: failed to create context\n");
    return -1;
}

void glx_destroy_context(struct window *window)
{
    glXMakeCurrent(window->display, None, NULL);
    glXDestroyContext(window->display, window->context);

    printf(INFO "GLX: context destroyed\n");
}

XVisualInfo *glx_get_visual(struct window *window)
{
    int screen;

    screen = DefaultScreen(window->display);
    if (glx_load_functions(window, screen) != 0)
        return NULL;

    if (glx_choose_fbconfig(window, screen, &window->fbconfig) != 0)
        return NULL;

    return glXGetVisualFromFBConfig(window->display, window->fbconfig);
}

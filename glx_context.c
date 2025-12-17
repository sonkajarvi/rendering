/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>

#include <X11/Xlib.h>

#include <glad/gl.h>
#include <glad/glx.h>

#include "rendering.h"

static int GLX_load_functions(struct window *w, int screen)
{
    int version;

    version = gladLoadGLX(w->display, screen, GL_get_proc_address);
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

static int GLX_choose_fbconfig(struct window *w, int screen, GLXFBConfig *fbc)
{
    GLXFBConfig *configs;
    XVisualInfo *vi;
    int count, buffers, samples;
    int best, best_samples, worst, worst_samples;

    configs = glXChooseFBConfig(w->display, screen, fbconfig_attribs, &count);
    if (!configs) {
        printf(ERROR "GLX: failed to choose FB config\n");
        return -1;
    }

    best = -1, worst = -1, best_samples = -1, worst_samples = 999;
    for (int i = 0; i < count; i++) {
        vi = glXGetVisualFromFBConfig(w->display, configs[i]);
        if (vi) {
            glXGetFBConfigAttrib(w->display, configs[i], GLX_SAMPLE_BUFFERS, &buffers);
            glXGetFBConfigAttrib(w->display, configs[i], GLX_SAMPLES, &samples);

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
    GLX_CONTEXT_MAJOR_VERSION_ARB,  3,
    GLX_CONTEXT_MINOR_VERSION_ARB,  3,
    GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
};

int GLX_create_context(struct window *w)
{
    w->context = glXCreateContextAttribsARB(w->display, w->fbconfig, NULL, True, context_attribs);
    if (!w->context)
        return -1;

    if (!glXMakeCurrent(w->display, w->window, w->context))
        return -1;

    return 0;
}

void GLX_destroy_context(struct window *w)
{
    glXMakeCurrent(w->display, None, NULL);
    glXDestroyContext(w->display, w->context);

    printf(INFO "GLX: context destroyed\n");
}

XVisualInfo *GLX_get_visual(struct window *w)
{
    int screen;

    screen = DefaultScreen(w->display);
    if (GLX_load_functions(w, screen) != 0)
        return NULL;

    if (GLX_choose_fbconfig(w, screen, &w->fbconfig) != 0)
        return NULL;

    return glXGetVisualFromFBConfig(w->display, w->fbconfig);
}

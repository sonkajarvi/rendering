/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <dlfcn.h>
#include <stdio.h>

#include <glad/gl.h>

#include "window.h"

static void *handle = NULL;

static const char *so_names[] = {
    "libGL.so",
    "libGL.so.1",
    NULL
};

int gl_open_handle(void)
{
    int i;

    if (!handle) {
        for (i = 0; so_names[i]; i++) {
            handle = dlopen(so_names[i], RTLD_LAZY | RTLD_LOCAL);
            if (handle)
                break;
        }

        if (!handle) {
            printf(ERROR "GL: failed to open handle\n");
            return -1;
        }

        printf(INFO "GL: opened handle for %s\n", so_names[i]);
    }

    return 0;
}

void gl_close_handle(void)
{
    if (handle)
        dlclose(handle);
}

GLADapiproc gl_get_proc_address(const char *name)
{
    return dlsym(handle, name);
}

int gl_load_functions(struct window *w)
{
    (void)w;
    int version;

    version = gladLoadGL(gl_get_proc_address);
    if (!version) {
        printf(ERROR "GL: failed to load functions\n");
        return -1;
    }

    printf(INFO "GL: loaded functions for version %d.%d\n",
        GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    printf(INFO "GL: OpenGL device information:\n");
    printf(INFO "GL: ... vendor:         %s\n", glGetString(GL_VENDOR));
    printf(INFO "GL: ... renderer:       %s\n", glGetString(GL_RENDERER));
    printf(INFO "GL: ... GL version:     %s\n", glGetString(GL_VERSION));
    printf(INFO "GL: ... GLSL version:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return 0;
}

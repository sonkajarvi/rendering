#include <dlfcn.h>
#include <stdio.h>

#include <glad/gl.h>

#include "rendering.h"

static void *handle = NULL;

static const char *so_names[] = {
    "libGL.so",
    "libGL.so.1",
    NULL
};

int GL_open_handle(void)
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

void GL_close_handle(void)
{
    if (handle)
        dlclose(handle);
}

GLADapiproc GL_get_proc_address(const char *name)
{
    return dlsym(handle, name);
}

int GL_load_functions(struct window *w)
{
    (void)w;
    int version;

    version = gladLoadGL(GL_get_proc_address);
    if (!version) {
        printf(ERROR "GL: failed to load functions\n");
        return -1;
    }

    printf(INFO "GL: loaded functions for version %d.%d\n",
        GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    printf(INFO "OpenGL device information:\n");
    printf(INFO "... vendor:         %s\n", glGetString(GL_VENDOR));
    printf(INFO "... renderer:       %s\n", glGetString(GL_RENDERER));
    printf(INFO "... GL version:     %s\n", glGetString(GL_VERSION));
    printf(INFO "... GLSL version:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return 0;
}

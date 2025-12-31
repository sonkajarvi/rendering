/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/gl.h>

#include "rendering.h"

/* Vertex buffer length, in bytes */
#define VERTEX_BUFFER_LENGTH 4096

static GLuint create_shader(struct renderer *rend, const char *v_path, const char *f_path)
{
    int ret;
    char *v_source, *f_source;
    GLuint program, v_shader, f_shader;

    v_source = f_source = NULL;
    program = v_shader = f_shader = 0;

    /* Vertex shader */

    v_source = read_file(v_path);
    if (!v_source)
        goto out_cleanup;

    v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, (void *)&v_source, NULL);
    glCompileShader(v_shader);

    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        glGetShaderInfoLog(v_shader, VERTEX_BUFFER_LENGTH, NULL, (void *)rend->v_buf);
        printf(ERROR "GL: failed to compile vertex shader:\n%s", (char *)rend->v_buf);
        goto out_cleanup;
    }

    printf(INFO "GL: compiled vertex shader: path %s\n", v_path);

    /* Fragment shader */

    f_source = read_file(f_path);
    if (!f_source)
        goto out_cleanup;

    f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, (void *)&f_source, NULL);
    glCompileShader(f_shader);

    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        glGetShaderInfoLog(f_shader, VERTEX_BUFFER_LENGTH, NULL, (void *)rend->v_buf);
        printf(ERROR "GL: failed to compile fragment shader:\n%s", (char *)rend->v_buf);
        goto out_cleanup;
    }

    printf(INFO "GL: compiled fragment shader: path %s\n", f_path);

    /* Shader program */

    program = glCreateProgram();
    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if (!ret) {
        glGetProgramInfoLog(program, VERTEX_BUFFER_LENGTH, NULL, (void *)rend->v_buf);
        printf(ERROR "GL: failed to create shader program:\n%s", (char *)rend->v_buf);
        goto out_cleanup;
    }

    printf(INFO "GL: created shader program: id %u\n", program);
    return program;

out_cleanup:
    free(v_source);
    glDeleteShader(v_shader);

    free(f_source);
    glDeleteShader(f_shader);

    glDeleteProgram(program);

    return 0;
}

int GL_renderer_create(struct renderer *rend)
{
    rend->v_buf = malloc(VERTEX_BUFFER_LENGTH);
    if (!rend->v_buf) {
        printf(ERROR "Failed to allocate vertex buffer\n");
        return -1;
    }

    glGenVertexArrays(1, &rend->vao);
    glBindVertexArray(rend->vao);

    glGenBuffers(1, &rend->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_LENGTH, NULL, GL_DYNAMIC_DRAW);

    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, position));
    glEnableVertexAttribArray(0);

    /* color */
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, color));
    glEnableVertexAttribArray(1);

    /* center */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, center));
    glEnableVertexAttribArray(2);
    // glVertexAttribDivisor(2, 1);

    /* radius */
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, radius));
    glEnableVertexAttribArray(3);
    // glVertexAttribDivisor(3, 1);

    rend->program = create_shader(rend, RESOURCES_PATH "/vertex_2d.glsl", RESOURCES_PATH "/fragment_2d.glsl");
    if (!rend->program) {
        free(rend->v_buf);
        return -1;
    }

    return 0;
}

void GL_renderer_destroy(struct renderer *rend)
{
    glDeleteBuffers(1, &rend->vbo);
    glDeleteVertexArrays(1, &rend->vao);
    glDeleteProgram(rend->program);

    free(rend->v_buf);
}

void GL_renderer_clear_color(struct renderer *rend, float r, float g, float b)
{
    (void)rend;
    glClearColor(r, g, b, 1.0f);
}

void GL_renderer_new_frame(struct renderer *rend, struct window *w)
{
    XWindowAttributes wa;

    XGetWindowAttributes(w->display, w->window, &wa);
    glViewport(0, 0, wa.width, wa.height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rend->program);

    GLfloat size[2];
    size[0] = wa.width;
    size[1] = wa.height;
    glUniform2fv(glGetUniformLocation(rend->program, "u_WindowSize"), 1, size);
}

void GL_renderer_flush_vertices(struct renderer *rend)
{
    size_t offset;

    glBindVertexArray(rend->vao);
    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo);

    offset = rend->v_ptr - rend->v_buf;
    glBufferSubData(GL_ARRAY_BUFFER, 0, offset * sizeof(struct vertex), rend->v_buf);
    glDrawArrays(GL_TRIANGLES, 0, offset);

    rend->v_ptr = rend->v_buf;
}

void GL_renderer_push_vertices(struct renderer *rend, struct vertex *vertices, size_t count)
{
    size_t offset;

    offset = rend->v_ptr - rend->v_buf;
    if ((offset + count) * sizeof(struct vertex) > VERTEX_BUFFER_LENGTH)
        GL_renderer_flush_vertices(rend);

    memcpy(rend->v_ptr, vertices, count * sizeof(struct vertex));
    rend->v_ptr += count;
}

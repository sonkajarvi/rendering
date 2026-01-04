/**
 * Copyright (c) 2025, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/gl.h>

#include "rendering.h"

/* Vertex buffer length, in bytes */
#define VERTEX_BUFFER_LENGTH 4096

static GLuint create_shader(struct renderer *rdr, const char *v_path, const char *f_path)
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
        glGetShaderInfoLog(v_shader, VERTEX_BUFFER_LENGTH, NULL, (void *)rdr->v_buf);
        printf(ERROR "GL: failed to compile vertex shader:\n%s", (char *)rdr->v_buf);
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
        glGetShaderInfoLog(f_shader, VERTEX_BUFFER_LENGTH, NULL, (void *)rdr->v_buf);
        printf(ERROR "GL: failed to compile fragment shader:\n%s", (char *)rdr->v_buf);
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
        glGetProgramInfoLog(program, VERTEX_BUFFER_LENGTH, NULL, (void *)rdr->v_buf);
        printf(ERROR "GL: failed to create shader program:\n%s", (char *)rdr->v_buf);
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

int GL_renderer_create(struct renderer *rdr)
{
    struct image white;

    rdr->v_buf = malloc(VERTEX_BUFFER_LENGTH);
    if (!rdr->v_buf) {
        printf(ERROR "Failed to allocate vertex buffer\n");
        return -1;
    }

    glGenVertexArrays(1, &rdr->vao);
    glBindVertexArray(rdr->vao);

    glGenBuffers(1, &rdr->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rdr->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_LENGTH, NULL, GL_DYNAMIC_DRAW);

    /* position */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
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

    /* size */
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, size));
    glEnableVertexAttribArray(3);

    /* radius */
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, radius));
    glEnableVertexAttribArray(4);

    /* texture coords */
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, texture_coords));
    glEnableVertexAttribArray(5);

    /* texture index */
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, texture_index));
    glEnableVertexAttribArray(6);

    rdr->program = create_shader(rdr, RESOURCES_PATH "/vertex_2d.glsl", RESOURCES_PATH "/fragment_2d.glsl");
    if (!rdr->program) {
        free(rdr->v_buf);
        return -1;
    }

    white.data = (uint8_t[]){0xff, 0xff, 0xff, 0xff};
    white.width = 1;
    white.height = 1;
    GL_texture_create(&rdr->white_texture, &white);

    memset(rdr->tex_array, 0, sizeof(rdr->tex_array));
    rdr->tex_count = 0;

    return 0;
}

void GL_renderer_destroy(struct renderer *rdr)
{
    glDeleteBuffers(1, &rdr->vbo);
    glDeleteVertexArrays(1, &rdr->vao);
    glDeleteProgram(rdr->program);

    GL_texture_destroy(&rdr->white_texture);
    free(rdr->v_buf);
}

void GL_renderer_clear_color(struct renderer *rdr, float r, float g, float b)
{
    (void)rdr;
    glClearColor(r, g, b, 1.0f);
}

void GL_renderer_new_frame(struct renderer *rdr, struct window *w)
{
    XWindowAttributes wa;

    XGetWindowAttributes(w->display, w->window, &wa);
    glViewport(0, 0, wa.width, wa.height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rdr->program);

    GLfloat size[2];
    size[0] = wa.width;
    size[1] = wa.height;
    glUniform2fv(glGetUniformLocation(rdr->program, "u_WindowSize"), 1, size);

    rdr->v_ptr = rdr->v_buf;

    rdr->tex_count = 0;
    GL_renderer_use_texture(rdr, &rdr->white_texture);
}

void GL_renderer_flush_vertices(struct renderer *rdr)
{
    static const GLint array[] = {0, 1, 2, 3};
    size_t offset;

    if (rdr->v_ptr == rdr->v_buf)
        return;

    glUniform1iv(glGetUniformLocation(rdr->program, "u_Textures"), 4, array);

    for (int i = 0; i < rdr->tex_count; i++)
        glBindTextureUnit(i, rdr->tex_array[i]->id);

    glBindVertexArray(rdr->vao);
    glBindBuffer(GL_ARRAY_BUFFER, rdr->vbo);

    offset = rdr->v_ptr - rdr->v_buf;
    glBufferSubData(GL_ARRAY_BUFFER, 0, offset * sizeof(struct vertex), rdr->v_buf);
    glDrawArrays(GL_TRIANGLES, 0, offset);

    rdr->v_ptr = rdr->v_buf;

    for (int i = 0; i < rdr->tex_count; i++)
        rdr->tex_array[i]->index = -1;
}

void GL_renderer_push_vertices(struct renderer *rdr, struct vertex *vertices, size_t count)
{
    size_t offset;

    offset = rdr->v_ptr - rdr->v_buf;
    if ((offset + count) * sizeof(struct vertex) > VERTEX_BUFFER_LENGTH)
        GL_renderer_flush_vertices(rdr);

    memcpy(rdr->v_ptr, vertices, count * sizeof(struct vertex));
    rdr->v_ptr += count;
}

void GL_renderer_use_texture(struct renderer *rdr, struct texture *tex)
{
    /* TODO: this should not fail silently */
    if (rdr->tex_count == ARRAYSIZE(rdr->tex_array))
        return;

    if (tex->index != -1)
        return;

    rdr->tex_array[rdr->tex_count] = tex;
    tex->index = rdr->tex_count;
    rdr->tex_count++;
}

/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/gl.h>

#include "color.h"
#include "gl_renderer.h"
#include "image.h"
#include "renderer.h"
#include "window.h"

/* Vertex buffer length, in bytes */
#define VERTEX_BUFFER_LENGTH (sizeof(struct vertex) * 1024)

static void gl_renderer_destroy(struct renderer *renderer)
{
    struct gl_renderer *gl_renderer = renderer_priv(renderer);

    glDeleteBuffers(1, &gl_renderer->vbo);
    glDeleteVertexArrays(1, &gl_renderer->vao);
    glDeleteProgram(gl_renderer->program);

    gl_texture_destroy(gl_renderer->text_white);
    free(gl_renderer->vert_buf);
}

static void gl_renderer_new_frame(struct renderer *renderer,
                                  struct window *window)
{
    struct gl_renderer *gl_renderer = renderer_priv(renderer);
    XWindowAttributes wa;

    /* TODO: do not call X11 directly here */
    XGetWindowAttributes(window->display, window->window, &wa);
    glViewport(0, 0, wa.width, wa.height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(COLOR_INT_TO_FLOAT(COLOR_R(renderer->clear_color)),
                 COLOR_INT_TO_FLOAT(COLOR_G(renderer->clear_color)),
                 COLOR_INT_TO_FLOAT(COLOR_B(renderer->clear_color)),
                 1.0f);

    glUseProgram(gl_renderer->program);

    GLfloat size[2];
    size[0] = wa.width;
    size[1] = wa.height;
    glUniform2fv(glGetUniformLocation(gl_renderer->program, "u_WindowSize"), 1,
                                      size);

    gl_renderer->vert_ptr = gl_renderer->vert_buf;

    gl_renderer->text_used = 0;
    renderer_use_texture(renderer, gl_renderer->text_white);
}

static void gl_renderer_push_vertices(struct renderer *renderer,
                                      struct vertex *vertices, size_t count)
{
    struct gl_renderer *gl_renderer = renderer_priv(renderer);
    size_t offset;

    offset = gl_renderer->vert_ptr - gl_renderer->vert_buf;
    if ((offset + count) * sizeof(struct vertex) > VERTEX_BUFFER_LENGTH)
        renderer_flush_vertices(renderer);

    memcpy(gl_renderer->vert_ptr, vertices, count * sizeof(struct vertex));
    gl_renderer->vert_ptr += count;
}

static void gl_renderer_flush_vertices(struct renderer *renderer)
{
    struct gl_renderer *gl_renderer = renderer_priv(renderer);
    static const GLint array[] = {0, 1, 2, 3};
    size_t offset;

    if (gl_renderer->vert_ptr == gl_renderer->vert_buf)
        return;

    glUniform1iv(glGetUniformLocation(gl_renderer->program, "u_Textures"), 4,
                                      array);

    for (int i = 0; i < gl_renderer->text_used; i++)
        glBindTextureUnit(i, gl_renderer->text_array[i]->id);

    glBindVertexArray(gl_renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, gl_renderer->vbo);

    offset = gl_renderer->vert_ptr - gl_renderer->vert_buf;
    glBufferSubData(GL_ARRAY_BUFFER, 0, offset * sizeof(struct vertex),
                    gl_renderer->vert_buf);
    glDrawArrays(GL_TRIANGLES, 0, offset);

    gl_renderer->vert_ptr = gl_renderer->vert_buf;

    for (int i = 0; i < gl_renderer->text_used; i++)
        gl_renderer->text_array[i]->index = -1;
}

static struct texture *gl_renderer_use_texture(struct renderer *renderer,
                                               struct texture *texture)
{
    struct gl_renderer *gl_renderer = renderer_priv(renderer);

    if (gl_renderer->text_used == ARRAY_SIZE(gl_renderer->text_array))
        return NULL;

    if (!texture)
        texture = gl_renderer->text_white;

    if (texture->index != -1)
        goto out;

    gl_renderer->text_array[gl_renderer->text_used] = texture;
    texture->index = gl_renderer->text_used;
    gl_renderer->text_used++;

out:
    return texture;
}

static void gl_initialize_vertex_objects(struct gl_renderer *gl_renderer)
{
    glGenVertexArrays(1, &gl_renderer->vao);
    glBindVertexArray(gl_renderer->vao);

    glGenBuffers(1, &gl_renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl_renderer->vbo);
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

    /* stroke width */
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, stroke_width));
    glEnableVertexAttribArray(7);

    /* rotation */
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE,
        sizeof(struct vertex), (void *)offsetof(struct vertex, rotation));
    glEnableVertexAttribArray(8);
}

static bool gl_initialize_shader(struct gl_renderer *gl_renderer,
                                 const char *vert_path, const char *frag_path)
{
    char *vert_src = NULL, *frag_src = NULL;
    GLuint vert_shader = 0, frag_shader = 0;
    GLint success;

    /*
     * Vertex shader
     */
    vert_src = read_file(vert_path);
    if (!vert_src) {
        success = false;
        goto err;
    }

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, (void *)&vert_src, NULL);
    glCompileShader(vert_shader);

    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, VERTEX_BUFFER_LENGTH, NULL,
                           (void *)gl_renderer->vert_buf);
        printf(ERROR "GL RENDERER: failed to compile fragment sahder:\n%s",
               (char *)gl_renderer->vert_buf);
        goto err;
    }

    printf(INFO "GL RENDERER: compiled vertex shader: path %s\n", vert_path);

    /*
     * Fragment shader
     */
    frag_src = read_file(frag_path);
    if (!frag_path) {
        success = false;
        goto err;
    }

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (void *)&frag_src, NULL);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, VERTEX_BUFFER_LENGTH, NULL,
                           (void *)gl_renderer->vert_buf);
        printf(ERROR "GL RENDERER: failed to compile fragment shader:\n%s",
               (char *)gl_renderer->vert_buf);
        goto err;
    }

    printf(INFO "GL RENDERER: compiled fragment shader: path %s\n", frag_path);

    /*
     * Shader program
     */
    gl_renderer->program = glCreateProgram();
    glAttachShader(gl_renderer->program, vert_shader);
    glAttachShader(gl_renderer->program, frag_shader);
    glLinkProgram(gl_renderer->program);

    glGetProgramiv(gl_renderer->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(gl_renderer->program, VERTEX_BUFFER_LENGTH, NULL,
                            (void *)gl_renderer->vert_buf);
        printf(ERROR "GL RENDERER: failed to create shader program:\n%s",
               (char *)gl_renderer->vert_buf);
        goto err;
    }

    printf(INFO "GL RENDERER: created shader program: id %u\n",
           gl_renderer->program);

err:
    free(vert_src);
    glDeleteShader(vert_shader);

    free(frag_src);
    glDeleteShader(frag_shader);

    if (!success)
        glDeleteProgram(gl_renderer->program);

    return success;
}

struct renderer *gl_renderer_create(void)
{
    struct gl_renderer *gl_renderer;
    struct renderer *renderer;
    struct image image;
    int ret;

    renderer = renderer_alloc(sizeof(*gl_renderer));
    if (!renderer)
        return NULL;

    renderer->destroy = gl_renderer_destroy;
    renderer->new_frame = gl_renderer_new_frame;
    renderer->push_vertices = gl_renderer_push_vertices;
    renderer->flush_vertices = gl_renderer_flush_vertices;
    renderer->use_texture = gl_renderer_use_texture;
    gl_renderer = renderer_priv(renderer);

    gl_renderer->vert_buf = malloc(VERTEX_BUFFER_LENGTH);
    if (!gl_renderer->vert_buf) {
        printf(ERROR "GL RENDERER: failed to allocate vertex buffer\n");
        goto free_renderer;
    }

    gl_initialize_vertex_objects(gl_renderer);
    ret = gl_initialize_shader(gl_renderer,
                               RESOURCES_PATH "/vertex_2d.glsl",
                               RESOURCES_PATH "/fragment_2d.glsl");
    if (!ret)
        goto free_vert_buf;

    image.data = (unsigned char[]){0xff, 0xff, 0xff, 0xff};
    image.width = 1;
    image.height = 1;
    gl_renderer->text_white = gl_texture_create(&image);

    return renderer;

free_vert_buf:
    free(gl_renderer->vert_buf);
free_renderer:
    free(renderer);

    return NULL;
}

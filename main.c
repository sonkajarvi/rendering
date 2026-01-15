#include "rendering.h"

int main(void)
{
    struct window win;
    struct image img;
    struct texture tex;

    if (window_create(&win, 1200, 800) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);
    window_show(&win);

    image_load_from_file(&img, RESOURCES_PATH "/obama.jpg");
    GL_texture_create(&tex, &img);
    image_free(&img);

    while (!window_should_close(&win)) {
        GL_renderer_new_frame(&win.renderer, &win);

        renderer_draw_roundrect(&win.renderer, (vec2){-400, 0}, (vec2){100, 100}, (vec4){50, 50, 50, 50}, 0xcc321aff);
        renderer_draw_roundrect(&win.renderer, (vec2){-200, 0}, (vec2){100, 100}, (vec4){38, 38, 38, 38}, 0xcc321aff);
        renderer_draw_roundrect(&win.renderer, (vec2){   0, 0}, (vec2){100, 100}, (vec4){25, 25, 25, 25}, 0xcc321aff);
        renderer_draw_roundrect(&win.renderer, (vec2){ 200, 0}, (vec2){100, 100}, (vec4){13, 13, 13, 13}, 0xcc321aff);
        renderer_draw_roundrect(&win.renderer, (vec2){ 400, 0}, (vec2){100, 100}, (vec4){ 0,  0,  0,  0}, 0xcc321aff);

        GL_renderer_flush_vertices(&win.renderer);

        window_poll_events(&win);
        window_swap_buffers(&win);
    }

    GL_texture_destroy(&tex);
    window_destroy(&win);
    return 0;
}

#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
// layout (location = 2) in vec2 a_tex_coords;
// layout (location = 3) in float a_tex_index;

out vec4 v_color;
// out vec2 v_tex_coords;
// out float v_tex_index;

uniform vec2 u_window_size;

void main()
{
    v_color = a_color;
    // v_tex_coords = a_tex_coords;
    // v_tex_index = a_tex_index;

    vec2 position;
    position.x = (a_position.x / u_window_size.x) * 2.0;
    position.y = (a_position.y / u_window_size.y) * 2.0;

    gl_Position = vec4(position, 0.0, 1.0);
}

#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_Center;
layout (location = 3) in float a_Radius;

out vec4 v_Color;
flat out vec2 v_Center;
flat out float v_Radius;

uniform vec2 u_WindowSize;

void main()
{
    v_Color = a_Color;
    v_Center = a_Center;
    v_Radius = a_Radius;

    vec2 p = 2.0 * a_Position / u_WindowSize;
    gl_Position = vec4(p, 0.0, 1.0);
}

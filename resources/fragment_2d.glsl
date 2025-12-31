#version 330 core

in vec4 v_Color;
flat in vec2 v_Center;
flat in float v_Radius;

out vec4 o_Color;

uniform vec2 u_WindowSize;

void main()
{
    vec2 c = v_Center + u_WindowSize / 2.0;

    if (length(gl_FragCoord.xy - c) > v_Radius)
        discard;

    o_Color = v_Color;
}

#version 330 core

flat in vec4 v_Color;
flat in vec2 v_Center;
flat in vec2 v_Size;
flat in vec4 v_Radius;

out vec4 o_Color;

uniform vec2 u_WindowSize;

bool in_roundrect(vec2 point, vec2 center, vec2 size, vec4 radius)
{
    point -= center;

    radius.xy = point.x > 0.0 ? radius.xy : radius.wz;
    radius.x = point.y > 0.0 ? radius.x : radius.y;

    vec2 m = abs(point) - size / 2.0 + radius.x;
    return min(max(m.x, m.y), 0.0) + length(max(m, 0.0)) < radius.x;
}

void main()
{
    vec2 p = gl_FragCoord.xy - u_WindowSize / 2.0;
    vec2 c = v_Center;

    if (in_roundrect(p, c, v_Size, v_Radius))
        o_Color = v_Color;
    else
        discard;
        // o_Color = vec4(0.0, 0.0, 0.0, 1.0);
}

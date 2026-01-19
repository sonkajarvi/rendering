#version 330 core

flat in vec4 v_Color;
flat in vec2 v_Center;
flat in vec2 v_Size;
flat in vec4 v_Radius;
in vec2 v_TextureCoords;
flat in float v_TextureIndex;
flat in float v_StrokeWidth;

out vec4 o_Color;

uniform vec2 u_WindowSize;
uniform sampler2D u_Textures[4];

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

    if (v_StrokeWidth > 0.0) {
        float half = v_StrokeWidth / 2.0;

        vec2 so = v_Size + v_StrokeWidth;
        vec4 ro = v_Radius;
        ro.x += ro.x > 0 ? half : 0.0;
        ro.y += ro.y > 0 ? half : 0.0;
        ro.z += ro.z > 0 ? half : 0.0;
        ro.w += ro.w > 0 ? half : 0.0;

        vec2 si = v_Size - v_StrokeWidth;
        vec4 ri = v_Radius - half;

        if (!in_roundrect(p, v_Center, so, ro) || in_roundrect(p, v_Center, si, ri))
            discard;
    } else {
        if (!in_roundrect(p, v_Center, v_Size, v_Radius))
            discard;
    }

    vec4 c = v_Color;

    switch (int(v_TextureIndex)) {
    case 0: c *= texture(u_Textures[0], v_TextureCoords); break;
    case 1: c *= texture(u_Textures[1], v_TextureCoords); break;
    case 2: c *= texture(u_Textures[2], v_TextureCoords); break;
    case 3: c *= texture(u_Textures[3], v_TextureCoords); break;
    }

    o_Color = c;
}

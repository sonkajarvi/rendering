#version 330 core


flat in vec4 v_Color;
flat in vec2 v_Center;
flat in vec2 v_Size;
flat in vec4 v_Radius;
in vec2 v_TextureCoords;
flat in float v_TextureIndex;

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

    if (!in_roundrect(p, v_Center, v_Size, v_Radius))
        discard;

    vec4 c = v_Color;

    switch (int(v_TextureIndex)) {
    case 0: c *= texture(u_Textures[0], v_TextureCoords); break;
    case 1: c *= texture(u_Textures[1], v_TextureCoords); break;
    case 2: c *= texture(u_Textures[2], v_TextureCoords); break;
    case 3: c *= texture(u_Textures[3], v_TextureCoords); break;
    }

    o_Color = c;
}

/**
 * Copyright (c) 2025-2026, sonkajarvi
 *
 * Licensed under the BSD 2-Clause License.
 * The full license can be found in the LICENSE.txt file.
 */

#version 330 core

flat in vec4 v_Color;
flat in vec2 v_Center;
flat in vec2 v_Size;
flat in vec4 v_Radius;
in vec2 v_TextureCoords;
flat in float v_TextureIndex;
flat in float v_StrokeWidth;
flat in float v_Rotation;

out vec4 o_Color;

uniform vec2 u_WindowSize;
uniform sampler2D u_Textures[4];

layout(origin_upper_left) in vec4 gl_FragCoord;

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
    vec3 v = gl_FragCoord.xyz;

    vec3 k = vec3(0.0, 0.0, 1.0);
    vec3 p = vec3(v_Center.x, v_Center.y, 0.0);
    float a = radians(-v_Rotation);

    /* Rodrigues' rotation formula (with pivot, inverted) */
    v = p + (v - p) * cos(a) +
        cross(k, v - p) * sin(a) +
        k * dot(k, v - p) * (1 - cos(a));

    vec2 pos = v.xy;

    if (v_StrokeWidth > 0.0) {
        float half = v_StrokeWidth / 2.0;

        vec2 size_out = v_Size + v_StrokeWidth;
        vec4 radius_out = v_Radius;
        radius_out.x += radius_out.x > 0 ? half : 0.0;
        radius_out.y += radius_out.y > 0 ? half : 0.0;
        radius_out.z += radius_out.z > 0 ? half : 0.0;
        radius_out.w += radius_out.w > 0 ? half : 0.0;

        vec2 size_in = v_Size - v_StrokeWidth;
        vec4 radius_in = v_Radius - half;

        if (!in_roundrect(pos, v_Center, size_out, radius_out) ||
            in_roundrect(pos, v_Center, size_in, radius_in))
            discard;
    } else {
        if (!in_roundrect(pos, v_Center, v_Size, v_Radius))
            discard;
    }

    vec4 color = v_Color;

    switch (int(v_TextureIndex)) {
    case 0: color *= texture(u_Textures[0], v_TextureCoords); break;
    case 1: color *= texture(u_Textures[1], v_TextureCoords); break;
    case 2: color *= texture(u_Textures[2], v_TextureCoords); break;
    case 3: color *= texture(u_Textures[3], v_TextureCoords); break;
    }

    o_Color = color;
}

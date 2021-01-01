#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Margin;

float negativeCircle(vec2 center, float radius)
{
    float dist = distance(b_Pos, center);
    float delta = fwidth(dist);
    return smoothstep(radius - delta / 2.0, radius + delta / 2.0, dist);
}
float positiveCircle(vec2 center, float radius)
{
    float dist = distance(b_Pos, center);
    float delta = fwidth(dist);
    return 1.0 - smoothstep(radius - delta / 2.0, radius + delta / 2.0, dist);
}

void main()
{
    o_Color.rgba = b_Color.rgba;
    float halfSize = b_Data.b / 2.0;

    o_Color.a *= positiveCircle(vec2(b_Data.r + halfSize, b_Data.g + halfSize), halfSize - u_Margin);
}
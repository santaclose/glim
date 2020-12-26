#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Margin;
uniform vec3 u_HighlightCircle;

float fadeRange;

float negativeCircle(vec2 center, float radius)
{
    return smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(b_Pos, center));
}
float positiveCircle(vec2 center, float radius)
{
    return 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(b_Pos, center));
}

void main()
{
    fadeRange = 1.2;

    o_Color.rgba = b_Color.rgba;
    float halfSize = b_Data.b / 2.0;

    // highlight on mouse over
    o_Color.rgb += vec3(0.1 * float(distance(b_Pos, u_HighlightCircle.rg) < u_HighlightCircle.b));

    o_Color.a *= positiveCircle(vec2(b_Data.r + halfSize, b_Data.g + halfSize), halfSize - u_Margin);
}
#version 460 core

layout(location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Margin;
uniform float u_CornerRadius;
uniform float u_Thickness;

float fadeRange;

float udRoundBox(vec2 p, vec2 b, float r)
{
    return length(max(abs(p) - b + r, 0.0)) - r;
}

float positiveRoundedRectangle(vec2 topLeftCorner, vec2 bottomRightCorner)
{
    vec2 rectSize = bottomRightCorner - topLeftCorner;
    vec2 rectCenter = topLeftCorner + rectSize / 2.0;
    vec2 p = b_Pos - rectCenter;
    return 1.0 - clamp(udRoundBox(p, rectSize / 2.0, u_CornerRadius), 0.0, 1.0);
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

    vec2 topLeftCorner = b_Data.rg + vec2(u_Margin, u_Margin);
    vec2 bottomRightCorner = b_Data.ba - vec2(u_Margin, u_Margin);

    float roundedBoxMultiplier = positiveRoundedRectangle(topLeftCorner, bottomRightCorner);

    o_Color.a *= roundedBoxMultiplier;

    // add checkmark if value is true
    roundedBoxMultiplier =
        b_MoreData.x *
        positiveCircle((topLeftCorner + bottomRightCorner) / 2.0, (-topLeftCorner.x + bottomRightCorner.x) / 2.0 - 3.0);

    o_Color.a *= 1.0 - roundedBoxMultiplier;
}
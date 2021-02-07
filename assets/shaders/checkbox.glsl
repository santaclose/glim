#version 460 core

layout(location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_CornerRadius;

float udRoundBox(vec2 p, vec2 b, float r)
{
    return length(max(abs(p) - b + r, 0.0)) - r;
}

float positiveRoundedRectangle(vec2 topLeftCorner, vec2 bottomRightCorner)
{
    vec2 rectSize = bottomRightCorner - topLeftCorner;
    vec2 rectCenter = topLeftCorner + rectSize / 2.0;
    vec2 fixedPos = b_Pos;

    rectSize.x = floor(rectSize.x);
    rectSize.y = floor(rectSize.y);
    rectCenter.x = floor(rectCenter.x);
    rectCenter.y = floor(rectCenter.y);
    rectCenter.x += 0.5 * float(mod(rectSize.x, 2.0) < 0.1);
    rectCenter.y += 0.5 * float(mod(rectSize.y, 2.0) < 0.1);

    vec2 p = fixedPos - rectCenter;
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

    vec2 topLeftCorner = b_Data.rg + vec2(1.0, 1.0);
    vec2 bottomRightCorner = b_Data.ba - vec2(1.0, 1.0);

    float roundedBoxMultiplier = positiveRoundedRectangle(topLeftCorner, bottomRightCorner);

    o_Color.a *= roundedBoxMultiplier;

    // add checkmark if value is true
    roundedBoxMultiplier =
        b_MoreData.x *
        positiveCircle((topLeftCorner + bottomRightCorner) / 2.0, (-topLeftCorner.x + bottomRightCorner.x) / 2.0 - 3.0);

    o_Color.a *= 1.0 - roundedBoxMultiplier;
}
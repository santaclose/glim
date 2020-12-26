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

float positiveRectangle(vec2 topLeftCorner, vec2 bottomRightCorner)
{
    float val = 0.0;
    val += smoothstep(topLeftCorner.x - 0.2 / 2.0, topLeftCorner.x + 0.2 / 2.0, b_Pos.x);
    val *= smoothstep(bottomRightCorner.x + 0.2 / 2.0, bottomRightCorner.x - 0.2 / 2.0, b_Pos.x);
    val *= smoothstep(bottomRightCorner.y + 0.2 / 2.0, bottomRightCorner.y - 0.2 / 2.0, b_Pos.y);
    val *= smoothstep(topLeftCorner.y - 0.2 / 2.0, topLeftCorner.y + 0.2 / 2.0, b_Pos.y);
    return val;
}
float positiveCircle(vec2 center, float radius)
{
    return 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(b_Pos, center));
}

void main()
{
    fadeRange = 1.2;

    o_Color.rgba = b_Color.rgba;

    vec2 topLeftCorner = b_Data.rg + vec2(u_Margin, u_Margin);
    vec2 bottomRightCorner = b_Data.ba - vec2(u_Margin, u_Margin);

    float roundedBoxMultiplier = 0.0;
    vec2 cursor;
    cursor = topLeftCorner + vec2(u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = bottomRightCorner - vec2(u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = vec2(topLeftCorner.x + u_CornerRadius, bottomRightCorner.y - u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = vec2(bottomRightCorner.x - u_CornerRadius, topLeftCorner.y + u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);

    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x + u_CornerRadius, topLeftCorner.y),
            vec2(bottomRightCorner.x - u_CornerRadius, bottomRightCorner.y)),
        roundedBoxMultiplier);
    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x, topLeftCorner.y + u_CornerRadius),
            vec2(bottomRightCorner.x, bottomRightCorner.y - u_CornerRadius)),
        roundedBoxMultiplier);

    roundedBoxMultiplier = min(roundedBoxMultiplier, 1.0);

    o_Color.a *= roundedBoxMultiplier;

    // subtract inner box

    topLeftCorner = b_Data.rg + vec2(u_Margin + u_Thickness, u_Margin + u_Thickness);
    bottomRightCorner = b_Data.ba - vec2(u_Margin + u_Thickness, u_Margin + u_Thickness);

    roundedBoxMultiplier = 0.0;
    cursor;
    cursor = topLeftCorner + vec2(u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = bottomRightCorner - vec2(u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = vec2(topLeftCorner.x + u_CornerRadius, bottomRightCorner.y - u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);
    cursor = vec2(bottomRightCorner.x - u_CornerRadius, topLeftCorner.y + u_CornerRadius);
    roundedBoxMultiplier += positiveCircle(cursor, u_CornerRadius);

    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x + u_CornerRadius, topLeftCorner.y),
            vec2(bottomRightCorner.x - u_CornerRadius, bottomRightCorner.y)),
        roundedBoxMultiplier);
    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x, topLeftCorner.y + u_CornerRadius),
            vec2(bottomRightCorner.x, bottomRightCorner.y - u_CornerRadius)),
        roundedBoxMultiplier);

    roundedBoxMultiplier = 1.0 - min(roundedBoxMultiplier, 1.0);

    // add checkmark if value is true

    roundedBoxMultiplier +=
        b_MoreData.x *
        positiveCircle((topLeftCorner + bottomRightCorner) / 2.0, (-topLeftCorner.x + bottomRightCorner.x) / 2.0 - 2.0);

    o_Color.a *= roundedBoxMultiplier;
}
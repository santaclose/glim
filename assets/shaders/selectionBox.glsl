#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Radius;
uniform float u_OptionHeight;
uniform vec4 u_SelectionData;

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
    fadeRange = 1.5;

    o_Color.rgba = b_Color.rgba;
    
    vec2 topLeftCorner = b_Data.rg + vec2(1,1);
    vec2 bottomRightCorner = b_Data.ba - vec2(1,1);

    float roundedBoxMultiplier = 0.0;
    vec2 cursor;
    cursor = topLeftCorner + vec2(u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = bottomRightCorner - vec2(u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = vec2(topLeftCorner.x + u_Radius, bottomRightCorner.y - u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = vec2(bottomRightCorner.x - u_Radius, topLeftCorner.y + u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);

    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x + u_Radius, topLeftCorner.y),
            vec2(bottomRightCorner.x - u_Radius, bottomRightCorner.y)),
        roundedBoxMultiplier);
    roundedBoxMultiplier = max(
        positiveRectangle(
            vec2(topLeftCorner.x, topLeftCorner.y + u_Radius),
            vec2(bottomRightCorner.x, bottomRightCorner.y - u_Radius)),
        roundedBoxMultiplier);

    o_Color.a *= min(roundedBoxMultiplier, 1.0);
    
    // highlight on mouse over
    // u_SelectionData.r === selectedOptionIndex
    // u_SelectionData.g === leftLimit
    // u_SelectionData.b === topLimit
    // u_SelectionData.a === rightLimit
    if (u_SelectionData.r < 0.0)
        return;

    o_Color.rgb += vec3(0.1 * float(
        b_Pos.y > u_SelectionData.b + u_Radius + u_OptionHeight * (u_SelectionData.r) &&
        b_Pos.y < u_SelectionData.b + u_Radius + u_OptionHeight * (u_SelectionData.r + 1.0f) &&
        b_Pos.x > u_SelectionData.g &&
        b_Pos.x < u_SelectionData.a)
    );
}
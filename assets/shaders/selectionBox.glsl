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
    fadeRange = 1.5;

    o_Color.rgba = b_Color.rgba;
    
    float roundedBoxMultiplier = 0.0;
    vec2 cursor;
    cursor = b_Data.rg + vec2(u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = b_Data.ba - vec2(u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = vec2(b_Data.r + u_Radius, b_Data.a - u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    cursor = vec2(b_Data.b - u_Radius, b_Data.g + u_Radius);
    roundedBoxMultiplier += positiveCircle(cursor, u_Radius);
    roundedBoxMultiplier += float(b_Pos.x > b_Data.r + u_Radius && b_Pos.x < b_Data.b - u_Radius);
    roundedBoxMultiplier += float(b_Pos.y > b_Data.g + u_Radius && b_Pos.y < b_Data.a - u_Radius);

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
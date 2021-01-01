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

float udRoundBox(vec2 p, vec2 b, float r)
{
    return length(max(abs(p) - b + r, 0.0)) - r;
}

float positiveRoundedRectangle(vec2 topLeftCorner, vec2 bottomRightCorner)
{
    vec2 rectSize = bottomRightCorner - topLeftCorner;
    vec2 rectCenter = topLeftCorner + rectSize / 2.0;
    vec2 p = b_Pos - rectCenter;
    return 1.0 - clamp(udRoundBox(p, rectSize / 2.0, u_Radius), 0.0, 1.0);
}
float positiveCircle(vec2 center, float radius)
{
    float dist = distance(b_Pos, center);
    float delta = fwidth(dist);
    return 1.0 - smoothstep(radius - delta / 2.0, radius + delta / 2.0, dist);
}

void main()
{
    fadeRange = 1.2;

    o_Color.rgba = b_Color.rgba;
    
    vec2 topLeftCorner = b_Data.rg + vec2(1,1);
    vec2 bottomRightCorner = b_Data.ba - vec2(1,1);

    float roundedBoxMultiplier = 0.0;

    roundedBoxMultiplier += positiveRoundedRectangle(topLeftCorner, bottomRightCorner);

    o_Color.a *= roundedBoxMultiplier;
    
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
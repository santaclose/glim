#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_HandleRadius;
uniform float u_RailThickness;
uniform float u_SideMargin;

float fadeRange;

float positiveCircle(vec2 center, float radius)
{
    return 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(b_Pos, center));
}

void main()
{
    fadeRange = 1.2;
    vec2 topLeftQuadPos = b_Data.rg;
    vec2 bottomRightQuadPos = b_Data.ba;

    o_Color.rgba = b_Color.rgba;

    float middleRailPosY = (topLeftQuadPos.y + bottomRightQuadPos.y) / 2.0;

    float railRectangle = float(
        b_Pos.x > topLeftQuadPos.x + u_HandleRadius + u_SideMargin &&
        b_Pos.x < bottomRightQuadPos.x - u_HandleRadius - u_SideMargin &&
        b_Pos.y > middleRailPosY - u_RailThickness / 2.0 &&
        b_Pos.y < middleRailPosY + u_RailThickness / 2.0
        );

    float handleCircle = positiveCircle(
        vec2(mix(topLeftQuadPos.x + u_HandleRadius + u_SideMargin, bottomRightQuadPos.x - u_HandleRadius - u_SideMargin, b_MoreData.x), middleRailPosY),
        u_HandleRadius
        );

    o_Color.a = min(railRectangle + handleCircle, o_Color.a);
    o_Color.rgb -= vec3(railRectangle * 0.1 * (1.0 - handleCircle));
}
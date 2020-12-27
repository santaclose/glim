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

    float railRectangle;
    float handleCircle;

    // horizontal slider
    if (b_MoreData.y == 0.0)
    {
        float middleRailPosY = (topLeftQuadPos.y + bottomRightQuadPos.y) / 2.0;

        railRectangle = float(
            b_Pos.x > topLeftQuadPos.x + u_HandleRadius + u_SideMargin &&
            b_Pos.x < bottomRightQuadPos.x - u_HandleRadius - u_SideMargin &&
            b_Pos.y > middleRailPosY - u_RailThickness / 2.0 &&
            b_Pos.y < middleRailPosY + u_RailThickness / 2.0
            );

        handleCircle = positiveCircle(
            vec2(mix(topLeftQuadPos.x + u_HandleRadius + u_SideMargin, bottomRightQuadPos.x - u_HandleRadius - u_SideMargin, b_MoreData.x), middleRailPosY),
            u_HandleRadius
        );
    }
    // vertical slider
    else
    {
        float middleRailPosX = (topLeftQuadPos.x + bottomRightQuadPos.x) / 2.0;

        railRectangle = float(
            b_Pos.y > topLeftQuadPos.y + u_HandleRadius + u_SideMargin &&
            b_Pos.y < bottomRightQuadPos.y - u_HandleRadius - u_SideMargin &&
            b_Pos.x > middleRailPosX - u_RailThickness / 2.0 &&
            b_Pos.x < middleRailPosX + u_RailThickness / 2.0
            );

        handleCircle = positiveCircle(
            vec2(middleRailPosX, mix(topLeftQuadPos.y + u_HandleRadius + u_SideMargin, bottomRightQuadPos.y - u_HandleRadius - u_SideMargin, b_MoreData.x)),
            u_HandleRadius
        );
    }

    o_Color.a = min(railRectangle + handleCircle, o_Color.a);
    o_Color.rgb -= vec3(railRectangle * 0.1 * (1.0 - handleCircle));
}
#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_HandleRadius;
uniform float u_RailThickness;
uniform float u_SideMargin;

float positiveCircle(vec2 center, float radius)
{
    float dist = distance(b_Pos, center);
    float delta = fwidth(dist);
    return 1.0 - smoothstep(radius - delta / 2.0, radius + delta / 2.0, dist);
}

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
    return 1.0 - clamp(udRoundBox(p, rectSize / 2.0, 3.0), 0.0, 1.0);
}

void main()
{
    vec2 topLeftQuadPos = b_Data.rg;
    vec2 bottomRightQuadPos = b_Data.ba;

    o_Color.rgba = b_Color.rgba;

    float railRectangle;
    float handleCircle;

    // horizontal slider
    if (b_MoreData.y == 0.0)
    {
        float middleRailPosY = (topLeftQuadPos.y + bottomRightQuadPos.y) / 2.0;

        railRectangle = positiveRoundedRectangle(
            vec2(topLeftQuadPos.x + u_HandleRadius + u_SideMargin, middleRailPosY - u_RailThickness / 2.0),
            vec2(bottomRightQuadPos.x - u_HandleRadius - u_SideMargin, middleRailPosY + u_RailThickness / 2.0));

        handleCircle = positiveCircle(
            vec2(mix(topLeftQuadPos.x + u_HandleRadius + u_SideMargin, bottomRightQuadPos.x - u_HandleRadius - u_SideMargin, b_MoreData.x), middleRailPosY),
            u_HandleRadius
        );
    }
    // vertical slider
    else
    {
        float middleRailPosX = (topLeftQuadPos.x + bottomRightQuadPos.x) / 2.0;


        railRectangle = positiveRoundedRectangle(
            vec2(middleRailPosX - u_RailThickness / 2.0, topLeftQuadPos.y + u_HandleRadius + u_SideMargin),
            vec2(middleRailPosX + u_RailThickness / 2.0, bottomRightQuadPos.y - u_HandleRadius - u_SideMargin));

        handleCircle = positiveCircle(
            vec2(middleRailPosX, mix(topLeftQuadPos.y + u_HandleRadius + u_SideMargin, bottomRightQuadPos.y - u_HandleRadius - u_SideMargin, b_MoreData.x)),
            u_HandleRadius
        );
    }

    o_Color.a = min(railRectangle + handleCircle, o_Color.a);
    o_Color.rgb -= vec3(railRectangle * 0.1 * (1.0 - handleCircle));
}
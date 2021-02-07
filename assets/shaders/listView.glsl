#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Radius;
uniform vec2 u_SelectionTopLeft;
uniform vec2 u_SelectionBottomRight;

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
    return 1.0 - clamp(udRoundBox(p, rectSize / 2.0, u_Radius), 0.0, 1.0);
}

void main()
{
	o_Color = b_Color;
    float roundedRect = positiveRoundedRectangle(u_SelectionTopLeft, u_SelectionBottomRight);
	o_Color.rgb += roundedRect * 0.1;
}
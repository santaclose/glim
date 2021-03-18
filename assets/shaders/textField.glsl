#version 460 core

layout(location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_Margin;
uniform float u_CornerRadius;
uniform float u_CursorThickness;

float udRoundBox(vec2 p, vec2 b, float r)
{
    return length(max(abs(p) - b + r, 0.0)) - r;
}

float positiveRectangle(vec2 topLeftCorner, vec2 bottomRightCorner)
{
    return float(b_Pos.x > topLeftCorner.x && b_Pos.y > topLeftCorner.y && b_Pos.x < bottomRightCorner.x&& b_Pos.y < bottomRightCorner.y);
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

    o_Color.a *= positiveRoundedRectangle(topLeftCorner, bottomRightCorner);

    // add cursor if necessary
    if (b_MoreData.x == 0.0)
        return;

    vec2 cursorTopLeft = vec2(topLeftCorner.x + u_Margin + b_MoreData.y, topLeftCorner.y + 3.0);
    vec2 cursorBottomRight = vec2(topLeftCorner.x + u_Margin + b_MoreData.y + u_CursorThickness, bottomRightCorner.y - 3.0);

    float cursorRectangle = positiveRectangle(cursorTopLeft, cursorBottomRight);
    // brighter if highlighted
	o_Color.rgb += vec3(cursorRectangle * 0.2);
    // darker if can't be brighter
    o_Color.rgb -= vec3(cursorRectangle * float(o_Color.r > 1.0 || o_Color.g  > 1.0 || o_Color.b > 1.0) * 0.4);
}
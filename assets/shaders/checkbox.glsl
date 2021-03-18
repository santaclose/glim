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

float negativeCircle(vec2 center, float radius)
{
    float size = b_Data.b - b_Data.r;
	vec2 tCenter = vec2(center.x * size + b_Data.r, center.y * size + b_Data.g);
	float tRadius = radius * size;

	float dist = distance(b_Pos, tCenter);
	float delta = fwidth(dist);
	return smoothstep(tRadius - delta / 2.0, tRadius + delta / 2.0, dist);
}
float positiveCircle(vec2 center, float radius)
{
    float size = b_Data.b - b_Data.r;
	vec2 tCenter = vec2(center.x * size + b_Data.r, center.y * size + b_Data.g);
	float tRadius = radius * size;

	float dist = distance(b_Pos, tCenter);
	float delta = fwidth(dist);
	return 1.0 - smoothstep(tRadius - delta / 2.0, tRadius + delta / 2.0, dist);
}

void main()
{
    o_Color.rgba = b_Color.rgba;

    vec2 topLeftCorner = b_Data.rg + vec2(1.0, 1.0);
    vec2 bottomRightCorner = b_Data.ba - vec2(1.0, 1.0);

    float roundedBoxMultiplier = positiveRoundedRectangle(topLeftCorner, bottomRightCorner);

    o_Color.a *= roundedBoxMultiplier;

    float checkmark = positiveCircle(vec2(-3.2979,-4.5699), 6.488) * positiveCircle(vec2(3.2392,-2.3052), 4.1808);
    float temp = positiveCircle(vec2(-3.3864,-4.6536), 6.5205) * positiveCircle(vec2(3.337,-2.3256), 4.1808);
    checkmark *= 1.0 - temp;
    checkmark *= negativeCircle(vec2(3.8863,-2.5716), 4.1808);
    checkmark *= negativeCircle(vec2(-2.3005,-2.8116),4.1808);
    //float checkmark = positiveCircle(vec2(0.5,0.5), 0.2);

    // add checkmark if value is true
    roundedBoxMultiplier = b_MoreData.x * checkmark;

    o_Color.a *= 1.0 - roundedBoxMultiplier;
}
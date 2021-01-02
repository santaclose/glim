#version 460 core

layout(location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

uniform float u_GroupData[_GROUP_DATA_COUNT_];
uniform float u_Data[_DATA_FLOAT_COUNT_];
uniform int u_GroupCount;
uniform int u_CirclesPerGroup;

float negativeCircle(vec2 center, float radius)
{
	vec2 tCenter = vec2(center.x * b_Data[2] + b_Data[0], center.y * b_Data[2] + b_Data[1]);
	float tRadius = radius * b_Data[2];

	float dist = distance(b_Pos, tCenter);
	float delta = fwidth(dist);
	return smoothstep(tRadius - delta / 2.0, tRadius + delta / 2.0, dist);
}
float positiveCircle(vec2 center, float radius)
{
	vec2 tCenter = vec2(center.x * b_Data[2] + b_Data[0], center.y * b_Data[2] + b_Data[1]);
	float tRadius = radius * b_Data[2];

	float dist = distance(b_Pos, tCenter);
	float delta = fwidth(dist);
	return 1.0 - smoothstep(tRadius - delta / 2.0, tRadius + delta / 2.0, dist);
}

void main()
{
	int floatsPerGlyph = (u_GroupCount * u_CirclesPerGroup) * 4;
	int glyphID = int(b_Data[3]);

	o_Color = b_Color;
	float finalMultiplier = 0.0;
	for (int i = 0; i < u_GroupCount; i++)
	{
		float groupResult = 0.0;
		for (int j = 0; j < u_CirclesPerGroup; j++)
		{
			int targetIndex = (i * u_CirclesPerGroup + j) * 4 + floatsPerGlyph * glyphID;

			groupResult =
				(groupResult + positiveCircle(vec2(u_Data[targetIndex + 1], u_Data[targetIndex + 2]), u_Data[targetIndex + 3])) *
					float(u_Data[targetIndex] == 0.0) +
				(groupResult * negativeCircle(vec2(u_Data[targetIndex + 1], u_Data[targetIndex + 2]), u_Data[targetIndex + 3])) *
					float(u_Data[targetIndex] == 1.0) +
				(groupResult * positiveCircle(vec2(u_Data[targetIndex + 1], u_Data[targetIndex + 2]), u_Data[targetIndex + 3])) *
					float(u_Data[targetIndex] == 2.0);
			groupResult = min(groupResult, 1.0);
		}
		finalMultiplier =
			(finalMultiplier + groupResult) *			float(u_GroupData[u_GroupCount * glyphID + i] == 0.0) +
			(finalMultiplier * (1.0 - groupResult)) *	float(u_GroupData[u_GroupCount * glyphID + i] == 1.0) +
			(finalMultiplier * groupResult) *			float(u_GroupData[u_GroupCount * glyphID + i] == 2.0);
		finalMultiplier = min(finalMultiplier, 1.0);
	}

	o_Color.a *= finalMultiplier;
}
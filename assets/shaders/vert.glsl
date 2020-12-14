#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec4 a_Data;
layout (location = 3) in vec4 a_MoreData;

out vec2 b_Pos;
out vec4 b_Color;
out vec4 b_Data;
out vec4 b_MoreData;

uniform mat4 u_Mat;

void main()
{
	gl_Position = u_Mat * vec4(a_Position, 0.0f, 1.0f);
	b_Pos = a_Position;
	b_Color = a_Color;
	b_Data = a_Data;
	b_MoreData = a_MoreData;
}
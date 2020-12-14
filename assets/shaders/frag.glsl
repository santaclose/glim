#version 460 core

layout (location = 0) out vec4 o_Color;

in vec2 b_Pos;
in vec4 b_Color;
in vec4 b_Data;
in vec4 b_MoreData;

void main()
{
	o_Color = b_Color;
}
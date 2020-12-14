#include "FloatingButton.h"
#include "Geometry.h"
#include "Shader.h"
#include "Input.h"
#include "CircleFont.h"
#include <iostream>
#include <vector>

#define BUTTON_QUAD_MARGIN 3.0f
#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }

namespace Glim::FloatingButton {

	struct ButtonInfo {

		uint32_t geometryIndex;
		Corner corner;
		float size;
		glm::vec2 pos;
	};

	int currentButtonID = 0;

	std::vector<ButtonInfo> buttonInfo;

	const uint32_t* windowSize;
	float margin;

	// area used to highlight buttons
	glm::vec3 highlightCircle = { -1.0f, -1.0f, 0.0f };

	Shader shader;
	Geometry quads;

	IconSource iconSource;
	CircleFont circleFont;

	// assume one button per corner
	std::unordered_map<Corner, int> corner2buttonIndex;
	std::unordered_map<int, int> buttonIndex2iconIndex;

	void Add(Corner corner, float size);
	bool CollisionTest(int buttonIndex);
	void ComputeQuadPos(int buttonIndex);

	bool thereWasACollision = false;
}

bool Glim::FloatingButton::CollisionTest(int buttonIndex)
{
	return glm::distance(
		{ Glim::Input::mousePos[0], Glim::Input::mousePos[1] },
		buttonInfo[buttonIndex].pos + glm::vec2(buttonInfo[buttonIndex].size / 2.0f, buttonInfo[buttonIndex].size / 2.0f))
		< buttonInfo[buttonIndex].size / 2.0f - BUTTON_QUAD_MARGIN / 2.0f;
}

void Glim::FloatingButton::ComputeQuadPos(int buttonIndex)
{
	switch (buttonInfo[buttonIndex].corner)
	{
	case Corner::BottomLeft:
		buttonInfo[buttonIndex].pos.x = margin;
		buttonInfo[buttonIndex].pos.y = windowSize[1] - (margin + buttonInfo[buttonIndex].size);
		break;
	case Corner::BottomRight:
		buttonInfo[buttonIndex].pos.x = windowSize[0] - (margin + buttonInfo[buttonIndex].size);
		buttonInfo[buttonIndex].pos.y = windowSize[1] - (margin + buttonInfo[buttonIndex].size);
		break;
	case Corner::TopLeft:
		buttonInfo[buttonIndex].pos.x = margin;
		buttonInfo[buttonIndex].pos.y = margin;
		break;
	case Corner::TopRight:
		buttonInfo[buttonIndex].pos.x = windowSize[0] - (margin + buttonInfo[buttonIndex].size);
		buttonInfo[buttonIndex].pos.y = margin;
		break;
	default:
		std::cout << "[Floating Button] Invalid corner passed as argument\n";
		break;
	}
}

void Glim::FloatingButton::Init(const uint32_t* windowSize, float margin, IconSource iconSource, const std::string& iconsPath)
{
	if (iconSource == IconSource::CircleFont)
		circleFont.CreateFromFile(iconsPath);

	FloatingButton::iconSource = iconSource;
	FloatingButton::windowSize = windowSize;
	FloatingButton::margin = margin;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/floatingButton.glsl");
	shader.Bind();
	shader.SetUniform1f("u_Margin", BUTTON_QUAD_MARGIN);
	shader.SetUniform3fv("u_HighlightCircle", &highlightCircle.x);
	quads.CreateFromShader(&shader);
}

void Glim::FloatingButton::OnResize()
{
	for (int i = 0; i < buttonInfo.size(); i++)
	{
		ComputeQuadPos(i);
		quads.UpdateQuadVertexCoords(
			buttonInfo[i].geometryIndex,
			buttonInfo[i].pos,
			{ buttonInfo[i].size, buttonInfo[i].size }
		);
		quads.UpdateQuadData(
			buttonInfo[i].geometryIndex,
			{ buttonInfo[i].pos.x, buttonInfo[i].pos.y, buttonInfo[i].size, 0.0f },
			{ 0.0, 0.0, 0.0, 0.0 }
		);
		
		circleFont.UpdatePosition(buttonIndex2iconIndex[i], buttonInfo[i].pos);
	}
}

void Glim::FloatingButton::FrameBegin()
{
	if (!thereWasACollision)
	{
		highlightCircle = { -1.0f, -1.0f, 0.0f };
		shader.Bind();
		shader.SetUniform3fv("u_HighlightCircle", &highlightCircle.x);
	}
	thereWasACollision = false;
}

void Glim::FloatingButton::FrameEnd()
{
	currentButtonID = 0;
}

void Glim::FloatingButton::Add(Corner corner, float size)
{
	buttonInfo.emplace_back();
	buttonInfo.back().size = size;
	buttonInfo.back().corner = corner;

	ComputeQuadPos(buttonInfo.size() - 1);

	buttonInfo.back().geometryIndex =
		quads.CreateQuad(
			buttonInfo.back().pos,
			{ buttonInfo.back().size, buttonInfo.back().size },
			COLOR,
			{ buttonInfo.back().pos.x, buttonInfo.back().pos.y, buttonInfo.back().size, 0.0f });

	corner2buttonIndex[corner] = buttonInfo.size() - 1;
	if (iconSource == IconSource::CircleFont)
		buttonIndex2iconIndex[buttonInfo.size() - 1] = circleFont.Add(0, buttonInfo.back().pos, buttonInfo.back().size, {1.0, 1.0, 1.0, 1.0});
}

bool Glim::FloatingButton::Evaluate(Corner corner, float size, int iconID)
{
	if (corner2buttonIndex.find(corner) == corner2buttonIndex.end()) // button not added yet
		Add(corner, size);

	int buttonIndex = corner2buttonIndex[corner];

	if (iconSource == IconSource::CircleFont)
	{
		int iconIndex = buttonIndex2iconIndex[buttonIndex];
		circleFont.ChangeGlyph(iconIndex, iconID);
	}

	bool collided = CollisionTest(buttonIndex);
	if (collided)
	{
		highlightCircle = {
			buttonInfo[buttonIndex].pos.x + buttonInfo[buttonIndex].size / 2.0f,
			buttonInfo[buttonIndex].pos.y + buttonInfo[buttonIndex].size / 2.0f,
			buttonInfo[buttonIndex].size / 2.0f,
		};
		shader.Bind();
		shader.SetUniform3fv("u_HighlightCircle", &highlightCircle.x);
		thereWasACollision = true;
	}

	currentButtonID++;
	return collided && Glim::Input::MouseButtonDown(0);
}

glm::vec2 Glim::FloatingButton::GetLastButtonPosition()
{
	return buttonInfo[currentButtonID - 1].pos + glm::vec2(buttonInfo[currentButtonID - 1].size / 2.0f);
}

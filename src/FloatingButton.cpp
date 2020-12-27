#include "FloatingButton.h"
#include "Geometry.h"
#include "Shader.h"
#include "Input.h"
#include "CircleFont.h"
#include <iostream>
#include <vector>

#define BUTTON_QUAD_MARGIN 3.0f
#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }
#define ICON_COLOR { 1.0f, 1.0f, 1.0f, 1.0f }

namespace Glim::FloatingButton {

	struct ButtonInfo {

		uint32_t geometryIndex;
		glm::vec2 pos;
		float size;
		int iconIndex = -1;
	};

	int currentButtonID = 0;
	std::vector<ButtonInfo> buttons;

	const uint32_t* windowSize;
	Shader shader;
	Geometry quads;
	IconSource iconSource;
	CircleFont circleFont;

	// area used to highlight buttons
	glm::vec3 highlightCircle = { -1.0f, -1.0f, 0.0f };

	bool CollisionTest(int buttonIndex);

	bool thereWasACollision = false;
}

bool Glim::FloatingButton::CollisionTest(int buttonIndex)
{
	return glm::distance(
		{ Glim::Input::mousePos[0], Glim::Input::mousePos[1] },
		buttons[buttonIndex].pos + glm::vec2(buttons[buttonIndex].size / 2.0f, buttons[buttonIndex].size / 2.0f))
		< buttons[buttonIndex].size / 2.0f - BUTTON_QUAD_MARGIN / 2.0f;
}

void Glim::FloatingButton::Init(const uint32_t* windowSize, IconSource iconSource, const std::string& iconsPath)
{
	if (iconSource == IconSource::CircleFont)
		circleFont.CreateFromFile(iconsPath);

	FloatingButton::iconSource = iconSource;
	FloatingButton::windowSize = windowSize;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/floatingButton.glsl");
	shader.Bind();
	shader.SetUniform1f("u_Margin", BUTTON_QUAD_MARGIN);
	shader.SetUniform3fv("u_HighlightCircle", &highlightCircle.x);
	quads.CreateFromShader(&shader);
}

bool Glim::FloatingButton::Evaluate(const glm::vec2& position, float size, int iconID)
{
	if (currentButtonID == buttons.size())
	{
		buttons.emplace_back();
		buttons.back().geometryIndex = quads.CreateQuad();
		if (iconSource == IconSource::CircleFont)
			buttons.back().iconIndex = circleFont.Add(0, position, size, ICON_COLOR);
	}

	buttons[currentButtonID].pos = position;
	buttons[currentButtonID].size = size;
	if (iconSource == IconSource::CircleFont)
	{
		circleFont.ChangeGlyph(buttons[currentButtonID].iconIndex, iconID);
		circleFont.UpdatePosition(buttons[currentButtonID].iconIndex, position);
		circleFont.UpdateSize(buttons[currentButtonID].iconIndex, size);
		circleFont.SetGlyphColor(buttons[currentButtonID].iconIndex, ICON_COLOR);
	}

	quads.UpdateQuadVertexCoords(buttons[currentButtonID].geometryIndex, buttons[currentButtonID].pos, { size, size });
	quads.UpdateQuadColor(buttons[currentButtonID].geometryIndex, COLOR);

	bool collided = CollisionTest(currentButtonID);
	if (collided)
	{
		highlightCircle = {
			buttons[currentButtonID].pos.x + buttons[currentButtonID].size / 2.0f,
			buttons[currentButtonID].pos.y + buttons[currentButtonID].size / 2.0f,
			buttons[currentButtonID].size / 2.0f,
		};
		shader.Bind();
		shader.SetUniform3fv("u_HighlightCircle", &highlightCircle.x);
		thereWasACollision = true;
	}

	quads.UpdateQuadData(buttons[currentButtonID].geometryIndex,
		{ buttons[currentButtonID].pos.x, buttons[currentButtonID].pos.y, buttons[currentButtonID].size, 0.0f });

	currentButtonID++;
	if (collided && Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		Input::cursorCollisionDetected = true;
		return true;
	}
	return false;
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

void Glim::FloatingButton::BeforeDraw()
{
	// hide all unused quads
	while (currentButtonID < buttons.size())
	{
		quads.UpdateQuadColor(buttons[currentButtonID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		circleFont.SetGlyphColor(buttons[currentButtonID].iconIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		currentButtonID++;
	}
}

void Glim::FloatingButton::FrameEnd()
{
	currentButtonID = 0;
}
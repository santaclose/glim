#include "Slider.h"
#include "Geometry.h"
#include "Shader.h"
#include "Input.h"
#include <iostream>

#define SIDE_MARGIN 1.0f
#define RAIL_THICKNESS 3.0f
#define HANDLE_RADIUS 8.0f
#define QUAD_WIDTH 25.0f

#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }

namespace Glim::Slider {

	struct SliderInfo {

		uint32_t geometryIndex;
		glm::vec2 pos;
		Orientation orientation;
		float size;
		float* value;
	};

	int currentSliderID = 0;
	std::vector<SliderInfo> sliders;

	const uint32_t* windowSize;
	Shader shader;
	Geometry quads;

	int currentlyDraggingSlider = -1;
	float draggingOffset;

	bool CollisionTest(int sliderID);
}

bool Glim::Slider::CollisionTest(int sliderID)
{
	glm::vec2 circleCenter;
	if (sliders[sliderID].orientation == Orientation::Horizontal)
	{
		circleCenter.x = glm::mix(
			sliders[sliderID].pos.x + HANDLE_RADIUS + SIDE_MARGIN,
			sliders[sliderID].pos.x + sliders[sliderID].size - HANDLE_RADIUS - SIDE_MARGIN,
			*sliders[sliderID].value
		);
		circleCenter.y = sliders[sliderID].pos.y + QUAD_WIDTH / 2.0f;
		return glm::distance(circleCenter, *((glm::vec2*)&(Glim::Input::mousePos[0]))) < HANDLE_RADIUS;
	}
	else
	{
		circleCenter.x = sliders[sliderID].pos.x + QUAD_WIDTH / 2.0f;
		circleCenter.y = glm::mix(
			sliders[sliderID].pos.y + HANDLE_RADIUS + SIDE_MARGIN,
			sliders[sliderID].pos.y + sliders[sliderID].size - HANDLE_RADIUS - SIDE_MARGIN,
			*sliders[sliderID].value
		);
		return glm::distance(circleCenter, *((glm::vec2*)&(Glim::Input::mousePos[0]))) < HANDLE_RADIUS;
	}
}

void Glim::Slider::Init(const uint32_t* windowSize)
{
	Slider::windowSize = windowSize;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/slider.glsl");
	shader.Bind();
	shader.SetUniform1f("u_HandleRadius", HANDLE_RADIUS);
	shader.SetUniform1f("u_RailThickness", RAIL_THICKNESS);
	shader.SetUniform1f("u_SideMargin", SIDE_MARGIN);

	quads.CreateFromShader(&shader);
}

void Glim::Slider::Evaluate(const glm::vec2& position, float size, float* value, Orientation orientation)
{
	if (currentSliderID == sliders.size())
	{
		sliders.emplace_back();
		sliders.back().geometryIndex = quads.CreateQuad();
	}

	sliders[currentSliderID].pos = position;
	sliders[currentSliderID].value = value;
	sliders[currentSliderID].size = size;
	sliders[currentSliderID].orientation = orientation;

	if (orientation == Orientation::Horizontal)
		quads.UpdateQuadVertexCoords(sliders[currentSliderID].geometryIndex, sliders[currentSliderID].pos, { size, QUAD_WIDTH });
	else
		quads.UpdateQuadVertexCoords(sliders[currentSliderID].geometryIndex, sliders[currentSliderID].pos, { QUAD_WIDTH, size });

	quads.UpdateQuadColor(sliders[currentSliderID].geometryIndex, COLOR);

	float minPos;
	float maxPos;
	if (sliders[currentSliderID].orientation == Orientation::Horizontal)
	{
		minPos = sliders[currentSliderID].pos.x + HANDLE_RADIUS;
		maxPos = sliders[currentSliderID].pos.x + sliders[currentSliderID].size - HANDLE_RADIUS;
	}
	else
	{
		minPos = sliders[currentSliderID].pos.y + HANDLE_RADIUS;
		maxPos = sliders[currentSliderID].pos.y + sliders[currentSliderID].size - HANDLE_RADIUS;
	}

	if (CollisionTest(currentSliderID) && Input::MouseButtonDown(0) && !Input::cursorCollisionDetected)
	{
		Input::cursorCollisionDetected = true;

		currentlyDraggingSlider = currentSliderID;

		float valueFromMousePos = sliders[currentSliderID].orientation == Orientation::Horizontal ?
			(Input::mousePos[0] - minPos) / (maxPos - minPos) :
			(Input::mousePos[1] - minPos) / (maxPos - minPos);
		draggingOffset = *sliders[currentSliderID].value - valueFromMousePos;
	}

	if (currentlyDraggingSlider == currentSliderID)
	{
		float valueFromMousePos = sliders[currentSliderID].orientation == Orientation::Horizontal ?
			(Input::mousePos[0] - minPos) / (maxPos - minPos) :
			(Input::mousePos[1] - minPos) / (maxPos - minPos);

		*sliders[currentSliderID].value = glm::clamp(valueFromMousePos + draggingOffset, 0.0f, 1.0f);

		if (Input::MouseButtonUp(0))
			currentlyDraggingSlider = -1;
	}

	if (orientation == Orientation::Horizontal)
		quads.UpdateQuadData(sliders[currentSliderID].geometryIndex,
			{ sliders[currentSliderID].pos.x, sliders[currentSliderID].pos.y,
			  sliders[currentSliderID].pos.x + size, sliders[currentSliderID].pos.y + QUAD_WIDTH },
			{ *sliders[currentSliderID].value, 0.0f, 0.0f, 0.0f });
	else
		quads.UpdateQuadData(sliders[currentSliderID].geometryIndex,
			{ sliders[currentSliderID].pos.x, sliders[currentSliderID].pos.y,
			  sliders[currentSliderID].pos.x + QUAD_WIDTH, sliders[currentSliderID].pos.y + size },
			{ *sliders[currentSliderID].value, 1.0f, 0.0f, 0.0f });

	currentSliderID++;
	return;
}

float Glim::Slider::GetWidth()
{
	return QUAD_WIDTH;
}

void Glim::Slider::BeforeDraw()
{
	// hide all the unused sliders
	while (currentSliderID < sliders.size())
	{
		quads.UpdateQuadColor(sliders[currentSliderID].geometryIndex, { 0.0f, 0.0f, 0.0f, 0.0f });
		currentSliderID++;
	}
}

void Glim::Slider::FrameEnd()
{
	currentSliderID = 0;
}

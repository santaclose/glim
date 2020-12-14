#include "Slider.h"
#include "Geometry.h"
#include "Shader.h"
#include "Input.h"
#include <iostream>

#define RAIL_THICKNESS 3.0f
#define HANDLE_RADIUS 8.0f
#define QUAD_HEIGHT 25.0f

#define COLOR { 0.35f, 0.35f, 0.35f, 0.9f }

namespace Glim::Slider {

	struct SliderInfo {

		uint32_t geometryIndex;
		glm::vec2 pos;
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
	circleCenter.x = glm::mix(
		sliders[sliderID].pos.x + HANDLE_RADIUS,
		sliders[sliderID].pos.x + sliders[sliderID].size - HANDLE_RADIUS,
		*sliders[sliderID].value
		);
	circleCenter.y = sliders[sliderID].pos.y + QUAD_HEIGHT / 2.0f;
	return glm::distance(circleCenter, *((glm::vec2*)&(Glim::Input::mousePos[0]))) < HANDLE_RADIUS;
}

void Glim::Slider::Init(const uint32_t* windowSize)
{
	Slider::windowSize = windowSize;

	shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/slider.glsl");
	shader.Bind();
	shader.SetUniform1f("u_HandleRadius", HANDLE_RADIUS);
	shader.SetUniform1f("u_RailThickness", RAIL_THICKNESS);

	quads.CreateFromShader(&shader);
}

void Glim::Slider::OnResize()
{

}

void Glim::Slider::Evaluate(const glm::vec2& position, float sizeInPixels, float* value)
{
	if (currentSliderID == sliders.size())
	{
		sliders.emplace_back();
		sliders.back().value = value;
		sliders.back().geometryIndex =
			quads.CreateQuad(
				{ 0.0f, 0.0f },
				{ 0.0f, 0.0f },
				COLOR
				);
	}

	sliders[currentSliderID].pos = position - glm::vec2(0.0, QUAD_HEIGHT / 2.0);
	sliders[currentSliderID].size = sizeInPixels;
	quads.UpdateQuadPos(sliders[currentSliderID].geometryIndex, sliders[currentSliderID].pos, { sizeInPixels, QUAD_HEIGHT });

	float minPos = sliders[currentSliderID].pos.x + HANDLE_RADIUS;
	float maxPos = sliders[currentSliderID].pos.x + sliders[currentSliderID].size - HANDLE_RADIUS;
	if (CollisionTest(currentSliderID) && Input::MouseButtonDown(0))
	{
		currentlyDraggingSlider = currentSliderID;

		float valueFromMousePos = (Input::mousePos[0] - minPos) / (maxPos - minPos);
		draggingOffset = *sliders[currentSliderID].value - valueFromMousePos;
	}

	if (currentlyDraggingSlider == currentSliderID)
	{
		float valueFromMousePos = (Input::mousePos[0] - minPos) / (maxPos - minPos);

		*sliders[currentSliderID].value = glm::clamp(valueFromMousePos + draggingOffset, 0.0f, 1.0f);

		if (Input::MouseButtonUp(0))
			currentlyDraggingSlider = -1;
	}

	quads.UpdateQuadData(sliders[currentSliderID].geometryIndex,
		{ sliders[currentSliderID].pos.x, sliders[currentSliderID].pos.y + QUAD_HEIGHT,
		  sliders[currentSliderID].pos.x + sizeInPixels, sliders[currentSliderID].pos.y },
		{ *sliders[currentSliderID].value, 0.0f, 0.0f, 0.0f }
		);

	currentSliderID++;
	return;
}

void Glim::Slider::FrameEnd()
{
	currentSliderID = 0;
}

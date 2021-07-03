#pragma once

#include <Elements/ButtonLayer.h>
#include <Elements/CheckboxLayer.h>
#include <Elements/ListViewLayer.h>
#include <Elements/SelectionBox.h>
#include <Elements/SliderLayer.h>
#include <Elements/TextFieldLayer.h>
#include <Elements/TextLayer.h>
#include <CircleFont.h>
#include <Drawable.h>
#include <Enums.h>
#include <FontManager.h>
#include <Geometry.h>
#include <Input.h>
#include <LayerRenderer.h>
#include <Shader.h>
#include <Vertex.h>
#include <GlobalState.h>

namespace Glim {

	void Init();
	void UpdateWindowSize(const uint32_t* windowSize);
	void FrameBegin();
	void Render();
	void FrameEnd();
	void Terminate();
}
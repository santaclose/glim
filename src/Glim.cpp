#include "Glim.h"

void Glim::Init()
{
	FontManager::Init();
}

void Glim::UpdateWindowSize(const uint32_t* windowSize)
{
	Shader::UpdateMatrix(windowSize);
	TextLayer::OnResize(windowSize);
}

void Glim::FrameBegin()
{
	for (ButtonLayer* instance : ButtonLayer::instances) instance->FrameBegin();
	for (ListViewLayer* instance : ListViewLayer::instances) instance->FrameBegin();
}

void Glim::Render()
{
	for (ButtonLayer* instance : ButtonLayer::instances) instance->BeforeDraw();
	for (CheckboxLayer* instance : CheckboxLayer::instances) instance->BeforeDraw();
	for (ListViewLayer* instance : ListViewLayer::instances) instance->BeforeDraw();
	for (SliderLayer* instance : SliderLayer::instances) instance->BeforeDraw();

	LayerRenderer::Render();
}

void Glim::FrameEnd()
{
	GlobalState::FrameEnd();
	Input::FrameEnd();

	for (ButtonLayer* instance : ButtonLayer::instances) instance->FrameEnd();
	for (CheckboxLayer* instance : CheckboxLayer::instances) instance->FrameEnd();
	for (ListViewLayer* instance : ListViewLayer::instances) instance->FrameEnd();
	for (SliderLayer* instance : SliderLayer::instances) instance->FrameEnd();
	for (TextFieldLayer* instance : TextFieldLayer::instances) instance->FrameEnd();
	for (TextLayer* instance : TextLayer::instances) instance->FrameEnd();
}

void Glim::Terminate()
{
	for (ButtonLayer* instance : ButtonLayer::instances) delete instance;
	for (CheckboxLayer* instance : CheckboxLayer::instances) delete instance;
	for (ListViewLayer* instance : ListViewLayer::instances) delete instance;
	for (SelectionBox* instance : SelectionBox::instances) delete instance;
	for (SliderLayer* instance : SliderLayer::instances) delete instance;
	for (TextFieldLayer* instance : TextFieldLayer::instances) delete instance;
	for (TextLayer* instance : TextLayer::instances) delete instance;

	FontManager::Destroy();
}

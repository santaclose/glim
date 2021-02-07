#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include "Input.h"
#include "Shader.h"
#include "Geometry.h"
#include "FontManager.h"
#include "CircleFont.h"
#include "LayerRenderer.h"
#include "Elements/TextLayer.h"
#include "Elements/ButtonLayer.h"
#include "Elements/SelectionBox.h"
#include "Elements/SliderLayer.h"
#include "Elements/CheckboxLayer.h"
#include "Elements/TextFieldLayer.h"
#include "Elements/ListViewLayer.h"

#define APPLICATION_WIDTH 1280
#define APPLICATION_HEIGHT 720

unsigned int appFont;

Glim::SelectionBox selectionBoxes;
Glim::ButtonLayer floatingButtons;
float sliderValue = 0.95;
Glim::SliderLayer sliders;
bool checkboxValue = false;
Glim::CheckboxLayer checkboxes;
Glim::TextLayer sampleText;

std::vector<std::string> listViewItems = { "asdf", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv", "fdsa", "zxcv" };
Glim::ListViewLayer listViewLayer;

#define TEXT_FIELD_BUFFER_SIZE 128
char textFieldBuffer[TEXT_FIELD_BUFFER_SIZE];
char textField2Buffer[TEXT_FIELD_BUFFER_SIZE];
char textField3Buffer[TEXT_FIELD_BUFFER_SIZE];
Glim::TextFieldLayer sampleTextField;

uint32_t windowSize[2] = { APPLICATION_WIDTH, APPLICATION_HEIGHT };

const std::vector<std::string> categoryOptions = { "0", "1", "2", "3" };
const std::vector<std::vector<std::string>> itemOptions = {
	{ "0a", "0b", "0c", "0d" },
	{ "1a", "1b", "1c", "1d" },
	{ "2a", "2b", "2c", "2d" },
	{ "3a", "3b", "3c", "3d" } };
const std::vector<std::string> fileSelectionBoxOptions =
{ "Open", "Save", "Import", "Export", "Exit", "Preferences" };

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowSize[0] = width;
	windowSize[1] = height;
	glViewport(0, 0, width, height);

	Glim::Shader::UpdateMatrix(windowSize);
	Glim::TextLayer::OnResize();
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Glim::Input::UpdateMousePosition(xpos, ypos);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
	Glim::Input::UpdateMouseButtons(button, action);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Glim::Input::UpdateKeyboard(key, action);
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	Glim::Input::UpdateCharacter(codepoint);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Glim::Input::UpdateMouseScroll(xoffset, yoffset);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MACOS
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(windowSize[0], windowSize[1], "glim demo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, windowSize[0], windowSize[1]);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Get GPU info and supported OpenGL version
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) << std::endl;

	// Monitor info
	/*GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	float xscale, yscale;
	glfwGetMonitorContentScale(monitor, &xscale, &yscale);
	float wxscale, wyscale;
	glfwGetWindowContentScale(window, &wxscale, &wyscale);
	std::cout << "Monitor content scale: " << xscale << ", " << yscale << '\n';
	std::cout << "Window content scale: " << wxscale << ", " << wyscale << '\n';*/

	// Glim::Init();

	Glim::FontManager::Init();
	appFont = Glim::FontManager::CreateFromFile("assets/fonts/Open_Sans/OpenSans-Regular.ttf");

	selectionBoxes.Init(windowSize);
	floatingButtons.Init(windowSize, Glim::ButtonLayer::IconSource::CircleFont, "assets/icons.cf");
	sliders.Init(windowSize);
	checkboxes.Init(windowSize);
	sampleText.Init(windowSize);
	sampleTextField.Init(windowSize);
	listViewLayer.Init(windowSize);

	memset(textFieldBuffer, 0, TEXT_FIELD_BUFFER_SIZE);
	memset(textField2Buffer, 0, TEXT_FIELD_BUFFER_SIZE);
	memset(textField3Buffer, 0, TEXT_FIELD_BUFFER_SIZE);
	textField2Buffer[0] = '9';

	int testSelectionBoxID = -1;
	int fileSelectionBoxID = -1;

	Glim::Shader::UpdateMatrix(windowSize);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		floatingButtons.FrameBegin();

		glClearColor(0.9, 0.9, 0.9, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// glim code ----------------

		// selection boxes

		if (testSelectionBoxID > -1)
		{
			int selection = selectionBoxes.Evaluate(testSelectionBoxID);
			switch (selection)
			{
			case Glim::SelectionBox::Selection::None:
				break;
			default:
				std::cout << "category selection: " << selection << std::endl;
				selectionBoxes.Delete(testSelectionBoxID);
				testSelectionBoxID = -1;
				break;
			}
		}

		// top left menu
		if (fileSelectionBoxID > -1)
		{
			int selection = selectionBoxes.Evaluate(fileSelectionBoxID);
			switch (selection)
			{
			case Glim::SelectionBox::Selection::None:
				break;
			case Glim::SelectionBox::Selection::Cancel:
				selectionBoxes.Delete(fileSelectionBoxID);
				fileSelectionBoxID = -1;
				break;
			default:
				std::cout << "file menu selection: " << selection << std::endl;
				if (selection == 4)
					glfwSetWindowShouldClose(window, 1);
				selectionBoxes.Delete(fileSelectionBoxID);
				fileSelectionBoxID = -1;
				break;
			}
		}

		// floating buttons
		if (floatingButtons.Evaluate({ 18.0f, 18.0f }, 65.0f, 1))
		{
			std::cout << "file button clicked\n";
			if (fileSelectionBoxID == -1)
				fileSelectionBoxID = selectionBoxes.Create(
					&fileSelectionBoxOptions, { 18.0f + 65.0f / 2.0f, 18.0f + 65.0f / 2.0f }, appFont);
		}
		if (floatingButtons.Evaluate({ windowSize[0] - 18.0f - 65.0f, windowSize[1] - 18.0f - 65.0f }, 65.0f, 0))
		{
			std::cout << "hierarchy button clicked\n";
			if (testSelectionBoxID == -1)
				testSelectionBoxID = selectionBoxes.Create(
					&categoryOptions, { windowSize[0] - 18.0f - 65.0f / 2.0f, windowSize[1] - 18.0f - 65.0f / 2.0f }, appFont, Glim::Corner::BottomRight);
		}

		sliders.Evaluate({ 50.0f - sliders.GetWidth() / 2.0, windowSize[1] / 2.0f - 150.0f }, 300.0f, &sliderValue, Glim::Orientation::Vertical);
		checkboxes.Evaluate({ 50.0f - checkboxes.GetSize() / 2.0, windowSize[1] / 2.0f + 150.0f }, &checkboxValue);
		//if (Glim::Input::Key(Glim::Input::KeyCode::Space))
		//	sliderValue = glm::sin(glfwGetTime()) * 0.5 + 0.5;

		sampleText.Element(
			"asdffdsa",
			{ windowSize[0] / 2.0f ,  0.0f },
			(1.0 - sliderValue) * 500.0f + 5.0f,
			appFont,
			0x000000ff,
			Glim::HAlignment::Center);

		sampleTextField.Evaluate({ windowSize[0] / 2.0f ,  windowSize[1] / 2.0f - 150.0f },
			textField3Buffer, TEXT_FIELD_BUFFER_SIZE, appFont, (1.0 - sliderValue) * 300.0f + 11.0f, Glim::HAlignment::Left);
		sampleTextField.Evaluate({ windowSize[0] / 2.0f ,  windowSize[1] / 2.0f + 150.0f },
			textFieldBuffer, TEXT_FIELD_BUFFER_SIZE, appFont, (1.0 - sliderValue) * 300.0f + 11.0f, Glim::HAlignment::Center);
		sampleTextField.Evaluate({ windowSize[0] / 2.0f ,  windowSize[1] / 2.0f },
			textField2Buffer, TEXT_FIELD_BUFFER_SIZE, appFont, (1.0 - sliderValue) * 300.0f + 11.0f, Glim::HAlignment::Right);

		listViewLayer.Evaluate({ windowSize[0] - 216.0f, windowSize[1] / 2.0f -  200.0f }, { 200.0, 400.0 }, &listViewItems, appFont);

		// glim code end ----------------

		//Glim::Render();
		sliders.BeforeDraw();
		floatingButtons.BeforeDraw();
		checkboxes.BeforeDraw();
		checkboxes.FrameEnd();
		Glim::LayerRenderer::Render();

		glfwSwapBuffers(window);

		Glim::Input::FrameEnd();
		sampleText.FrameEnd();
		sampleTextField.FrameEnd();
		listViewLayer.FrameEnd();
		floatingButtons.FrameEnd();
		selectionBoxes.FrameEnd();
		checkboxes.FrameEnd();
		sliders.FrameEnd();
		glfwWaitEvents();
	}

	//Glim::Terminate();
	sampleTextField.Destroy();
	Glim::FontManager::Destroy();
	glfwTerminate();
	return 0;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include "Input.h"
#include "Shader.h"
#include "Geometry.h"
#include "Text.h"
#include "FloatingButton.h"
#include "SelectionBox.h"
#include "Slider.h"
#include "Checkbox.h"
#include "CircleFont.h"

#define APPLICATION_WIDTH 1280
#define APPLICATION_HEIGHT 720

Glim::SelectionBox selectionBoxes;
Glim::FloatingButton floatingButtons;
Glim::Slider sliders;
Glim::Checkbox checkboxes;

uint32_t windowSize[2] = { APPLICATION_WIDTH, APPLICATION_HEIGHT };

const std::vector<std::string> categoryOptions = { "0", "1", "2", "3" };
const std::vector<std::vector<std::string>> itemOptions = {
	{ "0a", "0b", "0c", "0d" },
	{ "1a", "1b", "1c", "1d" },
	{ "2a", "2b", "2c", "2d" },
	{ "3a", "3b", "3c", "3d" } };
const std::vector<std::string> fileSelectionBoxOptions =
{ "Open", "Save", "Import", "Export", "Exit", "Preferences" };

float sliderValue = 0.5;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowSize[0] = width;
	windowSize[1] = height;
	glViewport(0, 0, width, height);

	Glim::Shader::UpdateMatrix(windowSize);
	Glim::Text::OnResize();
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Glim::Input::UpdateMousePosition(xpos, ypos);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
	Glim::Input::UpdateMouseButtons(button, action);
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

	Glim::Geometry::Init(windowSize);
	Glim::Text::Init(windowSize);
	Glim::Text::CreateFontFromFile("assets/fonts/FiraCode/FiraCode-Regular.ttf");

	selectionBoxes.Init(windowSize);
	floatingButtons.Init(windowSize, Glim::FloatingButton::IconSource::CircleFont, "assets/icons.cf");
	sliders.Init(windowSize);
	checkboxes.Init(windowSize);

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

		static float sliderBValue = 1.0;

		// floating buttons
		if (floatingButtons.Evaluate({ 18.0f, 18.0f }, sliderBValue * 65.0f, 1))
		{
			std::cout << "file button clicked\n";
			if (fileSelectionBoxID == -1)
				fileSelectionBoxID = selectionBoxes.Create(
					&fileSelectionBoxOptions, { 18.0f + 65.0f / 2.0f, 18.0f + 65.0f / 2.0f });
		}
		if (floatingButtons.Evaluate({ windowSize[0] - 18.0f - 65.0f, windowSize[1] - 18.0f - 65.0f }, 65.0f, 0))
		{
			std::cout << "hierarchy button clicked\n";
			if (testSelectionBoxID == -1)
				testSelectionBoxID = selectionBoxes.Create(
					&categoryOptions, { windowSize[0] - 18.0f - 65.0f / 2.0f, windowSize[1] - 18.0f - 65.0f / 2.0f }, Glim::Corner::BottomRight);
		}

		static bool lal = false, lel = true;
		checkboxes.Evaluate({ windowSize[0] / 2.0 - 150.0f - checkboxes.GetSize(), windowSize[1] - 70.0f - checkboxes.GetSize() / 2.0f }, &lal);
		checkboxes.Evaluate({ windowSize[0] / 2.0 - 150.0f - checkboxes.GetSize(), windowSize[1] - 120.0f - checkboxes.GetSize() / 2.0f }, &lel);
		if (lal)
		sliders.Evaluate({ windowSize[0] / 2.0f - 150.0f, windowSize[1] - 70.0f - sliders.GetWidth() / 2.0f }, 300, &sliderValue);
		if (lel)
		sliders.Evaluate({ windowSize[0] / 2.0f - 150.0f, windowSize[1] - 120.0f - sliders.GetWidth() / 2.0f }, 300, &sliderBValue);

		Glim::Text::Element(
			"The quick brown fox jumps over the lazy dog",
			{ windowSize[0] / 2.0f ,  windowSize[1] / 2.0f },
			sliderValue * 500.0f + 5.0f,
			0,
			0x000000ff,
			Glim::Alignment::Center);


		// glim code end ----------------

		//Glim::Render();
		sliders.BeforeDraw();
		floatingButtons.BeforeDraw();
		checkboxes.BeforeDraw();
		checkboxes.FrameEnd();
		Glim::Geometry::DrawAll();
		Glim::Text::DrawAll();

		glfwSwapBuffers(window);

		Glim::Input::FrameEnd();
		Glim::Text::FrameEnd();
		floatingButtons.FrameEnd();
		selectionBoxes.FrameEnd();
		checkboxes.FrameEnd();
		sliders.FrameEnd();
		glfwWaitEvents();
	}

	//Glim::Terminate();
	Glim::Text::Destroy();
	glfwTerminate();
	return 0;
}
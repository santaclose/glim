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
#include "CircleFont.h"

#define APPLICATION_WIDTH 1280
#define APPLICATION_HEIGHT 720

uint32_t windowSize[2] = { APPLICATION_WIDTH, APPLICATION_HEIGHT };

const std::vector<std::string> categoryOptions = { "0", "1", "2", "3" };
const std::vector<std::vector<std::string>> itemOptions = {
	{ "0a", "0b", "0c", "0d" },
	{ "1a", "1b", "1c", "1d" },
	{ "2a", "2b", "2c", "2d" },
	{ "3a", "3b", "3c", "3d" } };
const std::vector<std::string> fileSelectionBoxOptions =
{ "Open", "Save", "Import", "Export", "Exit", "Preferences" };

float sliderAValue = 0.5;
float sliderBValue = 0.75;

Glim::CircleFont testCircleFont;
unsigned int theGlyph;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowSize[0] = width;
	windowSize[1] = height;
	glViewport(0, 0, width, height);

	Glim::Shader::UpdateMatrix(windowSize);
	Glim::Text::OnResize();
	Glim::FloatingButton::OnResize();
	Glim::SelectionBox::OnResize();

	testCircleFont.UpdatePosition(theGlyph, { width / 2.0f, height / 2.0f });
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
	Glim::Text::Init("assets/fonts/Lato/Lato-Black.ttf", windowSize);
	Glim::SelectionBox::Init(windowSize);
	Glim::FloatingButton::Init(windowSize, 18.0f, Glim::FloatingButton::IconSource::CircleFont, "assets/icons.cf");
	Glim::Slider::Init(windowSize);

	testCircleFont.CreateFromFile("assets/icons.cf");
	theGlyph = testCircleFont.Add(0, { windowSize[0] / 2.0f - 120, windowSize[1] / 2.0f - 120 }, 240, {0.0, 0.0, 0.0, 1.0});

	int addNodeSelectionBoxID = -1;
	int addNodeSelectionBox2ID = -1;
	int fileSelectionBoxID = -1;

	Glim::Shader::UpdateMatrix(windowSize);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		Glim::FloatingButton::FrameBegin();

		glClearColor(0.9, 0.9, 0.9, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// glim code ----------------

		// selection boxes
		if (addNodeSelectionBox2ID > -1)
		{
			int selection = Glim::SelectionBox::Evaluate(addNodeSelectionBox2ID);
			if (selection > Glim::SelectionBox::Selection::None)
			{
				std::cout << "item selection: " << selection << std::endl;
				Glim::SelectionBox::Delete(addNodeSelectionBox2ID);
				addNodeSelectionBox2ID = -1;
			}
		}
		if (addNodeSelectionBoxID > -1)
		{
			int selection = Glim::SelectionBox::Evaluate(addNodeSelectionBoxID);
			switch (selection)
			{
			case Glim::SelectionBox::Selection::None:
				break;
			case Glim::SelectionBox::Selection::Cancel:
				std::cout << "category selection: " << selection << std::endl;
				Glim::SelectionBox::Delete(addNodeSelectionBoxID);
				addNodeSelectionBoxID = -1;
				break;
			default:
				std::cout << "category selection: " << selection << std::endl;
				if (addNodeSelectionBox2ID != -1)
					Glim::SelectionBox::Delete(addNodeSelectionBox2ID);

				addNodeSelectionBox2ID = Glim::SelectionBox::Create(
					&(itemOptions[selection]),
					Glim::SelectionBox::GetLastSelectionPosition(true),
					Glim::Corner::BottomRight);
				break;
			}
		}

		// top left menu
		if (fileSelectionBoxID > -1)
		{
			int selection = Glim::SelectionBox::Evaluate(fileSelectionBoxID);
			if (selection > -2)
			{
				std::cout << "file menu selection: " << selection << std::endl;
				if (selection == 4)
					glfwSetWindowShouldClose(window, 1);
				Glim::SelectionBox::Delete(fileSelectionBoxID);
				fileSelectionBoxID = -1;
			}
		}

		// floating buttons
		if (Glim::FloatingButton::Evaluate(Glim::Corner::BottomRight, 65.0f, 0))
		{
			std::cout << "hierarchy button clicked\n";
			if (addNodeSelectionBoxID == -1)
				addNodeSelectionBoxID = Glim::SelectionBox::Create(
					&categoryOptions, Glim::FloatingButton::GetLastButtonPosition(), Glim::Corner::BottomRight);
		}
		if (Glim::FloatingButton::Evaluate(Glim::Corner::TopLeft, 65.0f, 1))
		{
			std::cout << "file button clicked\n";
			if (fileSelectionBoxID == -1)
				fileSelectionBoxID = Glim::SelectionBox::Create(
					&fileSelectionBoxOptions, Glim::FloatingButton::GetLastButtonPosition());
		}

		Glim::Slider::Evaluate({ windowSize[0] / 2.0f - 150.0f, windowSize[1] - 70 }, 300, &sliderAValue);
		Glim::Slider::Evaluate({ windowSize[0] / 2.0f - 50.0f, windowSize[1] - 40 }, 100, &sliderBValue);
		{
			int glyphId = sliderBValue * (testCircleFont.GetGlyphCount() - 1);
			testCircleFont.ChangeGlyph(theGlyph, glyphId);
		}

		float newIconSize = sliderAValue * 1024.0f + 5.0f;
		testCircleFont.UpdatePosition(theGlyph, { windowSize[0] / 2.0 - newIconSize / 2.0, windowSize[1] / 2.0 - newIconSize / 2.0 });
		testCircleFont.UpdateSize(theGlyph, newIconSize);

		// glim code end ----------------

		//Glim::Render();
		Glim::Geometry::DrawAll();
		Glim::Text::DrawAll();

		glfwSwapBuffers(window);

		Glim::Input::FrameEnd();
		Glim::Text::FrameEnd();
		Glim::FloatingButton::FrameEnd();
		Glim::SelectionBox::FrameEnd();
		Glim::Slider::FrameEnd();
		glfwWaitEvents();
	}

	//Glim::Terminate();
	Glim::Text::Destroy();
	glfwTerminate();
	return 0;
}
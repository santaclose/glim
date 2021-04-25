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

#define APPLICATION_WIDTH 360
#define APPLICATION_HEIGHT 720
#define BAR_HEIGHT 48

#define MAIN_COLOR { 0, 194.0f/255.0f, 149.0f/255.0f, 1.0 }
#define SECOND_COLOR { 0.0f, 168.0f/255.0f, 128.0f/255.0f, 0.9 }

unsigned int appFont;
unsigned int barQuadId;

Glim::SelectionBox selectionBoxes;
Glim::ButtonLayer floatingButtons;
float sliderValue = 0.5;
Glim::SliderLayer sliders;

Glim::Geometry topBarQuad;

std::vector<std::string> listViewItems = { "asdf", "fdsa", "zxcv", "5655", "0000" };
Glim::ListViewLayer listViewLayer;

#define TEXT_FIELD_BUFFER_SIZE 32
char textFieldBuffer[TEXT_FIELD_BUFFER_SIZE];
Glim::TextFieldLayer textFieldLayer;

uint32_t windowSize[2] = { APPLICATION_WIDTH, APPLICATION_HEIGHT };

const std::vector<std::string> addOptions = { "Create Playlist" };
const std::vector<std::string> fileSelectionBoxOptions = { "Library", "Playlists", "Quit" };

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowSize[0] = width;
	windowSize[1] = height;
	glViewport(0, 0, width, height);

	Glim::Shader::UpdateMatrix(windowSize);
	Glim::TextLayer::OnResize();

	topBarQuad.UpdateQuadVertexCoords(barQuadId, { 0.0f, 0.0f }, { windowSize[0], BAR_HEIGHT });
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

	// Glim::Init();

	Glim::FontManager::Init();
	appFont = Glim::FontManager::CreateFromFile("assets/fonts/Open_Sans/OpenSans-Regular.ttf");

	selectionBoxes.Init(windowSize);
	floatingButtons.Init(windowSize, Glim::ButtonLayer::IconSource::CircleFont, "assets/icons.cf");
	sliders.Init(windowSize);
	textFieldLayer.Init(windowSize);

	Glim::Shader basicShader;
	basicShader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	topBarQuad.Init(&basicShader);
	barQuadId = topBarQuad.CreateQuad({ 0.0, 0.0 }, { windowSize[0], BAR_HEIGHT }, MAIN_COLOR);

	listViewLayer.Init(windowSize);

	memset(textFieldBuffer, 0, TEXT_FIELD_BUFFER_SIZE);

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
			case 0:
				listViewItems.push_back("asdf");
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
				if (selection == 2)
					glfwSetWindowShouldClose(window, 1);
				selectionBoxes.Delete(fileSelectionBoxID);
				fileSelectionBoxID = -1;
				break;
			}
		}

		// floating buttons
		float menuButtonSize = BAR_HEIGHT - 8.0;
		if (floatingButtons.Evaluate({ 8.0f, 4.0 }, menuButtonSize, 1, MAIN_COLOR))
		{
			std::cout << "file button clicked\n";
			if (fileSelectionBoxID == -1)
				fileSelectionBoxID = selectionBoxes.Create(
					&fileSelectionBoxOptions, { 8.0f + menuButtonSize / 2.0, 4.0 + menuButtonSize / 2.0 }, appFont, Glim::Corner::TopLeft, SECOND_COLOR);
		}
		if (floatingButtons.Evaluate({ windowSize[0] - 18.0f - 58.0f, windowSize[1] - 18.0f - 58.0f }, 58.0f, 0, MAIN_COLOR))
		{
			std::cout << "hierarchy button clicked\n";
			if (testSelectionBoxID == -1)
				testSelectionBoxID = selectionBoxes.Create(
					&addOptions, { windowSize[0] - 18.0f - 58.0f / 2.0f, windowSize[1] - 18.0f - 58.0f / 2.0f }, appFont, Glim::Corner::BottomRight, SECOND_COLOR);
		}

		sliders.Evaluate({ 20.0f - sliders.GetWidth() / 2.0, windowSize[1] / 2.0f - 150.0f }, 300.0f, &sliderValue, Glim::Orientation::Vertical, {0.95f, 0.95f, 0.95f, 1.0f});

		textFieldLayer.Evaluate({ windowSize[0] / 2.0f, BAR_HEIGHT / 2.0 },
			textFieldBuffer, TEXT_FIELD_BUFFER_SIZE, appFont, 14.0f, Glim::HAlignment::Center, Glim::VAlignment::Center, SECOND_COLOR);

		int asdf = listViewLayer.Evaluate({ 0.0, BAR_HEIGHT }, { windowSize[0], windowSize[1] - BAR_HEIGHT }, &listViewItems, appFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0x333333ff);
		if (asdf != -1)std::cout << asdf << std::endl;

		// glim code end ----------------

		//Glim::Render();
		sliders.BeforeDraw();
		floatingButtons.BeforeDraw();
		Glim::LayerRenderer::Render();

		glfwSwapBuffers(window);

		Glim::Input::FrameEnd();
		textFieldLayer.FrameEnd();
		listViewLayer.FrameEnd();
		floatingButtons.FrameEnd();
		selectionBoxes.FrameEnd();
		sliders.FrameEnd();
		glfwWaitEvents();
	}

	//Glim::Terminate();
	textFieldLayer.Destroy();
	Glim::FontManager::Destroy();
	glfwTerminate();
	return 0;
}
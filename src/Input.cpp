#include "Input.h"

namespace Glim::Input {

	bool mouseButtonsReleasing[3] = { false, false, false };
	bool mouseButtonsPressing[3] = { false, false, false };
	bool mouseButtons[3] = { false, false, false };
	float mousePos[2];
	bool cursorCollisionDetected = false;
	void* currentlyHandling = nullptr;
}

void Glim::Input::UpdateMouseButtons(int button, int action)
{
	if (button < 0 || button > 2)
		return;

	mouseButtonsReleasing[button] = !action;
	mouseButtonsPressing[button] = action;
	mouseButtons[button] = action;
}

void Glim::Input::FrameEnd()
{
	mouseButtonsReleasing[0] =
		mouseButtonsPressing[0] =
		mouseButtonsReleasing[1] =
		mouseButtonsPressing[1] =
		mouseButtonsReleasing[2] =
		mouseButtonsPressing[2] = false;
	cursorCollisionDetected = false;
}

void Glim::Input::UpdateMousePosition(double xpos, double ypos)
{
	mousePos[0] = xpos;
	mousePos[1] = ypos;
}

bool Glim::Input::MouseButtonDown(int buttonID)
{
	return mouseButtonsPressing[buttonID];
}

bool Glim::Input::MouseButtonUp(int buttonID)
{
	return mouseButtonsReleasing[buttonID];
}

bool Glim::Input::MouseButton(int buttonID)
{
	return mouseButtons[buttonID];
}

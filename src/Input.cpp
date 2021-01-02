#include "Input.h"
#include <unordered_map>

namespace Glim::Input {

	bool mouseButtonsReleasing[3] = { false, false, false };
	bool mouseButtonsPressing[3] = { false, false, false };
	bool mouseButtons[3] = { false, false, false };
	float mousePos[2];

	struct KeyState {

		bool pressing = false;
		bool releasing = false;
		bool repeating = false;
		bool isDown = false;
	};

	std::unordered_map<int, KeyState> keyStates;

	bool charInput = false;
	unsigned int character;

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

	for (auto& pair : keyStates)
	{
		pair.second.pressing =
			pair.second.releasing =
			pair.second.repeating = false;
	}

	charInput = false;

	cursorCollisionDetected = false;
}

void Glim::Input::UpdateKeyboard(int key, int action)
{
	if (keyStates.find(key) == keyStates.end())
		keyStates[key] = KeyState();

	if (action == 1)
	{
		keyStates[key].isDown = true;
		keyStates[key].pressing = true;
	}
	else if (action == 0)
	{
		keyStates[key].isDown = false;
		keyStates[key].releasing = true;
	}
	else
	{
		keyStates[key].repeating = true;
	}
}

void Glim::Input::UpdateCharacter(unsigned int character)
{
	Input::character = character;
	Input::charInput = true;
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


bool Glim::Input::KeyDown(int key)
{
	if (keyStates.find(key) == keyStates.end())
		return false;
	return keyStates[key].pressing;
}

bool Glim::Input::KeyUp(int key)
{
	if (keyStates.find(key) == keyStates.end())
		return false;
	return keyStates[key].releasing;
}

bool Glim::Input::Key(int key)
{
	if (keyStates.find(key) == keyStates.end())
		return false;
	return keyStates[key].isDown;
}

bool Glim::Input::KeyRepeat(int key)
{
	if (keyStates.find(key) == keyStates.end())
		return false;
	return keyStates[key].repeating;
}

bool Glim::Input::CharacterInput(unsigned int& character)
{
	character = Input::character;
	return charInput;
}
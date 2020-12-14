#pragma once

namespace Glim::Input {

	extern bool mouseButtons[3];
	extern float mousePos[2];

	void UpdateMouseButtons(int button, int action);
	void UpdateMousePosition(double xpos, double ypos);
	void FrameEnd();

	bool MouseButtonDown(int buttonID);
	bool MouseButtonUp(int buttonID);
	bool MouseButton(int buttonID);
}
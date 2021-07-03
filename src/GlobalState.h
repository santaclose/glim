#pragma once

namespace Glim::GlobalState {

	extern bool cursorCollisionDetected;
	extern void* currentlyHandling;

	void FrameEnd();
}
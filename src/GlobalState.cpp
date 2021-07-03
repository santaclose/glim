#include "GlobalState.h"

namespace Glim::GlobalState {

	bool cursorCollisionDetected = false;
	void* currentlyHandling = nullptr;

	void FrameEnd()
	{
		cursorCollisionDetected = false;
	}
}
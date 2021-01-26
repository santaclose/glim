#pragma once
#include <string>

namespace Glim::FontManager {
	
	void Init();
	unsigned int CreateFromFile(const std::string& filePath);
	void* GetFontData(unsigned int fontID);
	void Destroy();
}
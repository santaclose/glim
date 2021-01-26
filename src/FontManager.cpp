#include "FontManager.h"
#include <vector>
#include <iostream>
#include "msdfgl.h"

namespace Glim::FontManager {

	std::vector<msdfgl_font_t> fonts;
	msdfgl_context_t context;
}

void Glim::FontManager::Init()
{
	context = msdfgl_create_context("460 core");
	if (!context)
		std::cout << "[Text] Failed to create msdfgl context\n";

	/* Enable auto-generating undefined glyphs as they are encountered for the first time. */
	msdfgl_set_missing_glyph_callback(context, msdfgl_generate_glyph, NULL);
}

unsigned int Glim::FontManager::CreateFromFile(const std::string& filePath)
{

	fonts.emplace_back();
	fonts.back() = msdfgl_load_font(context, filePath.c_str(),
		1.7, 2.0, NULL);
	/* range, scale, atlas (NULL creates a new one) */

	/* Loads characters 0-128 onto the textures. This is where all the GPU cycles went. */
	msdfgl_generate_ascii(fonts.back());

	return fonts.size() - 1;
}

void* Glim::FontManager::GetFontData(unsigned int fontID)
{
	return &(fonts[fontID]);
}

void Glim::FontManager::Destroy()
{
	for (msdfgl_font_t& font : fonts)
		msdfgl_destroy_font(font);
	msdfgl_destroy_context(context);
}

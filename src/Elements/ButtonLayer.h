#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Geometry.h"
#include "../Shader.h"
#include "../CircleFont.h"

namespace Glim {

	class ButtonLayer {
	public:
		static std::vector<ButtonLayer*> instances;
		enum class IconSource { None = 0, CircleFont = 1 };

	private:
		struct ButtonInfo {

			uint32_t geometryIndex;
			glm::vec2 pos;
			float size;
			int iconIndex = -1;
		};

		int m_currentID = 0;
		std::vector<ButtonInfo> m_buttons;

		Shader m_shader;
		Geometry m_quads;
		IconSource m_iconSource;
		CircleFont m_circleFont;
		
		int m_currentlyInteracting = -1;

		bool CollisionTest(int buttonIndex);

	public:
		ButtonLayer::ButtonLayer(IconSource iconSource = IconSource::CircleFont, const std::string& iconsPath = "assets/icons.cf");
		ButtonLayer::~ButtonLayer();

		bool Evaluate(const glm::vec2& position, float size, int iconID = -1, const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f }, const glm::vec4& iconColor = { 1.0f, 1.0f, 1.0f, 1.0f });

		void FrameBegin();
		void BeforeDraw();
		void FrameEnd();
	};
}
#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "../Geometry.h"
#include "../Shader.h"
#include "../CircleFont.h"

namespace Glim {

	class ButtonLayer {
	public:
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

		const uint32_t* m_windowSize;
		Shader m_shader;
		Geometry m_quads;
		IconSource m_iconSource;
		CircleFont m_circleFont;
		
		int m_currentlyInteracting = -1;

		bool CollisionTest(int buttonIndex);

	public:
		void Init(const uint32_t* windowSize, IconSource iconSource = IconSource::None, const std::string& iconsPath = "");
		bool Evaluate(const glm::vec2& position, float size, int iconID = -1, const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f }, const glm::vec4& iconColor = { 1.0f, 1.0f, 1.0f, 1.0f });

		void FrameBegin();
		void BeforeDraw();
		void FrameEnd();
	};
}
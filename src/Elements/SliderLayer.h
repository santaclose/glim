#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "../Enums.h"
#include "../Geometry.h"
#include "../Shader.h"

namespace Glim {
	
	class SliderLayer {
	public:
		static std::vector<SliderLayer*> instances;

	private:
		struct SliderInfo {

			uint32_t geometryIndex;
			glm::vec2 pos;
			Orientation orientation;
			float size;
			float* value;
		};

		int m_currentID = 0;
		std::vector<SliderInfo> m_sliders;

		const uint32_t* m_windowSize;
		Shader m_shader;
		Geometry m_quads;

		int m_currentlyDraggingSlider = -1;
		float m_draggingOffset;

		bool CollisionTest(int sliderID);

	public:
		SliderLayer::SliderLayer();
		SliderLayer::~SliderLayer();

		void Evaluate(const glm::vec2& position, float sizeInPixels, float* value, Orientation orientation = Orientation::Horizontal, const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f });

		float GetWidth();

		void BeforeDraw();
		void FrameEnd();
	};
}
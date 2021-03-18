#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "../Geometry.h"
#include "../Shader.h"

namespace Glim {

	class CheckboxLayer {
	private:
		struct CheckboxInfo {

			uint32_t geometryIndex;
			glm::vec2 pos;
			float size;
			bool* value;
		};

		int m_currentID = 0;
		std::vector<CheckboxInfo> m_checkboxes;

		const uint32_t* m_windowSize;
		Shader m_shader;
		Geometry m_quads;

		int m_currentlyInteracting = -1;

		bool CollisionTest(int checkboxID);

	public:
		void Init(const uint32_t* windowSize);
		void Evaluate(const glm::vec2& position, bool* value, const glm::vec4& color = { 0.35f, 0.35f, 0.35f, 0.9f });
		float GetSize();

		void BeforeDraw();
		void FrameEnd();
	};
}
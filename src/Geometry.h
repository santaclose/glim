#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "Shader.h"
#include "Drawable.h"

namespace Glim {

	class Geometry : public Drawable {

	private:
		const Shader* m_shader;

		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;

		std::vector<Vertex> m_vertexVector;
		std::vector<unsigned int> m_indexVector;
		bool m_vertexDataHasChanged = false;

	public:
		void Init(const Shader* shader);
		unsigned int CreateQuad();
		unsigned int CreateQuad(
			const glm::vec2& pos,
			const glm::vec2& size,
			const glm::vec4& color,
			const glm::vec4& data = { 0.0f, 0.0f, 0.0f, 0.0f },
			const glm::vec4& moreData = { 0.0f, 0.0f, 0.0f, 0.0f });
		void UpdateQuadVertexCoords(
			unsigned int index,
			const glm::vec2& pos,
			const glm::vec2& size);
		void UpdateQuadColor(
			unsigned int index,
			const glm::vec4& color);
		void UpdateQuadData(
			unsigned int index,
			const glm::vec4& data,
			const glm::vec4& moreData = { 0.0f, 0.0f, 0.0f, 0.0f },
			unsigned char mask = 0xff);
		unsigned int GetVertexCount();
		const glm::vec2& GetVertexPos(unsigned int index);

		void Draw();
	};
}
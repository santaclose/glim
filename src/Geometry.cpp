#include "Geometry.h"
#include <glad/glad.h>
#include <iostream>
#include "Input.h"

namespace Glim {

	std::vector<Geometry*> Geometry::instances;
	const uint32_t* Geometry::windowSize;

	void Geometry::Init(const uint32_t* windowSize)
	{
		Geometry::windowSize = windowSize;
	}

	void Geometry::CreateFromShader(const Shader* shader)
	{
		m_shader = shader;

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, data));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, moreData));

		glBindVertexArray(0);

		instances.push_back(this);
	}

	unsigned int Geometry::CreateQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, const glm::vec4& data, const glm::vec4& moreData)
	{
		m_vertexVector.push_back({ {pos.x, pos.y}, color, data, moreData });
		m_vertexVector.push_back({ {pos.x + size.x, pos.y}, color, data, moreData });
		m_vertexVector.push_back({ {pos.x + size.x, pos.y + size.y}, color, data, moreData });
		m_vertexVector.push_back({ {pos.x, pos.y + size.y}, color, data, moreData });

		m_indexVector.push_back(m_vertexVector.size() - 4);
		m_indexVector.push_back(m_vertexVector.size() - 3);
		m_indexVector.push_back(m_vertexVector.size() - 2);
		m_indexVector.push_back(m_vertexVector.size() - 2);
		m_indexVector.push_back(m_vertexVector.size() - 1);
		m_indexVector.push_back(m_vertexVector.size() - 4);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertexVector.size() * sizeof(Vertex), &m_vertexVector[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexVector.size() * sizeof(unsigned int), &m_indexVector[0], GL_DYNAMIC_DRAW);

		return m_vertexVector.size() - 4;
	}

	void Geometry::UpdateQuadVertexCoords(unsigned int index, const glm::vec2& pos, const glm::vec2& size)
	{
		m_vertexVector[index + 0].pos = { pos.x, pos.y };
		m_vertexVector[index + 1].pos = { pos.x + size.x, pos.y };
		m_vertexVector[index + 2].pos = { pos.x + size.x, pos.y + size.y };
		m_vertexVector[index + 3].pos = { pos.x, pos.y + size.y };
		m_vertexDataHasChanged = true;
	}

	void Geometry::UpdateQuadColor(unsigned int index, const glm::vec4& color)
	{
		m_vertexVector[index + 0].color = color;
		m_vertexVector[index + 1].color = color;
		m_vertexVector[index + 2].color = color;
		m_vertexVector[index + 3].color = color;
		m_vertexDataHasChanged = true;
	}

	void Geometry::UpdateQuadData(unsigned int index, const glm::vec4& data, const glm::vec4& moreData, unsigned char mask)
	{
		if (mask & 0b10000000)
		{
			m_vertexVector[index + 0].data.x = data.x;
			m_vertexVector[index + 1].data.x = data.x;
			m_vertexVector[index + 2].data.x = data.x;
			m_vertexVector[index + 3].data.x = data.x;
		}
		if (mask & 0b01000000)
		{
			m_vertexVector[index + 0].data.y = data.y;
			m_vertexVector[index + 1].data.y = data.y;
			m_vertexVector[index + 2].data.y = data.y;
			m_vertexVector[index + 3].data.y = data.y;
		}
		if (mask & 0b00100000)
		{
			m_vertexVector[index + 0].data.z = data.z;
			m_vertexVector[index + 1].data.z = data.z;
			m_vertexVector[index + 2].data.z = data.z;
			m_vertexVector[index + 3].data.z = data.z;
		}
		if (mask & 0b00010000)
		{
			m_vertexVector[index + 0].data.w = data.w;
			m_vertexVector[index + 1].data.w = data.w;
			m_vertexVector[index + 2].data.w = data.w;
			m_vertexVector[index + 3].data.w = data.w;
		}
		if (mask & 0b00001000)
		{
			m_vertexVector[index + 0].moreData.x = moreData.x;
			m_vertexVector[index + 1].moreData.x = moreData.x;
			m_vertexVector[index + 2].moreData.x = moreData.x;
			m_vertexVector[index + 3].moreData.x = moreData.x;
		}
		if (mask & 0b00000100)
		{
			m_vertexVector[index + 0].moreData.y = moreData.y;
			m_vertexVector[index + 1].moreData.y = moreData.y;
			m_vertexVector[index + 2].moreData.y = moreData.y;
			m_vertexVector[index + 3].moreData.y = moreData.y;
		}
		if (mask & 0b00000010)
		{
			m_vertexVector[index + 0].moreData.z = moreData.z;
			m_vertexVector[index + 1].moreData.z = moreData.z;
			m_vertexVector[index + 2].moreData.z = moreData.z;
			m_vertexVector[index + 3].moreData.z = moreData.z;
		}
		if (mask & 0b00000001)
		{
			m_vertexVector[index + 0].moreData.w = moreData.w;
			m_vertexVector[index + 1].moreData.w = moreData.w;
			m_vertexVector[index + 2].moreData.w = moreData.w;
			m_vertexVector[index + 3].moreData.w = moreData.w;
		}
		m_vertexDataHasChanged = true;
	}

	unsigned int Geometry::GetVertexCount()
	{
		return m_vertexVector.size();
	}

	const glm::vec2& Geometry::GetVertexPos(unsigned int index)
	{
		return m_vertexVector[index].pos;
	}

	void Geometry::DrawAll()
	{
		for (int i = instances.size() - 1; i > -1; i--)
			instances[i]->Draw();
	}

	void Geometry::Draw()
	{
		if (m_vertexVector.size() == 0)
			return;

		if (m_vertexDataHasChanged)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertexVector.size() * sizeof(Vertex), &m_vertexVector[0], GL_DYNAMIC_DRAW);
			m_vertexDataHasChanged = false;
		}

		m_shader->Bind();
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_indexVector.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}
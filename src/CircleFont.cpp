#include "CircleFont.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

void Glim::CircleFont::CreateFromFile(const std::string& filePath)
{
	// load circle font file
	std::ifstream file(filePath);
	if (file.fail())
		std::cout << "[Circle Font] Failed to load from file: \"" << filePath << "\"\n";

	bool firstLineProcessed = false;
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '=')
			continue;

		if (line[0] == '-')
		{
			m_GroupData.resize(m_GroupData.size() + 1);
			if (sscanf(line.c_str(), "- %f -",
				&(m_GroupData[m_GroupData.size() - 1])) == EOF)
			{
				std::cout << "[Circle Font] Failed to parse group data\n";
				return;
			}
			continue;
		}

		if (!firstLineProcessed)
		{
			if (sscanf(line.c_str(), "%u;%u", &m_groupCount, &m_circlesPerGroup) == EOF)
			{
				std::cout << "[Circle Font] Failed to parse initial line\n";
				return;
			}
			firstLineProcessed = true;
		}
		else
		{
			m_CircleData.resize(m_CircleData.size() + 4);
			if (sscanf(line.c_str(), "%f,%f,%f,%f",
				&(m_CircleData[m_CircleData.size() - 4]),
				&(m_CircleData[m_CircleData.size() - 3]),
				&(m_CircleData[m_CircleData.size() - 2]),
				&(m_CircleData[m_CircleData.size() - 1])) == EOF)
			{
				std::cout << "[Circle Font] Failed to parse initial line\n";
				return;
			}
		}
	}
	m_glyphCount = m_CircleData.size() / (m_groupCount * m_circlesPerGroup) / 4;

	file.close();

	// create shader and geometry
	std::string vertexShaderPath = "assets/shaders/vert.glsl";
	std::string fragmentShaderPath = "assets/shaders/cf.glsl";
	std::ifstream ifvs(vertexShaderPath);
	std::ifstream iffs(fragmentShaderPath);

	if (ifvs.fail())
		std::cout << "[Circle Font] Could not read vertex shader file: " << vertexShaderPath << std::endl;
	if (iffs.fail())
		std::cout << "[Circle Font] Could not read fragment shader file: " << fragmentShaderPath << std::endl;

	std::string vertexShaderSource((std::istreambuf_iterator<char>(ifvs)),
		(std::istreambuf_iterator<char>()));
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(iffs)),
		(std::istreambuf_iterator<char>()));

	// set shader data size
	std::string toFind = "_DATA_FLOAT_COUNT_";
	std::size_t pos = fragmentShaderSource.find(toFind);
	fragmentShaderSource.replace(pos, toFind.length(), std::to_string(m_CircleData.size()));
	toFind = "_GROUP_DATA_COUNT_";
	pos = fragmentShaderSource.find(toFind);
	fragmentShaderSource.replace(pos, toFind.length(), std::to_string(m_GroupData.size()));


	m_shader.CreateFromStrings(vertexShaderSource, fragmentShaderSource);
	m_shader.Bind();
	m_shader.SetUniform1fv("u_Data", &(m_CircleData[0]), m_CircleData.size());
	m_shader.SetUniform1fv("u_GroupData", &(m_GroupData[0]), m_GroupData.size());
	m_shader.SetUniform1i("u_GroupCount", m_groupCount);
	m_shader.SetUniform1i("u_CirclesPerGroup", m_circlesPerGroup);
	m_quads.CreateFromShader(&m_shader);
}

unsigned int Glim::CircleFont::Add(unsigned int glyphID, const glm::vec2& position, float size, const glm::vec4& color)
{
	m_quads.CreateQuad(position, { size, size }, color, { position.x, position.y, size, (float)glyphID }, { 0.0, 0.0, 0.0, 0.0 });
	onScreenGlyphs++;
	return onScreenGlyphs - 1;
}

void Glim::CircleFont::UpdatePosition(unsigned int glyphInstanceID, const glm::vec2& position)
{
	float quadSize = abs(m_quads.GetVertexPos(glyphInstanceID * 4).x - m_quads.GetVertexPos(glyphInstanceID * 4 + 1).x);
	m_quads.UpdateQuadVertexCoords(glyphInstanceID * 4, position, { quadSize, quadSize });
	m_quads.UpdateQuadData(glyphInstanceID * 4, { position.x, position.y, quadSize, 0.0 }, { 0.0, 0.0, 0.0, 0.0 }, 0b11100000);
}

void Glim::CircleFont::UpdateSize(unsigned int glyphInstanceID, float newSize)
{
	glm::vec2 quadPos = m_quads.GetVertexPos(glyphInstanceID * 4);
	m_quads.UpdateQuadVertexCoords(glyphInstanceID * 4, quadPos, { newSize, newSize });
	m_quads.UpdateQuadData(glyphInstanceID * 4, { quadPos.x, quadPos.y, newSize, 0.0 }, { 0.0, 0.0, 0.0, 0.0 }, 0b11100000);
}

void Glim::CircleFont::ChangeGlyph(unsigned int glyphInstanceID, unsigned int newGlyphID)
{
	m_quads.UpdateQuadData(glyphInstanceID * 4, { 0.0, 0.0, 0.0, (float)newGlyphID }, { 0.0, 0.0, 0.0, 0.0 }, 0b00010000);
}

void Glim::CircleFont::SetGlyphColor(unsigned int glyphInstanceID, const glm::vec4& color)
{
	m_quads.UpdateQuadColor(glyphInstanceID * 4, color);
}

unsigned int Glim::CircleFont::GetGlyphCount()
{
	return m_glyphCount;
}

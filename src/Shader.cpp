#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>

namespace Glim {

	glm::mat4 Shader::vertexShaderMatrix;
	std::vector<Shader*> Shader::instances;

	void Shader::UpdateMatrix(const uint32_t* windowSize)
	{
		vertexShaderMatrix = glm::ortho(0.0f, (float)windowSize[0], (float)windowSize[1], 0.0f);
		for (Shader* shader : instances)
		{
			shader->Bind();
			shader->SetUniformMatrix4fv("u_Mat", &vertexShaderMatrix[0][0]);
		}
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
			std::cout << message << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

	Shader::Shader() : m_gl_id(-1) {}

	void Shader::CreateFromStrings(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		m_gl_id = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		glAttachShader(m_gl_id, vs);
		glAttachShader(m_gl_id, fs);
		glLinkProgram(m_gl_id);
		glValidateProgram(m_gl_id);

		glDeleteShader(vs);
		glDeleteShader(fs);

		instances.push_back(this);
	}

	void Shader::CreateFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		std::ifstream ifvs(vertexShaderPath);
		std::ifstream iffs(fragmentShaderPath);

		if (ifvs.fail())
			std::cout << "Could not read vertex shader file: " << vertexShaderPath << std::endl;
		if (iffs.fail())
			std::cout << "Could not read fragment shader file: " << fragmentShaderPath << std::endl;

		std::string vertexShaderSource((std::istreambuf_iterator<char>(ifvs)),
			(std::istreambuf_iterator<char>()));
		std::string fragmentShaderSource((std::istreambuf_iterator<char>(iffs)),
			(std::istreambuf_iterator<char>()));
		CreateFromStrings(vertexShaderSource, fragmentShaderSource);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_gl_id);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_gl_id);
		//std::cout << "Shader " << m_gl_id << " bound\n";
	}

	int Shader::GetUniformLocation(const std::string& name)
	{
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
			return m_uniformLocationCache[name];

		int location = glGetUniformLocation(m_gl_id, name.c_str());
		if (location == -1)
			std::cout << "Could not get uniform location for " << name << std::endl;

		m_uniformLocationCache[name] = location;
		return location;
	}

	void Shader::SetUniformMatrix4fv(const std::string& name, const float* pointer)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, pointer);
	}
	void Shader::SetUniform1fv(const std::string& name, const float* pointer, int count)
	{
		glUniform1fv(GetUniformLocation(name), count, pointer);
	}
	void Shader::SetUniform2fv(const std::string& name, const float* pointer, int count)
	{
		glUniform2fv(GetUniformLocation(name), count, pointer);
	}
	void Shader::SetUniform3fv(const std::string& name, const float* pointer, int count)
	{
		glUniform3fv(GetUniformLocation(name), count, pointer);
	}
	void Shader::SetUniform4fv(const std::string& name, const float* pointer, int count)
	{
		glUniform4fv(GetUniformLocation(name), count, pointer);
	}
	void Shader::SetUniform1i(const std::string& name, const int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}
	void Shader::SetUniform1f(const std::string& name, const float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}
}
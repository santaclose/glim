#pragma once

#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace Glim {

	class Shader
	{
	private:
		static glm::mat4 vertexShaderMatrix;
		static std::vector<Shader*> instances;
	public:
		static void UpdateMatrix(const uint32_t* windowSize);

	private:
		std::unordered_map<std::string, int> m_uniformLocationCache;
	public:
		unsigned int m_gl_id;
	private:
		static unsigned int CompileShader(unsigned int type, const std::string& source);
		int GetUniformLocation(const std::string& name);
	public:
		//Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		Shader();
		~Shader();
		void CreateFromStrings(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
		void CreateFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void Bind() const;
		void SetUniformMatrix4fv(const std::string& name, const float* pointer);
		void SetUniform1fv(const std::string& name, const float* pointer, int count = 1);
		void SetUniform2fv(const std::string& name, const float* pointer, int count = 1);
		void SetUniform3fv(const std::string& name, const float* pointer, int count = 1);
		void SetUniform4fv(const std::string& name, const float* pointer, int count = 1);
		void SetUniform1i(const std::string& name, const int value);
		void SetUniform1f(const std::string& name, const float value);
	};
}
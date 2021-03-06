#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct VertAndFrag {
	unsigned int vert;
	unsigned int frag;
};

struct Tess {
	unsigned int tcs;
	unsigned int tes;
};

class Shader
{
public:
	unsigned int ID;
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvaluationPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvaluationPath, const char* geometryPath);
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	VertAndFrag compileVertAndFrag(const char* vertexPath, const char* fragmentPath);
	unsigned int compileGeo(const char* geometryPath);
	Tess compileTess(const char* tessControlPath, const char* tessEvaluationPath);
	void checkCompileErrors(GLuint shader, std::string type);
};
#endif
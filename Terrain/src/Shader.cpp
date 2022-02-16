
#include <shader.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	ID = glCreateProgram();

	VertAndFrag vertAndFrag = compileVertAndFrag(vertexPath, fragmentPath);

	glAttachShader(ID, vertAndFrag.vert);
	glAttachShader(ID, vertAndFrag.frag);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertAndFrag.vert);
	glDeleteShader(vertAndFrag.frag);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	ID = glCreateProgram();

	VertAndFrag vertAndFrag = compileVertAndFrag(vertexPath, fragmentPath);
	unsigned int geo = compileGeo(geometryPath);

	glAttachShader(ID, vertAndFrag.vert);
	glAttachShader(ID, vertAndFrag.frag);
	glAttachShader(ID, geo);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertAndFrag.vert);
	glDeleteShader(vertAndFrag.frag);
	glDeleteShader(geo);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvaluationPath)
{
	ID = glCreateProgram();

	VertAndFrag vertAndFrag = compileVertAndFrag(vertexPath, fragmentPath);
	Tess tess = compileTess(tessControlPath, tessEvaluationPath);

	glAttachShader(ID, vertAndFrag.vert);
	glAttachShader(ID, vertAndFrag.frag);
	glAttachShader(ID, tess.tcs);
	glAttachShader(ID, tess.tes);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertAndFrag.vert);
	glDeleteShader(vertAndFrag.frag);
	glDeleteShader(tess.tcs);
	glDeleteShader(tess.tes);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvaluationPath, const char* geometryPath)
{
	ID = glCreateProgram();

	VertAndFrag vertAndFrag = compileVertAndFrag(vertexPath, fragmentPath);
	Tess tess = compileTess(tessControlPath, tessEvaluationPath);
	unsigned int geo = compileGeo(geometryPath);

	glAttachShader(ID, vertAndFrag.vert);
	glAttachShader(ID, vertAndFrag.frag);
	glAttachShader(ID, tess.tcs);
	glAttachShader(ID, tess.tes);
	glAttachShader(ID, geo);

	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertAndFrag.vert);
	glDeleteShader(vertAndFrag.frag);
	glDeleteShader(tess.tcs);
	glDeleteShader(tess.tes);
	glDeleteShader(geo);
}

Tess Shader::compileTess(const char* tessControlPath, const char* tessEvaluationPath)
{
	std::string tessControlCode;
	std::string tessEvaluationCode;

	std::ifstream tcShaderFile;
	std::ifstream teShaderFile;

	tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		tcShaderFile.open(tessControlPath);
		std::stringstream tcShaderStream;
		tcShaderStream << tcShaderFile.rdbuf();
		tcShaderFile.close();
		tessControlCode = tcShaderStream.str();

		teShaderFile.open(tessEvaluationPath);
		std::stringstream teShaderStream;
		teShaderStream << teShaderFile.rdbuf();
		teShaderFile.close();
		tessEvaluationCode = teShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	unsigned int tessControl;
	unsigned int tessEvaluation;

	const char* tcShaderCode = tessControlCode.c_str();
	tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessControl, 1, &tcShaderCode, NULL);
	glCompileShader(tessControl);
	checkCompileErrors(tessControl, "TESS CONTROL");
	std::cout << " loaded TCS" << std::endl;

	const char* teShaderCode = tessEvaluationCode.c_str();
	tessEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessEvaluation, 1, &teShaderCode, NULL);
	glCompileShader(tessEvaluation);
	checkCompileErrors(tessEvaluation, "TESS EVALUATION");
	std::cout << " loaded TES" << std::endl;

	return { tessControl, tessEvaluation };
}

unsigned int Shader::compileGeo(const char* geometryPath)
{
	std::string geometryCode;

	std::ifstream gShaderFile;

	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		gShaderFile.open(geometryPath);
		std::stringstream gShaderStream;
		gShaderStream << gShaderFile.rdbuf();
		gShaderFile.close();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	unsigned int geometry;

	const char* gShaderCode = geometryCode.c_str();
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);
	checkCompileErrors(geometry, "GEOMETRY");
	std::cout << " loaded GEO" << std::endl;

	return geometry;
}

VertAndFrag Shader::compileVertAndFrag(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();


	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	std::cout << " loaded VERTEX" << std::endl;

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	std::cout << " loaded FRAGMENT" << std::endl;

	return { vertex, fragment };
}

void Shader::use()
{
	glUseProgram(ID);
}

// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}


#include "Shader.h"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const { glUseProgram(ID); }

void Shader::set_bool(const std::string& name, bool value) const 
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), int(value)); 
}

void Shader::set_int(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


ComputeShader::ComputeShader(const std::string& computePath)
{
    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        cShaderFile.open(computePath);

        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();

        cShaderFile.close();

        computeCode = cShaderStream.str();
    }
    catch (const std::ifstream::failure& e)
    {
        std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();

    unsigned int computeShader;
    computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &cShaderCode, NULL);
    glCompileShader(computeShader);

    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, computeShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(computeShader);
}

void ComputeShader::use() const { glUseProgram(ID); }

void ComputeShader::set_bool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), int(value));
}

void ComputeShader::set_int(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::set_float(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::set_vec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void ComputeShader::set_vec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void ComputeShader::set_vec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void ComputeShader::set_vec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void ComputeShader::set_vec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void ComputeShader::set_vec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void ComputeShader::set_mat2(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::set_mat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::set_mat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
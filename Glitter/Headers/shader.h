#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath) {
		std::ifstream vf, ff;
		std::string vc, fc;
		std::stringstream vss, fss;

		vf.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		ff.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vf.open(vertexPath);
			vss << vf.rdbuf();
			vf.close();
			vc = vss.str();

			ff.open(fragmentPath);
			fss << ff.rdbuf();
			ff.close();
			fc = fss.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE\n" << e.what() << '\n';
		}
		const char* vsc = vc.c_str();
		const char* fsc = fc.c_str();

		unsigned int vs, fs;
		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vsc, NULL);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vs, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION\n" << infoLog << '\n';
		}

		fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fsc, NULL);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fs, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION\n" << infoLog << '\n';
		}

		ID = glCreateProgram();
		glAttachShader(ID, fs);
		glAttachShader(ID, vs);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING\n" << infoLog << '\n';
		}

		glUseProgram(ID);
		glDeleteShader(vs);
		glDeleteShader(fs);
    }

    // use/activate the shader
    void use() {
        glUseProgram(ID);
    }

    // utility uniform functions
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMatrix(const std::string& name, glm::mat4 matrix) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

private:
	int success;
	char infoLog[512];
};

#endif
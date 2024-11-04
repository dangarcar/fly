#include "ShaderProgram.hpp"

#include <fstream>
#include <sstream>
#include <glad/glad.h>

#include "Utils.h"

ShaderProgram::ShaderProgram(std::filesystem::path vertexPath, std::filesystem::path fragmentPath) {
    std::string vertexCode, fragmentCode;

    try {
        std::ifstream vertexFile(vertexPath), fragmentFile(fragmentPath);
        std::stringstream vStream, fStream;

        vStream << vertexFile.rdbuf();
        vertexFile.close();

        fStream << fragmentFile.rdbuf();
        fragmentFile.close();

        vertexCode = vStream.str();
        fragmentCode = fStream.str();
    } catch(std::ifstream::failure e) {
        writeError("GL Error: shader file not successfully read\n");
    }

    //DEBUG INFO
    int success;
    char infoLog[512];
        
    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vCode = vertexCode.c_str(), *fCode = fragmentCode.c_str();
    glShaderSource(vertexShader, 1, &vCode, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        writeError("GL Error: vertex shader compilation failed: %s\n", infoLog);
    }
        
    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fCode, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        writeError("GL Error: fragment shader compilation failed: %s\n", infoLog);
    }
        
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        writeError("GL Error: program link failed: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::use() const {
    glUseProgram(id);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
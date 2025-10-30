#include <string>
#include <glad/glad.h>
#include "shader.h"
#include <sstream>
#include <iostream>
#include <fstream>
#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_GLSL
#include "stb_include.h"

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    
    // Read soruce code in
    char vertError[256];
    char* vertSource{ stb_include_file((char*)vertPath.data(), nullptr, std::string("assets/shaders").data(), vertError)};
    
    if (!vertSource) {
        std::cerr << "ERROR::SHADER::VERTEX::STB_INCLUDE_READ_FAILED\n" << vertPath << "\n" << vertError << "\n";
    }

    char fragError[256];
    char* fragSource{ stb_include_file((char*)fragPath.data(), nullptr, std::string("assets/shaders").data(), fragError)};

    if (!fragSource) {
        std::cerr << "ERROR::SHADER::VERTEX::STB_INCLUDE_READ_FAILED\n" << fragPath << "\n" << fragError << "\n";
    }

    // Compile vertex
    unsigned int vertShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);

    int success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertPath << "\n" << infoLog << "\n";
    }

    // Compile fragment
    unsigned int fragShader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);

    success = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragPath << infoLog << "\n";
    }

    // Program
    mID = glCreateProgram();
    glAttachShader(mID, vertShader);
    glAttachShader(mID, fragShader);
    glLinkProgram(mID);

    success = 0;
    glGetProgramiv(mID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(mID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    free(vertSource);
    free(fragSource);
}
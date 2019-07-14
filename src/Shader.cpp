/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppreez <ppreez@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/12 08:18:19 by ppreez            #+#    #+#             */
/*   Updated: 2019/07/14 14:51:36 by ppreez           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Shader.hpp"

Shader::Shader()
{}

Shader::~Shader()
{}

Shader::Shader(const char *vertexpath, const char *fragmentpath)
{
    const char *vShaderCode;
    const char *fShaderCode;
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::stringstream vShaderStream;
    std::stringstream fShaderStream;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexpath);
        fShaderFile.open(fragmentpath);
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e) {
        std::cout << "File read exception: " << e.what() << std::endl;
    }
    vShaderCode = vertexCode.c_str();
    fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    compileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    compileErrors(fragment, "FRAGMENT");
    
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    compileErrors(id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(Shader const &rhs)
{
    *this = rhs;
}

Shader &Shader::operator=(Shader const &rhs)
{
    if (this != &rhs)
    {
        *this = rhs;
    }
    return *this;
}

void Shader::use() const
{
    glUseProgram(id);
}

 void Shader::setBool(const std::string &name, bool value) const
 {
     glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
 }

 void Shader::setInt(const std::string &name, int value) const
 {
     glUniform1i(glGetUniformLocation(id, name.c_str()), value);
 }

 void Shader::setFloat(const std::string &name, float value) const
 {
     glUniform1f(glGetUniformLocation(id, name.c_str()), value);
 }

void Shader::compileErrors(unsigned int shader, std::string type)
{
    int success;
    char infolog[512];
    
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infolog);
            std::cout << "Program link error: " << infolog << std::endl;
            m_errors = true;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infolog);
            std::cout << "Shader compile error: " << infolog << std::endl;
            m_errors = true;

        }
    }
    
}
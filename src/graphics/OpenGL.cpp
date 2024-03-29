/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OpenGL.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppreez <ppreez@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/14 12:59:07 by ppreez            #+#    #+#             */
/*   Updated: 2019/08/07 15:06:41 by ppreez           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "graphics/OpenGL.hpp"

 // Constructors 

OpenGL::OpenGL()
{
}

OpenGL::OpenGL(unsigned int width, unsigned int height, unsigned int size)
:m_grid_width(width), m_grid_height(height), m_pixel_width(width * size), m_pixel_height(height * size)
{
    initialise();
}

OpenGL::OpenGL(OpenGL const &rhs)
{
    *this = rhs;
}

OpenGL &OpenGL::operator=(OpenGL const &rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

OpenGL::~OpenGL()
{
    delete m_shader;
    glfwTerminate();
}

// Private methods

void OpenGL::initialise()
{
    if (!glfwInit())
        throw std::exception();
        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwSetErrorCallback(error_callback);
    
}

void OpenGL::error_callback(int error, const char *description)
{
    std::cout << "OpenGL error: " << error << std::endl;
    std::cout << "OpenGL error: " << description << std::endl;
}

// Public methods

void OpenGL::createWindow()
{
    int height, width;
    

    float vertices[] = 
    {
         1.0,  1.0,
         1.0,  0.0,
         0.0,  0.0,
         0.0,  1.0
    };
    

    unsigned int indices[] =
    {
        0, 1, 3,
        1, 2, 3
};


    m_window = glfwCreateWindow(m_pixel_width, m_pixel_height, "Nibbler", nullptr, nullptr);
    if (!m_window)
        throw std::exception();
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);
    m_shader = new Shader("./shaders/vertex.vs", "./shaders/fragment.fs");

    if (m_shader->m_errors)
    {
        glfwTerminate();
        exit(1);
    }
    glGenVertexArrays(1, &m_vertexArray);
    glGenBuffers(1, &m_vertexBuffer);
    glGenBuffers(1, &m_elementBuffer);

    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)0);
    glEnableVertexAttribArray(0);

}

void OpenGL::closeWindow()
{
    glfwTerminate();
}

int OpenGL::retrieveInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        return EXIT;
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        return UP;
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        return DOWN;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        return LEFT;
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        return RIGHT;
    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS)
        return OPENGL_KEY;
    if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS)
        return SDL_KEY;
    if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS)
        return SFML_KEY;
    else
        return NONE;
}

void OpenGL::startFrame()
{
    glClearColor(0.0, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader->use();
    glBindVertexArray(m_vertexArray);
}

void OpenGL::endFrame()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void OpenGL::drawSquare(unsigned int x, unsigned int y, struct s_color color)
{
    float r = static_cast<float>(color.r) / 255;
    float g = static_cast<float>(color.g) / 255;
    float b = static_cast<float>(color.b) / 255;
    m_shader->setVec3("color", r, g, b);
    m_shader->setVec3("pos", x, y, 0.0);
    m_shader->setVec2("size", m_grid_width, m_grid_height);
    m_shader->use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

extern "C"
{
    IGlib *create_renderer(unsigned int width, unsigned int height, unsigned int size)
    {
        return new OpenGL(width, height, size);
    }
}
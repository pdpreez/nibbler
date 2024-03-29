/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Game.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppreez <ppreez@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/14 11:32:38 by ppreez            #+#    #+#             */
/*   Updated: 2019/08/09 11:10:55 by ppreez           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Game.hpp"

Game::Game(int speed, char increase, bool gen_obstacles)
:m_stayOpen(true), m_width(20), m_height(20), m_size(20), m_fps(speed + 14), m_speed(speed), m_score(0), m_renderer(0)
{
    if (increase == 'Y' || increase == 'y')
        m_increase = true;
    else
        m_increase = false;
    m_handle = nullptr;
    m_gen_obstacles = gen_obstacles;
    glib = nullptr;
}

Game::Game(int width, int height, int speed, char increase, bool gen_obstacles)
:m_stayOpen(true), m_size(20), m_fps(speed + 14), m_speed(speed), m_score(0), m_renderer(0)
{
    if (increase == 'Y' || increase == 'y')
        m_increase = true;
    else
        m_increase = false;
    m_width = std::clamp(width, 20, 50);
    m_height = std::clamp(height, 20, 40);
    m_handle = nullptr;
    m_gen_obstacles = gen_obstacles;
    glib = nullptr;
}

Game::Game(Game const &rhs)
{
    *this = rhs;
}

Game &Game::operator=(Game const &rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

Game::~Game()
{
    delete snake;
    delete glib;
    delete fruit;

}

void Game::run()
{
    snake = new Snake(m_width / 3, m_height / 2);
    fruit = new Fruit(m_width, m_height);
    if (m_gen_obstacles)
        place_obstacles();
    glib = create_renderer("shared/OpenGL.so", m_width, m_height, m_size);
    if (glib)
        glib->createWindow();
    auto start = getTime();
    while (m_stayOpen)
    {
        double fps = (1.0 / m_fps) * 1000;
        auto current = getTime();
        double delta = current.count() - start.count();
        while (delta < fps)
        {
            current = getTime();
            delta = current.count() - start.count();
        }
        start = current;
        process_input();
        if (!m_stayOpen)
            break ;
        collisions();
        glib->startFrame();
        glib->drawSquare(snake->getX(), snake->getY(), snake->getColor());
        for (auto a = obstacles.begin(); a != obstacles.end(); a++)
        {
            glib->drawSquare((*a)->getX(), (*a)->getY(), (*a)->getColor());
        }
        for (auto a = snake->m_body.begin(); a != snake->m_body.end(); a++)
        {
            glib->drawSquare((*a)->getX(), (*a)->getY(), (*a)->getColor());
        }
        glib->drawSquare(fruit->getX(), fruit->getY(), fruit->getColor());
        glib->endFrame();
    }
    if (glib)
    {
        glib->closeWindow();
        std::cout << "Game over! Final score: " << m_score << std::endl;
    }
}

void Game::process_input()
{
    unsigned int key;
    key = glib->retrieveInput();
    if (key == EXIT)
        m_stayOpen = false;
    if (key == OPENGL_KEY || key == SDL_KEY || key == SFML_KEY)
        change_renderer(key);
    snake->move(key);
}

std::chrono::milliseconds Game::getTime() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void Game::collisions()
{
    int x = snake->getX();
    int y = snake->getY();
    if (x == fruit->getX() && y == fruit->getY())
    {
        if (m_increase)
        {
            m_fps += 2;
            m_speed += 2;
        }
        snake->grow();
        if (m_gen_obstacles)
            m_score += (m_speed * 2);
        else
            m_score += m_speed;
        while ((x == fruit->getX() && y == fruit->getY()) || body_collisions(fruit->getX(), fruit->getY()))
        {
            fruit->reroll();
        }
    }
    for (auto a = snake->m_body.begin(); a != snake->m_body.end(); a++)
    {
        if (body_collisions(x, y) || obstacle_collisions(x, y) 
        || (snake->getX() < 0 || snake->getX() >= m_width || snake->getY() < 0 || snake->getY() >= m_height ) 
        || (*a)->getX() < 0 || (*a)->getX() >= m_width || (*a)->getY() < 0 || (*a)->getY() >= m_height)
        {
            m_stayOpen = false;
        }
    }
}

bool Game::body_collisions(int x, int y)
{
    for (auto a = snake->m_body.begin(); a != snake->m_body.end(); a++)
    {
        if ((*a)->getX() == x && (*a)->getY() == y)
            return true;
    }
    return false;
}

bool Game::obstacle_collisions(int x, int y)
{
    for (auto a = obstacles.begin(); a != obstacles.end(); a++)
    {
        if ((*a)->getX() == x && (*a)->getY() == y)
            return true;
    }
    return false;
}

void Game::change_renderer(unsigned int key)
{
    std::string path = "shared/";
    if (m_renderer != key)
    {
        if (key == OPENGL_KEY)
            path += "OpenGL.so";
        else if (key == SDL_KEY)
            path += "SDL.so";
        else if (key == SFML_KEY)
            path += "SFML.so";
        glib->closeWindow();
        delete glib;
        glib = create_renderer(path, m_width, m_height, m_size);
        if (!glib)
        {
            std::cout << "Error: Could not open file " << path.erase(0, 7) << std::endl;
            m_stayOpen = false;
            return ;
        }
        glib->createWindow();
        m_renderer = key;
    }
}

IGlib *Game::create_renderer(std::string const &str, unsigned int width, unsigned int height, unsigned int size)
{
    if (m_handle)
        dlclose(m_handle);
    m_handle = dlopen(str.c_str(), RTLD_NOW | RTLD_GLOBAL);
    char *errors;
    if (!m_handle)
        return nullptr;
    createFunc func = (createFunc)dlsym(m_handle, "create_renderer");
    if (!func)
        return nullptr;
    errors = dlerror();
    if (errors)
    {
        std::cout << "Dynamic linking error: " << errors << std::endl;
        return nullptr;
    }
    return (*func)(width, height, size);
}

void Game::place_obstacles()
{
    int distribution = (m_width * m_height) / 35;
    // int nbr_height = m_height / 7;
    int x;
    int y;
    for (int i = 0; i < distribution; i++)
    {
            x = rand() % m_width;
            y = rand() % m_width;
            if (y != snake->getY())
                obstacles.push_front(new Obstacle(x, y));
    }
}
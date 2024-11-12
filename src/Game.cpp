#include <Game.h>
#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <sstream>
#include <memory>
#include <string>


Game::Game(const std::string &config)
{
    init(config);
}

void Game::init(const std::string &path)
{
    std::ifstream config_file(path);
    if (!config_file)
    {
        std::cerr << "Error opening config file " << path << std::endl;
    }

    std::string line;
    while (std::getline(config_file, line))
    {
        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);

        if (tokens[0] == "WINDOW")
        {
            if (std::stoi(tokens[4]) == 1)
            {
                m_window.create(sf::VideoMode(std::stoi(tokens[1]), std::stoi(tokens[2])), "GeometryGame", sf::Style::Fullscreen);
            }
            else
            {
                m_window.create(sf::VideoMode(std::stoi(tokens[1]), std::stoi(tokens[2])), "GeometryGame", sf::Style::Default);
            }

            m_window.setFramerateLimit(std::stoi(tokens[3]));
        }
        else if (tokens[0] == "PLAYER")
        {
            m_playerConfig.SR = std::stoi(tokens[1]);
            m_playerConfig.CR = std::stoi(tokens[2]);
            m_playerConfig.S = std::stof(tokens[3]);
            m_playerConfig.FR = std::stoi(tokens[4]);
            m_playerConfig.FG = std::stoi(tokens[5]);
            m_playerConfig.FB = std::stoi(tokens[6]);
            m_playerConfig.OR = std::stoi(tokens[7]);
            m_playerConfig.OG = std::stoi(tokens[8]);
            m_playerConfig.OB = std::stoi(tokens[9]);
            m_playerConfig.OT = std::stoi(tokens[10]);
            m_playerConfig.V = std::stoi(tokens[11]);
        }
        else if (tokens[0] == "ENEMY")
        {
            m_enemyConfig.SR = std::stoi(tokens[1]);
            m_enemyConfig.CR = std::stoi(tokens[2]);
            m_enemyConfig.SMIN = std::stof(tokens[3]);
            m_enemyConfig.SMAX = std::stof(tokens[4]);
            m_enemyConfig.OR = std::stoi(tokens[5]);
            m_enemyConfig.OG = std::stoi(tokens[6]);
            m_enemyConfig.OB = std::stoi(tokens[7]);
            m_enemyConfig.OT = std::stoi(tokens[8]);
            m_enemyConfig.VMIN = std::stoi(tokens[9]);
            m_enemyConfig.VMAX = std::stoi(tokens[10]);
            m_enemyConfig.L = std::stoi(tokens[11]);
            m_enemyConfig.SI = std::stoi(tokens[12]);
        }
    }

    config_file.close();

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        if (!m_paused)
        {
            m_entities.update();

            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();
            sUserInput();
            sRender();

            // incrementing the frame may need to move when pause is implemented
            m_currentFrame++;
        }
    }
}

void Game::setPaused(bool paused)
{
    m_paused = true;
    m_running = false;
}

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR,
                                              m_playerConfig.V,
                                              sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
                                              sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
                                              m_playerConfig.OT);

    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
}

void Game::spawnEnemy()
{
    float radius = m_enemyConfig.SR;
    float collRadius = m_enemyConfig.CR;

    auto entity = m_entities.addEntity("enemy");

    float ex = collRadius + (std::rand() % (m_window.getSize().x - (int)std::ceil(2 * collRadius)));
    float ey = collRadius + (std::rand() % (m_window.getSize().y - (int)std::ceil(2 * collRadius)));

    float speed = m_enemyConfig.SMIN + (std::rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1));
    float angle = std::rand() % 360;

    float sx = std::cos(angle) * speed;
    float sy = std::sin(angle) * speed;

    int sides = m_enemyConfig.VMIN + (std::rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1));

    int r = (std::rand() % 255);
    int g = (std::rand() % 255);
    int b = (std::rand() % 255);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(sx, sy), 0.0f);

    entity->cShape = std::make_shared<CShape>(radius, sides, sf::Color(r, g, b), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

    entity->cCollision = std::make_shared<CCollision>(collRadius);

    entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target)
{
    // TODO: implement spawning of bullet which travels toward target
    // bullet speed is given as scalar speed
    // must set velocity by using formula in the slides
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}

void Game::sMovement()
{
    for (auto e : m_entities.getEntities("enemy"))
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    if ((m_player->cInput->up || m_player->cInput->down) && !(m_player->cInput->up && m_player->cInput->down))
    {
        if ((!m_player->cInput->right && !m_player->cInput->left) || (m_player->cInput->right && m_player->cInput->left))
        {
            m_player->cTransform->velocity = {0.0, m_playerConfig.S};

            m_player->cInput->up ? m_player->cTransform->pos.y -= m_player->cTransform->velocity.y 
                                 : m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
        }
        else
        {
            m_player->cTransform->velocity = {m_playerConfig.S * (float) std::cos(M_PI_4), m_playerConfig.S * (float) std::sin(M_PI_4)};

            m_player->cInput->right ? m_player->cTransform->pos.x += m_player->cTransform->velocity.x 
                                    : m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;

            m_player->cInput->up ? m_player->cTransform->pos.y -= m_player->cTransform->velocity.y 
                                 : m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

        }
    }
    else if ((m_player->cInput->right || m_player->cInput->left) && !(m_player->cInput->right && m_player->cInput->left))
    {
        m_player->cTransform->velocity = {m_playerConfig.S, 0.0f};

        m_player->cInput->right ? m_player->cTransform->pos.x += m_player->cTransform->velocity.x 
                                : m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
    }
}

void Game::sLifespan()
{
    for (auto e : m_entities.getEntities())
    {
        if (e->cLifespan)
        {
            if (e->cLifespan->remaining > 0)
            {
                e->cLifespan->remaining--;
                // implement alpha channel
            }
            else
            {
                e->destroy();
            }
        }
    }
}

void Game::sCollision()
{
    // TODO: implement all collision between entities
    // be sure to use collision radius, not shape radius
    for (auto e : m_entities.getEntities("enemy"))
    {
        // std::cout << e->cTransform->pos.x << ",  " << e->cTransform->pos.y << std::endl;
        if (e->cTransform->pos.x <= e->cCollision->radius || e->cTransform->pos.x >= m_window.getSize().x - e->cCollision->radius)
        {
            e->cTransform->velocity.x *= -1;
        }
        if (e->cTransform->pos.y <= e->cCollision->radius || e->cTransform->pos.y >= m_window.getSize().y - e->cCollision->radius)
        {
            e->cTransform->velocity.y *= -1;
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
    {
        spawnEnemy();
    }
}

void Game::sRender()
{
    m_window.clear();

    for (auto e : m_entities.getEntities())
    {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        m_window.draw(e->cShape->circle);
    }

    m_window.display();
}

void Game::sUserInput()
{
    // TODO: handle user input here
    // note: should only be setting the player's input components variables here
    // you should not implement the player's movement here
    // the movement system will read the variables you set in this function

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = true;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = true;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = true;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = true;
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = false;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = false;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = false;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = false;
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "Left mouse button clicked at " << event.mouseButton.x << ", " << event.mouseButton.y << "\n";
                // TODO: call spawnBullet here
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "Right mouse button clicked at " << event.mouseButton.x << ", " << event.mouseButton.y << "\n";
                // TODO: call spawnSpecialWeapon here
            }
        }
    }
}
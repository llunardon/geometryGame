#include <Game.h>
#include <iostream>
#include <fstream>
#include <cmath>

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path) 
{
    // TODO: read config file here, use PlayerConfig, EnemyConfig, BulletConfig structs
    // std::ifstream fin(path);
    // fin >> m_playerConfig.SR >> m_playerConfig.CR >>;

    m_window.create(sf::VideoMode(1280, 720), "GeometryGame");
    m_window.setFramerateLimit(60);

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
    // TODO: finish adding all properties of the player with config values

    // create every entity by calling EntityManager.addEntity(tag)
    auto entity = m_entities.addEntity("player");

    // give this entity a transform so it spawns at the center of the window with v(0, 0) and angle 0
    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    // shape: radius 32, 8 sides, dark grey fill, red outline of thickness 4
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // add input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // since we want this entity to be our player, set our Game's m_player variable to be this entity
    // this goes against the EntityManager paradigm, but we use the player so much that it's worth it
    m_player = entity;
}

void Game::spawnEnemy()
{
    float radius = 16.0f;
    auto entity = m_entities.addEntity("enemy");

    float ex = radius + (std::rand() % (m_window.getSize().x - (int) std::ceil(2 * radius)));
    float ey = radius + (std::rand() % (m_window.getSize().y - (int) std::ceil(2 * radius)));

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(radius, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 2.0f);

    entity->cLifespan = std::make_shared<CLifespan>(240);

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
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
    // TODO: implement all entity movement in this function
    // should read the m_player->cInput component to determine if player is moving 

    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
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
            } else
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
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime > 120)
    {
        std::cout << "enemy spawned" << std::endl;
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
                    std::cout << "W key was pressed\n";
                    // TODO: set player's input component "up" to true
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
                    std::cout << "W key was released\n";
                    // TODO: set player's input component "up" to false
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
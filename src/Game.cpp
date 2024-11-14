#include <Game.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>

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

    if (!m_font.loadFromFile("../opensans.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(10.0f, 10.0f);

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

            m_frameRate = std::stoi(tokens[3]);
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
        else if (tokens[0] == "BULLET")
        {
            m_bulletConfig.SR = std::stoi(tokens[1]);
            m_bulletConfig.CR = std::stoi(tokens[2]);
            m_bulletConfig.S = std::stof(tokens[3]);
            m_bulletConfig.FR = std::stoi(tokens[4]);
            m_bulletConfig.FG = std::stoi(tokens[5]);
            m_bulletConfig.FB = std::stoi(tokens[6]);
            m_bulletConfig.OR = std::stoi(tokens[7]);
            m_bulletConfig.OG = std::stoi(tokens[8]);
            m_bulletConfig.OB = std::stoi(tokens[9]);
            m_bulletConfig.OT = std::stoi(tokens[10]);
            m_bulletConfig.V = std::stoi(tokens[11]);
            m_bulletConfig.L = std::stoi(tokens[12]);
        }
    }

    config_file.close();

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        sUserInput();

        if (!m_paused)
        {
            m_entities.update();

            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();

            m_currentFrame++;
        }

        sRender();
    }
}

void Game::setPaused(bool paused)
{
    paused ? m_paused = false : m_paused = true;
}

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    Vec2 center = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f};
    Vec2 velocity = {0.0f, 0.0f};

    entity->cTransform = std::make_shared<CTransform>(center, velocity, 0.0f);

    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR,
                                              m_playerConfig.V,
                                              sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
                                              sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
                                              m_playerConfig.OT);

    entity->cInput = std::make_shared<CInput>();

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    m_player = entity;
}

void Game::spawnEnemy()
{

    auto entity = m_entities.addEntity("enemy");

    float collRadius = m_enemyConfig.CR;

    Vec2 origin = {collRadius + (std::rand() % (m_window.getSize().x - (int)std::ceil(2 * collRadius))), collRadius + (std::rand() % (m_window.getSize().y - (int)std::ceil(2 * collRadius)))};

    float speed = m_enemyConfig.SMIN + (std::rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1));
    float angle = std::rand() % 360;

    Vec2 velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

    int sides = m_enemyConfig.VMIN + (std::rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1));

    int r = (std::rand() % 255);
    int g = (std::rand() % 255);
    int b = (std::rand() % 255);

    entity->cTransform = std::make_shared<CTransform>(origin, velocity, 0.0f);

    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR,
                                              sides,
                                              sf::Color(r, g, b),
                                              sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
                                              m_enemyConfig.OT);

    entity->cCollision = std::make_shared<CCollision>(collRadius);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    float radius = m_enemyConfig.SR / 3;
    float collRadius = m_enemyConfig.CR / 3;

    Vec2 origin = {e->cTransform->pos.x, e->cTransform->pos.y};

    float angle = e->cTransform->angle;
    float speed = m_playerConfig.S;

    int sides = e->cShape->circle.getPointCount();

    sf::Color fillColor = e->cShape->circle.getFillColor();
    sf::Color outlineColor = e->cShape->circle.getOutlineColor();
    float outlineThickness = e->cShape->circle.getOutlineThickness() / 2;

    for (int i = 0; i < sides; i++)
    {
        auto entity = m_entities.addEntity("small");

        Vec2 velocity = {speed * (float)std::cos((360.0f / sides) * i * (M_PI / 180.0f) + angle), speed * (float)std::sin((360.0f / sides) * i * (M_PI / 180.0f) + angle)};

        entity->cTransform = std::make_shared<CTransform>(origin, velocity, angle);

        entity->cCollision = std::make_shared<CCollision>(collRadius);

        entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

        entity->cShape = std::make_shared<CShape>(radius, sides, fillColor, outlineColor, outlineThickness);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target)
{
    auto bulletEntity = m_entities.addEntity("bullet");

    float originX = entity->cTransform->pos.x;
    float originY = entity->cTransform->pos.y;
    float angle = std::atan2((target.y - originY), (target.x - originX));

    Vec2 velocity = {std::cos(angle) * m_bulletConfig.S, std::sin(angle) * m_bulletConfig.S};

    sf::Color fillColor = sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
    sf::Color outlineColor = sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);

    bulletEntity->cTransform = std::make_shared<CTransform>(Vec2(originX, originY), velocity, 0.0f);

    bulletEntity->cShape = std::make_shared<CShape>(m_bulletConfig.SR,
                                                    m_bulletConfig.V,
                                                    fillColor,
                                                    outlineColor,
                                                    m_bulletConfig.OT);

    bulletEntity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    bulletEntity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2 &target)
{
    auto bulletEntity = m_entities.addEntity("special");

    float originX = entity->cTransform->pos.x;
    float originY = entity->cTransform->pos.y;
    float angle = std::atan2((target.y - originY), (target.x - originX));

    Vec2 velocity = {std::cos(angle) * m_bulletConfig.S, std::sin(angle) * m_bulletConfig.S};

    sf::Color fillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
    sf::Color outlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);

    bulletEntity->cTransform = std::make_shared<CTransform>(Vec2(originX, originY), velocity, 0.0f);

    bulletEntity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, 
                                                    m_playerConfig.V,
                                                    fillColor,
                                                    outlineColor,
                                                    m_playerConfig.OT / 2);

    bulletEntity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    bulletEntity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

bool Game::canMove(const char dir)
{
    switch (dir)
    {
    case 'u':
        return m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() > 0;
        break;
    case 'd':
        return m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() < m_window.getSize().y;
        break;
    case 'r':
        return m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() < m_window.getSize().x;
        break;
    case 'l':
        return m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() > 0;
        break;
    default:
        return false;
        break;
    }
}

void Game::sMovement()
{
    for (auto e : m_entities.getEntities("enemy"))
    {
        e->cTransform->pos += e->cTransform->velocity;
    };

    for (auto e : m_entities.getEntities("bullet"))
    {
        e->cTransform->pos += e->cTransform->velocity;
    };

    for (auto e : m_entities.getEntities("special"))
    {
        e->cTransform->pos += e->cTransform->velocity;
    };

    for (auto e : m_entities.getEntities("small"))
    {
        e->cTransform->pos += e->cTransform->velocity;
    };

    if ((m_player->cInput->up || m_player->cInput->down) && !(m_player->cInput->up && m_player->cInput->down))
    {
        /*
            straight up or down
        */
        if ((!m_player->cInput->right && !m_player->cInput->left) || (m_player->cInput->right && m_player->cInput->left))
        {
            m_player->cTransform->velocity = {0.0, m_playerConfig.S};

            if (m_player->cInput->up && (canMove('u')))
            {
                m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
            }
            else if (m_player->cInput->down && (canMove('d')))
            {
                m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
            }
        }
        /*
            diagonal
        */
        else
        {
            m_player->cTransform->velocity = {m_playerConfig.S * (float)std::cos(M_PI_4), m_playerConfig.S * (float)std::sin(M_PI_4)};

            if (m_player->cInput->right && (canMove('r')))
            {
                m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
            }
            else if (m_player->cInput->left && (canMove('l')))
            {
                m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
            }

            if (m_player->cInput->up && (canMove('u')))
            {
                m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
            }
            else if (m_player->cInput->down && (canMove('d')))
            {
                m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
            }
        }
    }
    /*
        straight right or left
    */
    else if ((m_player->cInput->right || m_player->cInput->left) && !(m_player->cInput->right && m_player->cInput->left))
    {
        m_player->cTransform->velocity = {m_playerConfig.S, 0.0f};

        if (m_player->cInput->right && (canMove('r')))
        {
            m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
        }
        else if (m_player->cInput->left && (canMove('l')))
        {
            m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
        }
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

                sf::Color color = e->cShape->circle.getFillColor();
                color.a = 255 * e->cLifespan->remaining / e->cLifespan->total;

                e->cShape->circle.setFillColor(color);
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
    for (auto e : m_entities.getEntities("enemy"))
    {
        /* enemy-bullet collision */
        for (auto bullet : m_entities.getEntities("bullet"))
        {
            if (e->cTransform->pos.dist(bullet->cTransform->pos) < std::abs(e->cCollision->radius + bullet->cCollision->radius))
            {
                spawnSmallEnemies(e);
                e->destroy();
            }
        }

        /* enemy-special weapon collision */
        for (auto specialBullet : m_entities.getEntities("special"))
        {
            if (e->cTransform->pos.dist(specialBullet->cTransform->pos) < std::abs(e->cCollision->radius + specialBullet->cCollision->radius))
            {
                e->destroy();
                specialBullet->destroy();
            }
        }

        /* enemy-player collision */
        if (e->cTransform->pos.dist(m_player->cTransform->pos) < std::abs(e->cCollision->radius + m_player->cCollision->radius))
        {
            m_player->cTransform->pos = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f};
        }

        /* enemy-wall collision */
        if (e->cTransform->pos.x <= e->cCollision->radius || e->cTransform->pos.x >= m_window.getSize().x - e->cCollision->radius)
        {
            e->cTransform->velocity.x *= -1;
        }
        if (e->cTransform->pos.y <= e->cCollision->radius || e->cTransform->pos.y >= m_window.getSize().y - e->cCollision->radius)
        {
            e->cTransform->velocity.y *= -1;
        }
    }

    for (auto e : m_entities.getEntities("small"))
    {
        /* enemy-player collision */
        if (e->cTransform->pos.dist(m_player->cTransform->pos) < std::abs(e->cCollision->radius + m_player->cCollision->radius))
        {
            m_player->cTransform->pos = {m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f};
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

    float roundedTime = (float) m_currentFrame / m_frameRate;

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << roundedTime;
    std::string roundedTimeStr = out.str();
    m_text.setString(roundedTimeStr);

    m_window.draw(m_text);

    m_window.display();
}

void Game::sUserInput()
{
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
            case sf::Keyboard::P:
                setPaused(m_paused);
                break;
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
            if (!m_paused)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                }

                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    spawnSpecialWeapon(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                }
            }
        }
    }
}
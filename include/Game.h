#pragma once

#include <Entity.h>
#include <EntityManager.h>
#include <SFML/Graphics.hpp>

/*
    config file:
        - WINDOW W H FL FS
            - W, H: window width and height (int)
            - FL: frame limit (int)
            - FS: full screen or not (bool)

        - FONT F S R G B
            - F: font file name, no whitespace (std::string)
            - S: font size (int)
            - R, G, B (int, int, int)

        - PLAYER SR CR S FR FG FB OR OG OB OT V
            - SR: shape radius (int)
            - CR: collision radius (int)
            - S: speed, magnitude (float)
            - FR, FG, FB: fill color (int, int, int)
            - OR, OG, OB: outline color (int, int, int)
            - OT: outline thickness (int)
            - V: number of vertices of the shape (int)

        - ENEMY SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
            - SR: shape radius (int)
            - CR: collision radius (int)
            - SMIN, SMAX: speed min and max (float, float)
            - OR, OG, OB: outline color (int, int, int)
            - VMIN, VMAX: min and max vertices (int, int)
            - L: small shape lifespan (int)
            - SI: spawn interval (int)

         - BULLET: SR CR S FR FG FB OR OG OB OT V L
            - SR: shape radius (int)
            - CR: collision radius (int)
            - S: speed, magnitude (float)
            - FR, FG, FB: fill color (int, int, int)
            - OR, OG, OB: outline color (int, int, int)
            - OT: outline thickness (int)
            - V: number of vertices of the shape (int)
            - L: lifespan (int)
*/

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};

struct EnemyConfig
{
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};

struct BulletConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game
{
    sf::RenderWindow m_window;
    EntityManager m_entities;
    sf::Font m_font;
    sf::Text m_text;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    int m_frameRate = 60;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    int m_lastSpecialWeaponUse = 0;
    int m_specialWeaponCooldown = m_frameRate * 4;
    bool m_paused = false;
    bool m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string &config);
    void setPaused(bool paused);

    bool canMove(const char dir);

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sRender();
    void sEnemySpawner();
    void sCollision();

    void drawLoadingBar();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vec2 &target);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);

public:
    Game(const std::string &config);

    void run();
};
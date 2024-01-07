//
// Created by Vlad Puscaru on 07.12.2023.
//

#ifndef COMP4300_ASSIGNMENT2_GAMEENGINE_H
#define COMP4300_ASSIGNMENT2_GAMEENGINE_H

#include <SFML/Graphics.hpp>
#include "../EntityManager/EntityManager.h"
#include "../ConfigSpecification.h"
#include <cmath>
#include <imgui.h>
#include <imgui-SFML.h>

class GameEngine {
private:
    EntityManager m_eManager;

    sf::RenderWindow m_window;

    sf::Font font;
    sf::Text text;

    bool m_paused;

    Config m_config;

    size_t m_currentFrame;

    void spawnEnemy();
    void spawnPlayer(const std::shared_ptr<Entity>& existingPlayer = nullptr);
    void spawnBullet(sf::Vector2f origin, sf::Vector2f direction);
    void spawnSmallEnemies(const std::shared_ptr<Entity>& parent);

    sf::Vector2f normalize(const sf::Vector2f& vec);

    bool collides(const sf::CircleShape& cA, const sf::CircleShape& cB);

    void drawUI();
public:
    GameEngine(int wWidth, int wHeight, const std::string& wTitle, Config config);

    void run();

    void sUserInput();
    void sLifeSpan();
    void sCollision();
    void sMovement();
    void sRender();
    void sSpawner();
};

float getRand(int min, int max);

#endif //COMP4300_ASSIGNMENT2_GAMEENGINE_H

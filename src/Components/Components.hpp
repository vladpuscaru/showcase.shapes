//
// Created by Vlad Puscaru on 07.12.2023.
//

#include <SFML/Graphics.hpp>

struct CTransform {
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
};

struct CShape {
    sf::CircleShape m_shape;
};

struct CInput {
    bool m_top = false;
    bool m_right = false;
    bool m_down = false;
    bool m_left = false;
};

struct CLifespan {
    int m_totalFrames = 0;
    int m_currentFrame = 0;
};

struct CScore {
    int score = 0;
};

struct CHealth {
    int lives = 3;
};
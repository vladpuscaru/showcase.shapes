//
// Created by Vlad Puscaru on 07.12.2023.
//

#include "GameEngine.h"

GameEngine::GameEngine(int wWidth, int wHeight, const std::string &wTitle, Config config)
        : m_window(sf::VideoMode(wWidth, wHeight), wTitle), m_config(config), m_currentFrame(0), m_paused(false) {
    spawnPlayer();

    if (!font.loadFromFile("./resources/font.ttf")) {
        std::cerr << "Cannot load font" << std::endl;
        exit(-1);
    }
    text.setFont(font);
}

void GameEngine::run() {
    m_window.setFramerateLimit(60);

    sf::Clock clock;
    ImGui::SFML::Init(m_window);

    while (m_window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        ImGui::SFML::Update(m_window, deltaTime);

        m_currentFrame++;
        m_eManager.update();

        if (m_config.gameConfig.isSUserInputActive) {
            sUserInput();
        }
        if (!m_paused && m_config.gameConfig.isSMovementActive) {
            sMovement();
        }
        if (!m_paused && m_config.gameConfig.isSSpawnerActive) {
            sSpawner();
        }
        if (!m_paused && m_config.gameConfig.isSCollisionActive) {
            sCollision();
        }
        if (!m_paused && m_config.gameConfig.isSLifespanActive) {
            sLifeSpan();
        }

        drawUI();

        sRender();
    }
}

void GameEngine::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);

        // General events
        if (event.type == sf::Event::Closed) {
            m_window.close();
            break;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    m_window.close();
                    break;
                case sf::Keyboard::P:
                    m_paused = !m_paused;
                    break;
            }
        }

        // Events that affect entities
        for (auto &e: m_eManager.getEntities()) {
            if (e->cInput) {
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            e->cInput->m_top = true;
                            break;
                        case sf::Keyboard::A:
                            e->cInput->m_left = true;
                            break;
                        case sf::Keyboard::S:
                            e->cInput->m_down = true;
                            break;
                        case sf::Keyboard::D:
                            e->cInput->m_right = true;
                            break;
                    }
                } else if (event.type == sf::Event::KeyReleased) {
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            e->cInput->m_top = false;
                            break;
                        case sf::Keyboard::A:
                            e->cInput->m_left = false;
                            break;
                        case sf::Keyboard::S:
                            e->cInput->m_down = false;
                            break;
                        case sf::Keyboard::D:
                            e->cInput->m_right = false;
                            break;
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (e->cTransform && e->cShape) {
                        spawnBullet({e->cTransform->m_position.x + e->cShape->m_shape.getRadius(),
                                     e->cTransform->m_position.y + e->cShape->m_shape.getRadius()},
                                    {event.mouseButton.x - e->cTransform->m_position.x,
                                     event.mouseButton.y - e->cTransform->m_position.y});
                    }
                }
            }
        }
    }
}

void GameEngine::sCollision() {
    for (auto &e: m_eManager.getEntities()) {
        if (e->cTransform && e->cShape) {
            if (e->getTag() != TAG_BULLET) {
                // Collision with window sides
                float left = e->cTransform->m_position.x;
                float top = e->cTransform->m_position.y;
                float right = left + e->cShape->m_shape.getRadius() * 2;
                float bot = top + e->cShape->m_shape.getRadius() * 2;

                if (left <= 0 || right >= m_window.getSize().x) {
                    e->cTransform->m_velocity.x = -e->cTransform->m_velocity.x;
                    if (left <= 0) {
                        e->cTransform->m_position.x = 0;
                        if (e->cInput) {
                            e->cInput->m_left = false;
                        }
                    }
                    if (right >= m_window.getSize().x)
                        e->cTransform->m_position.x = m_window.getSize().x - e->cShape->m_shape.getRadius() * 2;
                }

                if (top <= 0 || bot >= m_window.getSize().y) {
                    e->cTransform->m_velocity.y = -e->cTransform->m_velocity.y;
                    if (top <= 0)
                        e->cTransform->m_position.y = 0;
                    if (bot >= m_window.getSize().y)
                        e->cTransform->m_position.y = m_window.getSize().y - e->cShape->m_shape.getRadius() * 2;
                }
            }

            // Bullet - Enemy and Player - Enemy Collision
            if (e->getTag() == TAG_ENEMY) {
                auto players = m_eManager.getEntities(TAG_PLAYER);
                for (auto &player: players) {
                    if (player->cShape && collides(e->cShape->m_shape, player->cShape->m_shape)) {
                        e->destroy();
                        player->destroy();
                        if (player->cHealth) {
                            player->cHealth->lives--;
                        }
                        spawnPlayer(player);
                    }
                }

                if (e->isAlive()) {
                    auto bullets = m_eManager.getEntities(TAG_BULLET);
                    for (auto &bullet: bullets) {
                        if (bullet->cShape && collides(e->cShape->m_shape, bullet->cShape->m_shape)) {
                            e->destroy();
                            bullet->destroy();
                            spawnSmallEnemies(e);

                            for (auto &player: players) {
                                if (player->cScore) {
                                    player->cScore->score += e->cShape->m_shape.getPointCount();
                                }
                            }
                        }
                    }
                }
            }

            // Small Enemy - Bullet and Small Enemy - Player Collision
            if (e->getTag() == TAG_SMALL_ENEMY) {
                auto players = m_eManager.getEntities(TAG_PLAYER);
                for (auto &player: players) {
                    if (player->cShape && collides(e->cShape->m_shape, player->cShape->m_shape)) {
                        e->destroy();
                        player->destroy();
                        if (player->cHealth) {
                            player->cHealth->lives--;
                        }
                        spawnPlayer(player);
                    }
                }

                if (e->isAlive()) {
                    auto bullets = m_eManager.getEntities(TAG_BULLET);
                    for (auto &bullet: bullets) {
                        if (bullet->cShape && collides(e->cShape->m_shape, bullet->cShape->m_shape)) {
                            if (bullet->cLifespan &&
                                (float) bullet->cLifespan->m_currentFrame / (float) bullet->cLifespan->m_totalFrames <
                                0.1) {
                                // Small Enemy is invulnerable for the first third of its life
                                bullet->destroy();
                                continue;
                            }
                            e->destroy();
                            bullet->destroy();

                            for (auto &player: players) {
                                if (player->cScore) {
                                    player->cScore->score += 2 * e->cShape->m_shape.getPointCount();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void GameEngine::sMovement() {
    for (auto &e: m_eManager.getEntities()) {
        if (e->cInput && e->cTransform) {
            e->cTransform->m_velocity.x = 0;
            e->cTransform->m_velocity.y = 0;

            if (e->cInput->m_top) {
                e->cTransform->m_velocity.y += -5.f;
            }

            if (e->cInput->m_right) {
                e->cTransform->m_velocity.x += 5.f;
            }

            if (e->cInput->m_down) {
                e->cTransform->m_velocity.y += 5.f;
            }

            if (e->cInput->m_left) {
                e->cTransform->m_velocity.x += -5.f;
            }
        }

        if (e->cTransform) {
            e->cTransform->m_position += e->cTransform->m_velocity;
        }
    }
}

void GameEngine::sRender() {
    m_window.clear(sf::Color::Black);

    // Background
    float cellWidth = 25.f;
    float cellHeight = 25.f;
    int horizontalCellsNo = (m_window.getSize().x / cellWidth) + 1;
    int verticalCellsNo = (m_window.getSize().y / cellHeight) + 1;

    sf::RectangleShape rect;
    rect.setSize({ cellWidth, cellHeight });
    rect.setFillColor(sf::Color(255, 255, 255, 30));
    for (int i = 0; i < verticalCellsNo; i++) {
        for (int j = 0; j < horizontalCellsNo; j++) {
            rect.setPosition({ j * cellWidth, i * cellHeight });
            if (i % 2 == 0) {
                if (j % 2 == 0) {
                    continue;
                } else {
                    m_window.draw(rect);
                }
            } else {
                if (j % 2 == 0) {
                    m_window.draw(rect);
                } else {
                    continue;
                }
            }
        }
    }

    for (auto &e: m_eManager.getEntities()) {
        if (e->cTransform && e->cShape) {
            e->cShape->m_shape.setOrigin(e->cShape->m_shape.getRadius(), e->cShape->m_shape.getRadius());
            e->cShape->m_shape.rotate(1.5f);
            e->cShape->m_shape.setPosition({e->cTransform->m_position.x + e->cShape->m_shape.getRadius(),
                                            e->cTransform->m_position.y + e->cShape->m_shape.getRadius()});

            if (e->cLifespan) {
                auto fillColor = e->cShape->m_shape.getFillColor();
                auto outColor = e->cShape->m_shape.getFillColor();
                auto alpha = (1 - (float) e->cLifespan->m_currentFrame / e->cLifespan->m_totalFrames) * 255;
                e->cShape->m_shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, alpha));
                e->cShape->m_shape.setOutlineColor(sf::Color(outColor.r, outColor.g, outColor.b, alpha));
            }

            m_window.draw(e->cShape->m_shape);
            e->cShape->m_shape.setOrigin(0, 0); // set origin back to SFML default
        }

        if (e->getTag() == TAG_PLAYER) {
            if (e->cScore) {
                text.setString("SCORE: " + std::to_string(e->cScore->score));
                text.setFillColor(sf::Color::White);
                text.setPosition(25, 25);
                text.setCharacterSize(24);
                m_window.draw(text);
            }

            if (e->cHealth) {
                text.setString("LIVES: " + std::to_string(e->cHealth->lives));
                text.setFillColor(sf::Color::White);
                text.setPosition(m_window.getSize().x - text.getGlobalBounds().getSize().x - 25, 25);
                text.setCharacterSize(24);
                m_window.draw(text);
            }
        }
    }

    ImGui::SFML::Render(m_window);

    m_window.display();
}

void GameEngine::spawnPlayer(const std::shared_ptr<Entity> &existingPlayer) {
    auto player = m_eManager.addEntity(TAG_PLAYER);
    player->cShape = std::make_shared<CShape>();
    player->cShape->m_shape.setPointCount(m_config.playerConfig.vertices);
    player->cShape->m_shape.setRadius(m_config.playerConfig.shapeRadius);
    player->cShape->m_shape.setOutlineThickness(m_config.playerConfig.outThickness);
    player->cShape->m_shape.setOutlineColor(
            sf::Color(m_config.playerConfig.outRed, m_config.playerConfig.outGreen, m_config.playerConfig.outBlue));
    player->cShape->m_shape.setFillColor(
            sf::Color(m_config.playerConfig.fillRed, m_config.playerConfig.fillGreen, m_config.playerConfig.fillBlue));

    player->cTransform = std::make_shared<CTransform>();
    player->cTransform->m_position.x = m_window.getSize().x / 2 - player->cShape->m_shape.getRadius();
    player->cTransform->m_position.y = m_window.getSize().y / 2 - player->cShape->m_shape.getRadius();

    player->cInput = std::make_shared<CInput>();

    player->cHealth = std::make_shared<CHealth>();
    if (existingPlayer && existingPlayer->cHealth) {
        player->cHealth->lives = existingPlayer->cHealth->lives;
    }

    player->cScore = std::make_shared<CScore>();
    if (existingPlayer && existingPlayer->cScore) {
        player->cScore->score = existingPlayer->cScore->score;
    }
}

void GameEngine::sLifeSpan() {
    for (auto &e: m_eManager.getEntities()) {
        if (e->cLifespan) {
            e->cLifespan->m_currentFrame++;

            if (e->cLifespan->m_currentFrame >= e->cLifespan->m_totalFrames) {
                e->destroy();
            }
        }
    }
}

void GameEngine::spawnBullet(sf::Vector2f origin, sf::Vector2f direction) {
    auto e = m_eManager.addEntity(TAG_BULLET);
    e->cTransform = std::make_shared<CTransform>();
    e->cTransform->m_position = origin;
    e->cTransform->m_velocity = m_config.bulletConfig.speed * normalize(direction);

    e->cShape = std::make_shared<CShape>();
    e->cShape->m_shape.setRadius(m_config.bulletConfig.shapeRadius);
    e->cShape->m_shape.setFillColor(
            sf::Color(m_config.bulletConfig.fillRed, m_config.bulletConfig.fillGreen, m_config.bulletConfig.fillBlue));
    e->cShape->m_shape.setOutlineThickness(m_config.bulletConfig.outThickness);
    e->cShape->m_shape.setOutlineColor(
            sf::Color(m_config.bulletConfig.outRed, m_config.bulletConfig.outGreen, m_config.bulletConfig.outBlue));
    e->cShape->m_shape.setPointCount(m_config.bulletConfig.vertices);

    e->cLifespan = std::make_shared<CLifespan>();
    e->cLifespan->m_totalFrames = m_config.bulletConfig.lifespan;
    e->cLifespan->m_currentFrame = 0;
}

sf::Vector2f GameEngine::normalize(const sf::Vector2f &vec) {
    float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return vec / magnitude;
}

float getRand(int min, int max) {
    int range = max - min + 1;
    return std::rand() % range + min;
}

void GameEngine::sSpawner() {
    if (m_currentFrame % m_config.enemyConfig.spawnInterval == 0) {
        spawnEnemy();
    }
}

void GameEngine::spawnEnemy() {
    auto e = m_eManager.addEntity(TAG_ENEMY);

    e->cTransform = std::make_shared<CTransform>();
    e->cTransform->m_position = {
            getRand(0 + m_config.enemyConfig.shapeRadius, m_window.getSize().x - m_config.enemyConfig.shapeRadius),
            getRand(0 + m_config.enemyConfig.shapeRadius, m_window.getSize().y - m_config.enemyConfig.shapeRadius)
    };
    e->cTransform->m_velocity = {
            getRand(m_config.enemyConfig.minSpeed, m_config.enemyConfig.maxSpeed),
            getRand(m_config.enemyConfig.minSpeed, m_config.enemyConfig.maxSpeed)
    };

    e->cShape = std::make_shared<CShape>();
    e->cShape->m_shape.setRadius(m_config.enemyConfig.shapeRadius);
    e->cShape->m_shape.setPointCount(getRand(m_config.enemyConfig.minVertices, m_config.enemyConfig.maxVertices));

    e->cShape->m_shape.setFillColor(sf::Color(
            getRand(0, 255), getRand(0, 255), getRand(0, 255)
    ));
    e->cShape->m_shape.setOutlineThickness(2.f);
    e->cShape->m_shape.setOutlineColor(sf::Color(
            getRand(0, 255), getRand(0, 255), getRand(0, 255)
    ));
}

bool GameEngine::collides(const sf::CircleShape &cA, const sf::CircleShape &cB) {
    float distX = std::abs((cA.getPosition().x + cA.getRadius()) - (cB.getPosition().x + cB.getRadius()));
    float distY = std::abs((cA.getPosition().y + cA.getRadius()) - (cB.getPosition().y + cB.getRadius()));
    float dist = std::sqrt(distX * distX + distY * distY);

    return dist <= (cA.getRadius() + cB.getRadius());
}

void GameEngine::spawnSmallEnemies(const std::shared_ptr<Entity> &parent) {
    if (parent->cShape && parent->cTransform) {
        float theta = 2 * M_PI / parent->cShape->m_shape.getPointCount(); // angle of rotation in radians
        std::cout << "Theta " << theta << std::endl;

        sf::Vector2f direction = {1.f, 0.f};
        for (int i = 0; i < parent->cShape->m_shape.getPointCount(); i++) {
            auto e = m_eManager.addEntity(TAG_SMALL_ENEMY);

            e->cTransform = std::make_shared<CTransform>();
            e->cTransform->m_position = {parent->cTransform->m_position.x + parent->cShape->m_shape.getRadius(),
                                         parent->cTransform->m_position.y + parent->cShape->m_shape.getRadius()};

            float newDirX = std::cos(theta) * direction.x - std::sin(theta) * direction.y;
            float newDirY = std::sin(theta) * direction.x + std::cos(theta) * direction.y;
            direction = {newDirX, newDirY};

            std::cout << "Direction " << direction.x << ", " << direction.y << std::endl;

            e->cTransform->m_velocity = m_config.enemyConfig.minSpeed * normalize(direction);

            e->cShape = std::make_shared<CShape>();
            e->cShape->m_shape.setRadius(m_config.enemyConfig.shapeRadius / 2);
            e->cShape->m_shape.setPointCount(parent->cShape->m_shape.getPointCount());

            e->cShape->m_shape.setFillColor(parent->cShape->m_shape.getFillColor());
            e->cShape->m_shape.setOutlineThickness(parent->cShape->m_shape.getOutlineThickness());
            e->cShape->m_shape.setOutlineColor(parent->cShape->m_shape.getOutlineColor());

            e->cLifespan = std::make_shared<CLifespan>();
            e->cLifespan->m_totalFrames = m_config.enemyConfig.lifespan;
            e->cLifespan->m_currentFrame = 0;
        }
    }
}

void GameEngine::drawUI() {
    ImGui::Begin("UI. Press P for Pause");

    if (ImGui::Button("Systems")) {
        m_config.selectedUITab = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Entity Manager")) {
        m_config.selectedUITab = 1;
    }

    if (m_config.selectedUITab == 0) {
        ImGui::Checkbox("User Input", &m_config.gameConfig.isSUserInputActive);
        ImGui::Checkbox("Collision", &m_config.gameConfig.isSCollisionActive);
        ImGui::Checkbox("Movement", &m_config.gameConfig.isSMovementActive);
        ImGui::Checkbox("Lifespan", &m_config.gameConfig.isSLifespanActive);
        ImGui::Checkbox("Spawner", &m_config.gameConfig.isSSpawnerActive);
    } else if (m_config.selectedUITab == 1) {
        if (ImGui::CollapsingHeader("Entities by Tag")) {
            if (ImGui::CollapsingHeader(TAG_PLAYER)) {
                for (auto& e : m_eManager.getEntities(TAG_PLAYER)) {
                    if (ImGui::Button("D")) {
                        e->destroy();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", e->getTag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("[%.0f, %.0f]", e->cTransform->m_position.x, e->cTransform->m_position.x);
                }
            }
            if (ImGui::CollapsingHeader(TAG_ENEMY)) {
                for (auto& e : m_eManager.getEntities(TAG_ENEMY)) {
                    if (ImGui::Button("D")) {
                        e->destroy();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", e->getTag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("[%.0f, %.0f]", e->cTransform->m_position.x, e->cTransform->m_position.x);
                }
            }
            if (ImGui::CollapsingHeader(TAG_BULLET)) {
                for (auto& e : m_eManager.getEntities(TAG_BULLET)) {
                    if (ImGui::Button("D")) {
                        e->destroy();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", e->getTag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("[%.0f, %.0f]", e->cTransform->m_position.x, e->cTransform->m_position.x);
                }
            }
            if (ImGui::CollapsingHeader(TAG_SMALL_ENEMY)) {
                for (auto& e : m_eManager.getEntities(TAG_SMALL_ENEMY)) {
                    if (ImGui::Button("D")) {
                        e->destroy();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", e->getTag().c_str());
                    ImGui::SameLine();
                    ImGui::Text("[%.0f, %.0f]", e->cTransform->m_position.x, e->cTransform->m_position.x);
                }
            }
        }
        if (ImGui::CollapsingHeader("All Entities")) {
            for (auto& e : m_eManager.getEntities()) {
                if (ImGui::Button("D")) {
                    e->destroy();
                }
                ImGui::SameLine();
                ImGui::Text("%s", e->getTag().c_str());
                ImGui::SameLine();
                ImGui::Text("[%.0f, %.0f]", e->cTransform->m_position.x, e->cTransform->m_position.x);
            }
        }
    }

    ImGui::End();
}

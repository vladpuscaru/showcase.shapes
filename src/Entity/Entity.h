//
// Created by Vlad Puscaru on 07.12.2023.
//

#ifndef COMP4300_ASSIGNMENT2_ENTITY_H
#define COMP4300_ASSIGNMENT2_ENTITY_H

#include <string>
#include "../Components/Components.hpp"

#define TAG_PLAYER "Player"
#define TAG_ENEMY "Enemy"
#define TAG_SMALL_ENEMY "Small_Enemy"
#define TAG_BULLET "Bullet"

class Entity {
private:
    const size_t m_id;
    const std::string m_tag;
    bool m_alive;

    Entity(const std::string& tag, size_t id);
public:
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CLifespan> cLifespan;
    std::shared_ptr<CHealth> cHealth;
    std::shared_ptr<CScore> cScore;

    bool isAlive() const;
    void destroy();
    const std::string& getTag() const;

    friend class EntityManager;
};


#endif //COMP4300_ASSIGNMENT2_ENTITY_H

//
// Created by Vlad Puscaru on 07.12.2023.
//

#include "Entity.h"

Entity::Entity(const std::string &tag, size_t id)
    : m_tag(tag), m_id(id), m_alive(true), cTransform(nullptr), cShape(nullptr), cInput(nullptr), cLifespan(nullptr), cHealth(
        nullptr), cScore(nullptr)
{

}

void Entity::destroy() {
    m_alive = false;
}

const std::string &Entity::getTag() const {
    return m_tag;
}

bool Entity::isAlive() const {
    return m_alive;
}

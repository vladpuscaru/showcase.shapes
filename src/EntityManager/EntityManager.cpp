//
// Created by Vlad Puscaru on 07.12.2023.
//

#include "EntityManager.h"

EntityManager::EntityManager()
    : m_totalEntities(0)
{

}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
    std::shared_ptr<Entity> e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
    m_entitiesToAdd.push_back(e);
    return e;
}

EntityVec &EntityManager::getEntities() {
    return m_entitiesVec;
}

EntityVec &EntityManager::getEntities(const std::string &tag) {
    return m_entitiesMap[tag];
}

void EntityManager::update() {
    for (auto& e : m_entitiesToAdd) {
        m_entitiesVec.push_back(e);
        m_entitiesMap[e->getTag()].push_back(e);
    }
    m_entitiesToAdd.clear();

    removeDeadEntitiesFromVec(m_entitiesVec);
    for (auto& e : m_entitiesMap) {
        removeDeadEntitiesFromVec(e.second);
    }
}

void EntityManager::removeDeadEntitiesFromVec(EntityVec &vec) {
    EntityVec entitiesToRemove;
    for (auto& e : vec) {
        if (!e->m_alive) {
            entitiesToRemove.push_back(e);
        }
    }

    for (auto& e : entitiesToRemove) {
        auto it = vec.begin();
        while (it != vec.end()) {
            if ((*it)->m_id == e->m_id) {
                break;
            }
            it++;
        }
        if ((*it)->m_id == e->m_id) {
            vec.erase(it);
        }
    }
}
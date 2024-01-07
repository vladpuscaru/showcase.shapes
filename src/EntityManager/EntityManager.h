//
// Created by Vlad Puscaru on 07.12.2023.
//

#ifndef COMP4300_ASSIGNMENT2_ENTITYMANAGER_H
#define COMP4300_ASSIGNMENT2_ENTITYMANAGER_H

#include <vector>
#include <map>
#include "../Entity/Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, std::vector<std::shared_ptr<Entity>>> EntityMap;

class EntityManager {
private:
    EntityVec m_entitiesVec;
    EntityVec m_entitiesToAdd;
    EntityMap m_entitiesMap;
    size_t m_totalEntities;

    void removeDeadEntitiesFromVec(EntityVec& vec);
public:
    EntityManager();
    void update();
    std::shared_ptr<Entity> addEntity(const std::string& tag);
    EntityVec& getEntities();
    EntityVec& getEntities(const std::string& tag);
};


#endif //COMP4300_ASSIGNMENT2_ENTITYMANAGER_H

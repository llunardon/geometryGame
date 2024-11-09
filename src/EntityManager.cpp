#include <EntityManager.h>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
    for (auto e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->m_tag].push_back(e);
    }
    m_entitiesToAdd.clear();

    removeDeadEntities(m_entities);
    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
    for (size_t i = 0; i < vec.size(); )
    {
        if (!vec[i]->isActive())
        {
            vec.erase(vec.begin() + i);
        } else
        {
            i++;
        }
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    // TODO: correct this, return the correct vector from the EntityMap
    return m_entityMap[tag];
}
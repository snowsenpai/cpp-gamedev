#include "Entity.h"
#include "EntityManager.h"
#include <memory>

EntityManager::EntityManager() {}

// called at the beginning of each frame
void EntityManager::update()
{
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.emplace_back(e);
		m_entityMap[e->tag()].emplace_back(e);
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
	EntityVec::iterator newEnd = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& e) { return !e->isActive(); });
	vec.erase(newEnd, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.emplace_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}
#include "Entity.h"
#include "EntityManager.h"
#include <memory>

EntityManager::EntityManager() {}

// called at the beginning of each frame
void EntityManager::update()
{
	// TODO add entites from m_entitiesToAdd into m_entities and m_entityMap
	// TODO remove dead entities from m_entities and m_entityMap
	// clear m_entitiesToAdd
}

void EntityManager::removeDeadEntities(EntityVec& vec) 
{
	// TODO remove all dead entities from the input vector that is called by the update function
	// handle iterator invalidation, try out std::remove_if
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
	return m_entityMap[tag];
}
#pragma once

#include "Components.h"
#include <string>
#include <memory>

class Entity
{
	friend class EntityManager;

	size_t		m_id		= 0;
	bool		m_active	= true;
	std::string m_tag		= "default";

	Entity(const size_t id, const std::string& tag);

public:
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifeSpan> cLifeSpan;

	bool isActive() const;
	const std::string& tag() const;
	const size_t id() const;
	
	void destroy();
};
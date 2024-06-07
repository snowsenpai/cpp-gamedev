#include "Game.h"
#include <iostream>
#include <fstream>
#include <memory>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	std::ifstream configFile(config);
	if (!configFile.is_open())
	{
		std::cout << "failed to load config file: " << config << "\n";
	}

	std::string configType;
	while (configFile >> configType)
	{
		if (configType == "Window")
		{
			WindowConfig windowConfig{};
			configFile >> windowConfig.W >> windowConfig.H >> windowConfig.FL >> windowConfig.FS;

			if (windowConfig.FS)
			{
				// fix? full screen with title bar, can be maximized
				sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
				m_window.create(desktop, "Geom", sf::Style::Default);
			}
			else
			{
				// fix: when maximized display boarders are outside desktop and shapes appear streached
				m_window.create(sf::VideoMode(windowConfig.W, windowConfig.H), "Geom");
			}

			m_window.setFramerateLimit(windowConfig.FL);
		}
		else if (configType == "Font")
		{
			configFile >> m_fontConfig.F >> m_fontConfig.S >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;
		}
		else if (configType == "Player")
		{
			configFile >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}
		else if (configType == "Enemy")
		{
			configFile >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		else if (configType == "Bullet")
		{
			configFile >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}
	spawnPlayer();
}

void Game::run()
{
	// TODO add pause funcionality
	// some systems should function while paused (rendering)
	// some sysyems shouldn't (movement, input)
	while (m_running)
	{
		m_entities.update();
		
		if (!m_paused)
		{
			sUserInput();
			sMovement();
			sEnemySpawner();
			sCollision();
			sLifeSpan();
		}
		sRender();

		m_currentFrame++;
	}
	m_window.close();
}

void Game::setPaused(bool paused)
{
	// TODO pollEvents and listen for a key press? or t = !t
}

void Game::spawnPlayer()
{
	auto player = m_entities.addEntity("player");
	
	player->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), 0.0f);

	player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
	
	player->cInput = std::make_shared<CInput>();

	player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = player;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	// TODO

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns a bullet from a given entity(origin) to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto bullet = m_entities.addEntity("bullet");
	
	bullet->cLifeSpan = std::make_shared<CLifeSpan>(m_bulletConfig.L);
	
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FB, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

	Vec2 diffVec = target - entity->cTransform->pos; // if reverse angle will be wrong

	float angle = diffVec.angle();

	Vec2 bulletVel = Vec2::velocity(m_bulletConfig.S, angle);
	
	float entityRadius = entity->cShape->circle.getRadius();
	
	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, bulletVel, 0.0f);
}

// spawns the small enemies when a big one (input Entity* e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	// TODO
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO
	// options:
	// ricochet bullet off n enemies
}

void Game::sRender()
{
	m_window.clear();

	for (const auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->angle += 0.25f;
		e->cShape->circle.rotate(e->cTransform->angle);

		m_window.draw(e->cShape->circle);
	}
	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}
		// if key pressed is esc m_running false

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W :
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default:
				break;
			}
		}

		// mouse events
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2((float)event.mouseButton.x, (float)event.mouseButton.y));
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "mouse right button was pressed" << "\n";
				// spawnSpecialWeapon()
			}
		}
	}
}

void Game::sMovement()
{
	// player movement
	
	// reset velocity on each frame
	m_player->cTransform->velocity = { 0.0f, 0.0f };

	float speed = 8.0f;
	if (m_player->cInput->up)
	{
		m_player->cTransform->velocity.y = -speed;
	}
	else if (m_player->cInput->down)
	{
		m_player->cTransform->velocity.y = speed;
	}
	else if (m_player->cInput->left)
	{
		m_player->cTransform->velocity.x = -speed;
	}
	else if (m_player->cInput->right)
	{
		m_player->cTransform->velocity.x = speed;
	}

	m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
	
	// bullets
	for (const auto& bullet : m_entities.getEntities("bullet"))
	{
		bullet->cTransform->pos.x += bullet->cTransform->velocity.x;
		bullet->cTransform->pos.y += bullet->cTransform->velocity.y;
	}
	// TODO enemy movement
}

void Game::sEnemySpawner()
{
	// TODO
}

void Game::sCollision()
{
	unsigned int windowWidth = m_window.getSize().x;
	unsigned int windowHeight = m_window.getSize().y;

	float playerPosX = m_player->cShape->circle.getPosition().x;
	float playerPosY = m_player->cShape->circle.getPosition().y;

	float playerCR = m_player->cCollision->radius;

	// prevents player rendering from choking
	float tOffset = 0.05f;

	// player x window collision
	if (playerPosX + playerCR >= windowWidth) // right
	{
		// to restrict player movement, set a windowColl.right to true and check in sMovement along with player.right
		//m_player->cInput->right = false; will be over written by sUserInput in the next frame
		
		// teleport
		m_player->cTransform->pos.x = m_player->cShape->circle.getRadius() + tOffset;
	}
	else if (playerPosY + playerCR >= windowHeight) // bottom
	{
		m_player->cTransform->pos.y = m_player->cShape->circle.getRadius() + tOffset;
	}
	// when game starts player pos{x, y} might be 0,0 (default)
	else if (playerPosX && playerPosX - playerCR <= 0.0f) // left
	{
		m_player->cTransform->pos.x = windowWidth - m_player->cShape->circle.getRadius() - tOffset;
	}
	else if (playerPosY && playerPosY - playerCR <= 0.0f) // top
	{
		m_player->cTransform->pos.y = windowHeight - m_player->cShape->circle.getRadius() - tOffset;
	}
	// TODO, bullet x enemy, player x enemy collisions, 
}

void Game::sLifeSpan()
{
	// TODP for all entities
	// if no lifespan component, skip (continue)
	// if remaining lifespan > 0 , --1
	// if lifespan and is alive, reduce shape alpha
	// if lifespan and remaining = 0, destroy
	
	for (const auto& bullet : m_entities.getEntities("bullet"))
	{
		int fade = --bullet->cLifeSpan->remaining;
		
		bullet->cShape->circle.setFillColor(sf::Color(m_bulletConfig.FR, m_bulletConfig.FB, m_bulletConfig.FB, fade));
		
		bullet->cShape->circle.setOutlineColor(sf::Color(m_bulletConfig.OR, m_bulletConfig.OB, m_bulletConfig.OB, fade));

		if (0 == fade) bullet->destroy();
	}
}

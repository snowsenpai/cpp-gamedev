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
				// full screen
				sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
				m_window.create(desktop, "Geom", sf::Style::Default);
			}
			else
			{
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

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// TODO
}

// spawns the small enemies when a big one (input Entity* e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	// TODO
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO last
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
	}
}

void Game::sMovement()
{
	// player movement
	// reset velocity on each frame
	m_player->cTransform->velocity = { 0, 0 };

	if (m_player->cInput->up)
	{
		m_player->cTransform->velocity.y = -5.f;
	}
	else if (m_player->cInput->down)
	{
		m_player->cTransform->velocity.y = 5.f;
	}
	else if (m_player->cInput->left)
	{
		m_player->cTransform->velocity.x = -5.f;
	}
	else if (m_player->cInput->right)
	{
		m_player->cTransform->velocity.x = 5.f;
	}
	// TODO? smooth diagonal movement when 2 perpendicular directions are true

	m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
	
	// TODO enemy and bullet movement
}

void Game::sEnemySpawner()
{
	// TODO
}

void Game::sCollision()
{
	// TODO
}

void Game::sLifeSpan()
{
	// TODO
}

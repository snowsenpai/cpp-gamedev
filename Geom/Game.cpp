#include "Game.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <type_traits>
#include <string>

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
				// TODO fix: display window in "maximized" mode not default mode
				// use sf::Style::FullScreen and have a key to restore down
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
			
			sf::Font font;
			if (!font.loadFromFile(m_fontConfig.F))
			{
				std::cout << "failed to load font from file path: " << m_fontConfig.F << "\n";
			}
			m_font = font;
		}
		else if (configType == "Player")
		{
			configFile >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}
		else if (configType == "Enemy")
		{
			configFile >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SP;
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
	while (m_running)
	{
		m_entities.update();

		sUserInput();
		if (!m_paused)
		{
			sCollision();
			sScore();
			sMovement();
			sEnemySpawner();
			sLifeSpan();
			m_currentFrame++;
		}
		sRender();
		
	}
	m_window.close();
}

void Game::setPaused(bool paused)
{
	// TODO when paused display a "paused" text to the player
	m_paused = paused;
}

void Game::spawnPlayer()
{
	auto player = m_entities.addEntity("player");

	float posX = static_cast<float>(m_window.getSize().x / 2);
	float posY = static_cast<float>(m_window.getSize().y / 2);
	
	player->cTransform = std::make_shared<CTransform>(Vec2(posX, posY), Vec2(1.0f, 1.0f), 0.0f);

	player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
	
	player->cInput = std::make_shared<CInput>();

	player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = player;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	float windowX = static_cast<float>(m_window.getSize().x);
	float windowY = static_cast<float>(m_window.getSize().y);

	float shapeRadius = m_enemyConfig.SR;
	float minPos = 0.0f + shapeRadius;

	float posX = rng(minPos, windowX - shapeRadius);
	float posY = rng(minPos, windowY - shapeRadius);

	float destX = rng(0.0f, windowX);
	float destY = rng(0.0f, windowY);

	Vec2 origin(posX, posY);
	Vec2 dest(destX, destY);

	Vec2 diffVec = dest - origin;

	float angle = diffVec.angle();

	float speed = rng(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

	Vec2 vel = Vec2::velocity(speed, angle);

	int vertices = rng(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	int minColor = 0;
	int maxColor = 255;

	int fr = rng(minColor, maxColor);
	int fg = rng(minColor, maxColor);
	int fb = rng(minColor, maxColor);

	sf::Color fill(fr, fg, fb);
	sf::Color outline(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

	auto enemy = m_entities.addEntity("enemy");

	enemy->cShape = std::make_shared<CShape>(shapeRadius, vertices, fill, outline, m_enemyConfig.OT);
	
	// enemy needs an angle for it velocity
	enemy->cTransform = std::make_shared<CTransform>(origin, vel, 0.0f);
	
	enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	int points = vertices * 100;
	enemy->cScore = std::make_shared<CScore>(points);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns a bullet from a given entity(origin) to a target location
void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2& target)
{
	auto bullet = m_entities.addEntity("bullet");
	
	bullet->cLifeSpan = std::make_shared<CLifeSpan>(m_bulletConfig.L);
	
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FB, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

	Vec2 diffVec = target - entity->cTransform->pos;

	float angle = diffVec.angle();

	Vec2 bulletVel = Vec2::velocity(m_bulletConfig.S, angle);
	
	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, bulletVel, 0.0f);
}

// spawns the small enemies when a big one (input Entity* e) dies
void Game::spawnSmallEnemies(const std::shared_ptr<Entity>& entity)
{
	int vertices = static_cast<int>(entity->cShape->circle.getPointCount());
	int angleOffset = 360 / vertices;

	float smallSR = entity->cShape->circle.getRadius() / 3.0f;
	int smallLS = 40;

	Vec2 smallPos = entity->cTransform->pos;

	float speed = rng(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
	
	sf::Color fill = entity->cShape->circle.getFillColor();
	sf::Color outline = entity->cShape->circle.getOutlineColor();
	float thickness = entity->cShape->circle.getOutlineThickness();

	for (int i = 0; i < vertices; i++)
	{
		float angle = static_cast<float>(angleOffset * i);
		Vec2 vel = Vec2::velocity(speed, angle);

		auto smallEnemy = m_entities.addEntity("smallEnemy");
		
		smallEnemy->cLifeSpan = std::make_shared<CLifeSpan>(smallLS);
		smallEnemy->cTransform = std::make_shared<CTransform>(smallPos, vel, 0.0f);

		smallEnemy->cShape = std::make_shared<CShape>(smallSR, vertices, fill, outline, thickness);
	}
}

void Game::spawnSpecialWeapon(const std::shared_ptr<Entity>& entity, const Vec2& target)
{
	Vec2 distanceVec = target - entity->cTransform->pos;
	float angle = distanceVec.angle();
	Vec2 weaponVel = Vec2::velocity(22.5f, angle);
	auto weapon = m_entities.addEntity("specialWeapon");
	
	weapon->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, weaponVel, 0.0f);
	weapon->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	weapon->cLifeSpan = std::make_shared<CLifeSpan>(200);

	sf::Color white(255, 255, 255);
	sf::Color black(0, 0, 0);
	weapon->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, black, white, m_bulletConfig.OT);
}

void Game::sRender()
{
	m_window.clear();

	m_window.draw(m_text);

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
		if (event.type == sf::Event::LostFocus)
		{
			// the event hander(thread that calls pollEvent) is still blocked when window title bar is clicked/dragged
			// sf::Event::LostFocus is triggered for other cases but not for title bar clicks
			setPaused(true);
		}
		if (event.type == sf::Event::Resized)
		{
			// pause game play
			setPaused(true);
			m_window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
		}
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::P:
				setPaused(!m_paused);
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			default:
				break;
			}
		}

		if (!m_paused)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::W:
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
					spawnBullet(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
				}

				if (event.mouseButton.button == sf::Mouse::Right)
				{
					// TODO limit specialWeapon usage
					spawnSpecialWeapon(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
				}
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
	for (const auto& weapon : m_entities.getEntities("specialWeapon"))
	{
		weapon->cTransform->pos.x += weapon->cTransform->velocity.x;
		weapon->cTransform->pos.y += weapon->cTransform->velocity.y;
	}

	// enemies
	for (const auto& enemy : m_entities.getEntities("enemy"))
	{
		enemy->cTransform->pos.x += enemy->cTransform->velocity.x;
		enemy->cTransform->pos.y += enemy->cTransform->velocity.y;
	}
	// small enemies
	for (const auto& smallEnemy : m_entities.getEntities("smallEnemy"))
	{
		smallEnemy->cTransform->pos.x += smallEnemy->cTransform->velocity.x;
		smallEnemy->cTransform->pos.y += smallEnemy->cTransform->velocity.y;
	}
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SP)
	{
		spawnEnemy();
	}
}

void Game::sCollision()
{
	unsigned int windowWidth = m_window.getSize().x;
	unsigned int windowHeight = m_window.getSize().y;

	float playerPosX = m_player->cTransform->pos.x;
	float playerPosY = m_player->cTransform->pos.y;

	float playerCR = m_player->cCollision->radius;

	// prevents player rendering from choking
	float tOffset = 0.05f;
	float playerSR = m_player->cShape->circle.getRadius();

	// player x window collision
	if (playerPosX + playerCR >= windowWidth) // right
	{
		// restrict player movement?, set a windowColl.right to true and check in sMovement along with player.right
		//m_player->cInput->right = false; will be over written by sUserInput in the next frame
		
		// teleport
		m_player->cTransform->pos.x = playerSR + tOffset;
	}
	else if (playerPosY + playerCR >= windowHeight) // bottom
	{
		m_player->cTransform->pos.y = playerSR + tOffset;
	}
	// when game starts player pos{x, y} might be 0,0 (default)
	else if (playerPosX && playerPosX - playerCR <= 0.0f) // left
	{
		m_player->cTransform->pos.x = windowWidth - playerSR - tOffset;
	}
	else if (playerPosY && playerPosY - playerCR <= 0.0f) // top
	{
		m_player->cTransform->pos.y = windowHeight - playerSR - tOffset;
	}

	// bullet x enemy collision
	for (const auto& bullet : m_entities.getEntities("bullet"))
	{
		for (const auto& enemy : m_entities.getEntities("enemy"))
		{
			float enemyCR = enemy->cCollision->radius;
			float bulletCR = bullet->cCollision->radius;

			Vec2 diffVec = bullet->cTransform->pos - enemy->cTransform->pos;
			float distSqr = diffVec.x * diffVec.x + diffVec.y * diffVec.y;
			
			float radiusSum = bulletCR + enemyCR;
			float radiusSumSqr = radiusSum * radiusSum;

			if (distSqr < radiusSumSqr)
			{
				m_score += enemy->cScore->score;
				enemy->destroy();
				bullet->destroy();
				spawnSmallEnemies(enemy);
			}
		}
	}

	for (const auto& weapon : m_entities.getEntities("specialWeapon"))
	{
		float weaponCR = weapon->cCollision->radius;

		float weaponPosX = weapon->cTransform->pos.x;
		float weaponPosY = weapon->cTransform->pos.y;
		
		// speacialWeapon x window collision
		float newVelX = weapon->cTransform->velocity.x * -1.0f;
		float newVelY = weapon->cTransform->velocity.y * -1.0f;

		if (weaponPosX + weaponCR >= windowWidth) // right collison, bounce left
		{
			weapon->cTransform->velocity.x = newVelX;
		}
		else if (weaponPosY + weaponCR >= windowHeight) // bottom collison, bounce up
		{
			weapon->cTransform->velocity.y = newVelY;
		}
		else if (weaponPosX && weaponPosX - weaponCR <= 0.0f) // left collision, bounce right
		{
			weapon->cTransform->velocity.x = newVelX;
		}
		else if (weaponPosY && weaponPosY - weaponCR <= 0.0f) // top collision, bounce down
		{
			weapon->cTransform->velocity.y = newVelY;
		}

		// speacialWeapon x enemy collision
		for (const auto& enemy : m_entities.getEntities("enemy"))
		{
			float enemyCR = enemy->cCollision->radius;

			Vec2 diffVec = weapon->cTransform->pos - enemy->cTransform->pos;
			float distSqr = diffVec.x * diffVec.x + diffVec.y * diffVec.y;
			
			float radiusSum = weaponCR + enemyCR;
			float radiusSumSqr = radiusSum * radiusSum;

			if (distSqr < radiusSumSqr)
			{
				m_score += enemy->cScore->score;
				enemy->destroy();
				spawnSmallEnemies(enemy);
			}
		}
	}

	for (const auto& enemy : m_entities.getEntities("enemy"))
	{
		float enemyCR = enemy->cCollision->radius;

		float enemyPosX = enemy->cTransform->pos.x;
		float enemyPosY = enemy->cTransform->pos.y;

		// player x enemy collision
		Vec2 diffVec = enemy->cTransform->pos - m_player->cTransform->pos;

		float distSqr = diffVec.x * diffVec.x + diffVec.y * diffVec.y;
		float radiusSum = playerCR + enemyCR;
		float radiusSumSqr = radiusSum * radiusSum;
		
		if (distSqr < radiusSumSqr)
		{
			m_player->destroy();
			enemy->destroy();
			spawnPlayer();
			spawnSmallEnemies(enemy);
		}

		float newVelX = enemy->cTransform->velocity.x * - 1.0f;
		float newVelY = enemy->cTransform->velocity.y * - 1.0f;

		// enemy x window collision
		if (enemyPosX + enemyCR >= windowWidth) // right collison, bounce left
		{
			enemy->cTransform->velocity.x = newVelX;
		}
		else if (enemyPosY + enemyCR >= windowHeight) // bottom collison, bounce up
		{
			enemy->cTransform->velocity.y = newVelY;
		}
		else if (enemyPosX && enemyPosX - enemyCR <= 0.0f) // left collision, bounce right
		{
			enemy->cTransform->velocity.x = newVelX;
		}
		else if (enemyPosY && enemyPosY - enemyCR <= 0.0f) // top collision, bounce down
		{
			enemy->cTransform->velocity.y = newVelY;
		}
	}
}

void Game::sLifeSpan()
{
	// looping through EntittyVec is slow, takes longer to destroy entity
	int alphaFade = 6;
	for (const auto& bullet : m_entities.getEntities("bullet"))
	{
		int fade = --bullet->cLifeSpan->remaining;

		sf::Color bulletFillColor = bullet->cShape->circle.getFillColor();
		sf::Color bulletOutlineColor = bullet->cShape->circle.getOutlineColor();
		bulletFillColor.a -= alphaFade;
		bulletOutlineColor.a -= alphaFade;

		bullet->cShape->circle.setFillColor(bulletFillColor);
		bullet->cShape->circle.setOutlineColor(bulletOutlineColor);

		if (0 == fade) bullet->destroy();
	}
	for (const auto& smallEnemy : m_entities.getEntities("smallEnemy"))
	{
		int fade = --smallEnemy->cLifeSpan->remaining;

		sf::Color enemyFillColor = smallEnemy->cShape->circle.getFillColor();
		sf::Color enemyOutlineColor = smallEnemy->cShape->circle.getOutlineColor();
		enemyFillColor.a -= alphaFade;
		enemyOutlineColor.a -= alphaFade;

		smallEnemy->cShape->circle.setFillColor(enemyFillColor);
		smallEnemy->cShape->circle.setOutlineColor(enemyOutlineColor);

		if (0 == fade) smallEnemy->destroy();
	}
	for (const auto& weapon : m_entities.getEntities("specialWeapon"))
	{
		int fade = --weapon->cLifeSpan->remaining;

		sf::Color weaponFillColor = weapon->cShape->circle.getFillColor();
		sf::Color weaponOutlineColor = weapon->cShape->circle.getOutlineColor();
		--weaponFillColor.a;
		--weaponOutlineColor.a;

		weapon->cShape->circle.setFillColor(weaponFillColor);
		weapon->cShape->circle.setOutlineColor(weaponOutlineColor);

		if (0 == fade) weapon->destroy();
	}
}

void Game::sScore()
{
	sf::Text score;
	score.setFont(m_font);
	score.setCharacterSize(m_fontConfig.S);
	score.setFillColor(sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
	
	std::string text = "Score: ";
	std::string result = text + std::to_string(m_score);
	
	score.setString(result);

	m_text = score;
}

template <typename T>
T Game::rng(T min, T max)
{
	static_assert(std::is_arithmetic<T>::value, "Template parameter must be an arithmetic type");

	std::random_device rd;
	std::mt19937 gen(rd());

	if constexpr (std::is_integral<T>::value)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(gen);
	}
	else if constexpr (std::is_floating_point<T>::value)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(gen);
	}
}
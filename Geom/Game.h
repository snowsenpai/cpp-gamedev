#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>

struct WindowConfig { int W, H, FL, FS; };
struct FontConfig { std::string F = "default"; int S = 0, R = 0, G = 0, B = 0; };
struct PlayerConfig { int FR, FG, FB, OR, OG, OB, V; float SR, CR, OT, S; };
struct EnemyConfig { int OR, OG, OB, VMIN, VMAX, L, SP; float SR, CR, OT, SMIN, SMAX; };
struct BulletConfig { int FR, FG, FB, OR, OG, OB, V, L; float SR, CR, OT, S; };

class Game
{
	EntityManager m_entities;
	
	sf::RenderWindow m_window;
	sf::Font m_font;
	sf::Text m_text;
	
	FontConfig m_fontConfig;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	
	bool m_paused = false;
	bool m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setPaused(bool paused);

	template <typename T>
	T rng(T min, T max);

	void sRender();
	void sUserInput();
	void sMovement();
	void sEnemySpawner();
	void sCollision();
	void sLifeSpan();
	void sScore();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(const std::shared_ptr<Entity>& entity);
	void spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2& mousePos);
	void spawnSpecialWeapon(const std::shared_ptr<Entity>& entity, const Vec2& target);

public:
	Game(const std::string& config);

	void run();
};
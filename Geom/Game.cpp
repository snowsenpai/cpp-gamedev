#include "Game.h"
#include <iostream>
#include <fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	// TODO read config file
	// setup window
	// spawn player
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
			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
		}

		sRender();

		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	// TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	// TODO read player config
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
	// TODO
}

void Game::sUserInput()
{
	// TODO
}

void Game::sMovement()
{
	// TODO
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

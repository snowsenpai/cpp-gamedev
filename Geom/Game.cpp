#include "Game.h"
#include <iostream>
#include <fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	// TODO
}

void Game::run()
{
	// TODO
}

void Game::setPaused(bool paused)
{
	// TODO
}

void Game::spawnPlayer()
{
	// TODO
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	// TODO

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

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

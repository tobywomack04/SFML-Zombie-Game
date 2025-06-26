#include "zombie.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

using namespace std;

// Spawns zombies of different types
void Zombie::spawn(float startX, float startY, int type, int seed)
{
	m_Type = type;

	switch (type)
	{
	case 0:
		// Bloater zombie
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloater.png"));

		m_Speed = 40;
		m_Health = 5;
		break;

	case 1:
		// Chaser zombie
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));

		m_Speed = 70;
		m_Health = 1;
		break;

	case 2:
		// Crawler zombie
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));

		m_Speed = 20;
		m_Health = 3;
		break;

	case 3:
		// Semi-invisible zombie
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/invisZed.png"));

		m_Speed = 80;
		m_Health = 2;
		break;
	}

	// Modify the speed to make the zombie unique
	// Every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);
	// Somewhere between 80 and 100
	float modifier = (rand() % MAX_VARRIANCE) + OFFSET;
	// Express as a fraction of 1
	modifier /= 100; // Now equals between .7 and 1
	m_Speed *= modifier;

	m_Position.x = startX;
	m_Position.y = startY;

	m_Sprite.setOrigin(25, 25);
	m_Sprite.setPosition(m_Position);
}

// Returns whether a zombie is alive or not after being hit
bool Zombie::hit(int damage)
{
	m_Health -= damage;

	if (m_Health <= 0)
	{
		// dead
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture(
			"graphics/blood.png"));

		return true;
	}

	// injured but not dead yet
	return false;
}

// Returns a boolean stating whether the zombie is alive or not
bool Zombie::isAlive()
{
	return m_Alive;
}

// Returns zombie poition on the map
FloatRect Zombie::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

// Returns zombie sprite
Sprite Zombie::getSprite()
{
	return m_Sprite;
}

void Zombie::update(float elapsedTime,
	Vector2f playerLocation)
{
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	// Update the zombie position variables
	if (playerX > m_Position.x)
	{
		m_Position.x = m_Position.x +
			m_Speed * elapsedTime;
	}

	if (playerY > m_Position.y)
	{
		m_Position.y = m_Position.y +
			m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x)
	{
		m_Position.x = m_Position.x -
			m_Speed * elapsedTime;
	}

	if (playerY < m_Position.y)
	{
		m_Position.y = m_Position.y -
			m_Speed * elapsedTime;
	}
	
	// Move the sprite
	m_Sprite.setPosition(m_Position);

	// Face the sprite in the correct direction
	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;
	m_Sprite.setRotation(angle);
}

// Returns zombie's x position
float Zombie::getXPos() {
	return m_Position.x;
}

// Returns zombie's y position
float Zombie::getYPos() {
	return m_Position.y;
}

// Returns zombie's type
int Zombie::getType() {
	return m_Type;
}
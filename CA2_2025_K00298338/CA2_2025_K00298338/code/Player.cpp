#include "player.h"
#include "TextureHolder.h"
#include <iostream>

// Player constructor
Player::Player()
{
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;

	// Associate a texture with the sprite
	// !!Watch this space!!
	m_Sprite = Sprite(TextureHolder::GetTexture("graphics/player.png"));

	// Set the origin of the sprite to the centre for smooth rotation
	m_Sprite.setOrigin(25, 25);

	metalCount = 0;
	damage = 1;
}

// Resets players stats to default
void Player::resetPlayerStats()
{
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;
	metalCount = 0;
	damage = 1;
}

// Manages the player spawning into the map
void Player::spawn(IntRect arena, Vector2f resolution, int tileSize)
{
	// Place the player in the middle of the arena
	m_Position.x = arena.width / 2;
	m_Position.y = arena.height / 2;

	// Copy the details of the arena to the player's m_Arena
	m_Arena.left = arena.left;
	m_Arena.width = arena.width;
	m_Arena.top = arena.top;
	m_Arena.height = arena.height;

	// Remember how big the tiles are in this arena
	m_TileSize = tileSize;

	// Stores the resolution for future use
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;
}

// Returns when the player was last hit
Time Player::getLastHitTime()
{
	return m_LastHit;
}

// Manages player getting hit
bool Player::hit(Time timeHit)
{
	if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200)// 2 tenths of second
	{
		m_LastHit = timeHit;
		m_Health -= 10;
		return true;
	}
	else
	{
		return false;
	}

}

// Returns the player's current location
FloatRect Player::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

// Returns the centre of the player
Vector2f Player::getCenter()
{
	return m_Position;
}

// Returns the rotation of the player
float Player::getRotation()
{
	return m_Sprite.getRotation();
}

// Returns the player sprite
Sprite Player::getSprite()
{
	return m_Sprite;
}

// Returns the players current health value
int Player::getHealth()
{
	return m_Health;
}

// Player movement functions
void Player::moveLeft()
{
	m_LeftPressed = true;
}

void Player::moveRight()
{
	m_RightPressed = true;
}

void Player::moveUp()
{
	m_UpPressed = true;
}

void Player::moveDown()
{
	m_DownPressed = true;
}

void Player::stopLeft()
{
	m_LeftPressed = false;
}

void Player::stopRight()
{
	m_RightPressed = false;
}

void Player::stopUp()
{
	m_UpPressed = false;
}

void Player::stopDown()
{
	m_DownPressed = false;
}

// Multiplies the player's speed to simulate dodging
void Player::startDodge() {
	m_Speed = m_Speed * 2;
}

// Returns player's speed to original value to stop dodge
void Player::stopDodge() {
	m_Speed = m_Speed / 2;
}

// Updates the player's movement
void Player::update(float elapsedTime, Vector2i mousePosition)
{
	if (m_UpPressed)
	{
		m_Position.y -= m_Speed * elapsedTime;
	}

	if (m_DownPressed)
	{
		m_Position.y += m_Speed * elapsedTime;
	}

	if (m_RightPressed)
	{
		m_Position.x += m_Speed * elapsedTime;
	}

	if (m_LeftPressed)
	{
		m_Position.x -= m_Speed * elapsedTime;
	}

	m_Sprite.setPosition(m_Position);

	// Keep the player in the arena
	if (m_Position.x > m_Arena.width - m_TileSize)
	{
		m_Position.x = m_Arena.width - m_TileSize;
	}

	if (m_Position.x < m_Arena.left + m_TileSize)
	{
		m_Position.x = m_Arena.left + m_TileSize;
	}

	if (m_Position.y > m_Arena.height - m_TileSize)
	{
		m_Position.y = m_Arena.height - m_TileSize;
	}

	if (m_Position.y < m_Arena.top + m_TileSize)
	{
		m_Position.y = m_Arena.top + m_TileSize;
	}

	// Calculate the angle the player is facing
	float angle = (atan2(mousePosition.y - m_Resolution.y / 2, mousePosition.x - m_Resolution.x / 2) * 180) / 3.141;

	m_Sprite.setRotation(angle);
}

void Player::upgradeSpeed()
{
	// 20% speed upgrade
	m_Speed += (START_SPEED * .2);
}

void Player::upgradeHealth()
{
	// 20% max health upgrade
	m_MaxHealth += (START_HEALTH * .2);
}

// Increases the player's health value
void Player::increaseHealthLevel(int amount)
{
	m_Health += amount;

	// But not beyond the maximum
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}

// Increases the number of metal sheets the player is carrying by 1
void Player::increaseMetalCount() {
	metalCount++;
}

// Returns the number of metal sheets that the player is currently holding
int Player::getMetalCount() {
	return metalCount;
}

// Decreases the number of metal sheets the player is holding by 1
void Player::decreaseMetalCount() {
	metalCount--;
}

// Increases the player's damage by 1
void Player::incrementDamage() {
	damage++;
}

// Returns the player's current damage value
int Player::getDamage()
{
	return damage;
}


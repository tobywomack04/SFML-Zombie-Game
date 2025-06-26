#include <sstream>
#include <fstream>
#include <SFML/Audio.hpp>
#include "SoundManager.h"
#include <iostream>
#include<list>

using namespace std;
using namespace sf;

// SoundManager constructor
SoundManager::SoundManager() {
	// Loads hit sound file into buffer
	hitBuffer.loadFromFile("sound/hit.wav");
	hit.setBuffer(hitBuffer);

	// Loads splat sound file into buffer
	splatBuffer.loadFromFile("sound/splat.wav");
	splat.setBuffer(splatBuffer);

	// Loads shoot sound file into buffer
	shootBuffer.loadFromFile("sound/shoot.wav");
	shoot.setBuffer(shootBuffer);

	// Loads reload sound file into buffer
	reloadBuffer.loadFromFile("sound/reload.wav");
	reload.setBuffer(reloadBuffer);

	// Loads reload failed sound file into buffer
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	reloadFailed.setBuffer(reloadFailedBuffer);

	// Loads powerup pickup sound file into buffer
	powerupBuffer.loadFromFile("sound/powerup.wav");
	powerup.setBuffer(powerupBuffer);

	// Loads pickup sound file into buffer
	pickupBuffer.loadFromFile("sound/pickup.wav");
	pickup.setBuffer(pickupBuffer);

	// Loads freeze sound file into buffer
	freezeBuffer.loadFromFile("sound/freeze.wav");
	freezing.setBuffer(freezeBuffer);

	// Loads gun level up sound file into buffer
	metalBuffer.loadFromFile("sound/metal.wav");
	gunLevelUp.setBuffer(metalBuffer);

	// Loads soundtrack1 sound file into buffer
	soundtrack1Buffer.loadFromFile("sound/soundtrack1.wav");
	soundtrack1.setBuffer(soundtrack1Buffer);

	// Loads soundtrack2 sound file into buffer
	soundtrack2Buffer.loadFromFile("sound/soundtrack2.wav");
	soundtrack2.setBuffer(soundtrack2Buffer);

	// Seeding randomiser
	srand(time(0));
}

// Play reload sound function
void SoundManager::playReload() {
	reload.play();
}

// Play reload failed sound function
void SoundManager::playReloadFailed() {
	reloadFailed.play();
}

// Play splat sound function
void SoundManager::playSplat() {
	splat.play();
}

// Play pickup powerup sound function
void SoundManager::playPickup() {
	pickup.play();
}

// Play freeze sound function
void SoundManager::playFreezing() {
	freezing.play();
}

// Play powerup pickup sound function
void SoundManager::playPowerup() {
	powerup.play();
}

// Play gun level up sound function
void SoundManager::playGunLevelUp() {
	gunLevelUp.play();
}

// Play shoot sound function
void SoundManager::playShoot() {
	shoot.play();
}

// Play hit sound function
void SoundManager::playHit() {
	hit.play();
}

// Populate soundtrack function
void SoundManager::populateSoundtrack() {
	soundtrack[0] = soundtrack1;
	soundtrack[1] = soundtrack2;
}

bool SoundManager::isSoundtrackPlaying() {
	for (int i = 0; i < sizeof(soundtrack) / sizeof(Sound); i++)
	{
		if (soundtrack[i].getStatus() == SoundSource::Status::Playing) {
			return true;
		}
	}
	return false;
}

// Play soundtrack function
void SoundManager::playSoundtrack() {
	soundtrack[rand() % sizeof(soundtrack) / sizeof(Sound)].play();
}
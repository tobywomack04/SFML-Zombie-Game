#pragma once
#include <sstream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <iostream>
#include<list>

using namespace sf;

class SoundManager
{
private:
	// Prepare the hit sound
	SoundBuffer hitBuffer;
	Sound hit;

	// Prepare the splat sound
	SoundBuffer splatBuffer;
	sf::Sound splat;

	// Prepare the shoot sound
	SoundBuffer shootBuffer;
	Sound shoot;

	// Prepare the reload sound
	SoundBuffer reloadBuffer;
	Sound reload;

	// Prepare the failed sound
	SoundBuffer reloadFailedBuffer;
	Sound reloadFailed;

	// Prepare the powerup sound
	SoundBuffer powerupBuffer;
	Sound powerup;

	// Prepare the pickup sound
	SoundBuffer pickupBuffer;
	Sound pickup;

	// Prepare the freeze sound
	SoundBuffer freezeBuffer;
	Sound freezing;

	// Prepare the gun level up sound
	SoundBuffer metalBuffer;
	Sound gunLevelUp;

	// List to store soundtrack songs
	Sound soundtrack[2];

	// Prepare the first song of the soundtrack
	SoundBuffer soundtrack1Buffer;
	Sound soundtrack1;

	// Prepare the first song of the soundtrack
	SoundBuffer soundtrack2Buffer;
	Sound soundtrack2;

public:
	// SoundManager constructor
	SoundManager();

	// Play reload sound function
	void playReload();

	// Play reload failed sound function
	void playReloadFailed();

	// Play splat sound function
	void playSplat();

	// Play pickup powerup sound function
	void playPickup();

	// Play freeze sound function
	void playFreezing();

	// Play powerup pickup sound function
	void playPowerup();

	// Play gun level up sound function
	void playGunLevelUp();

	// Play shoot sound function
	void playShoot();

	// Play hit sound function
	void playHit();

	// Populate soundtrack function
	void populateSoundtrack();

	// Checks whether soundtrack is currently playing or not
	bool isSoundtrackPlaying();

	// Play soundtrack function
	void playSoundtrack();
};
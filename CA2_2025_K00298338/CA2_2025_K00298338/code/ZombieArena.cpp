#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ZombieArena.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include "SoundManager.h"
#include <iostream>

using namespace std;
using namespace sf;

int main()
{
	// Here is the instance of TextureHolder
	TextureHolder holder;

	// The game will always be in one of four states
	enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };

	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and create an SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	// Create a an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Creating a light blue transparent overlay when the freeze effect is active
	RectangleShape frozenOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
	frozenOverlay.setFillColor(sf::Color(173, 216, 230, 50));

	// Here are some of our clocks for timing everything
	Clock clock;
	Clock freezeTime;
	Clock dmgNumTimer;
	Clock zombieInvisTimer;

	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in relation to world coordinates
	Vector2f mouseWorldPosition;

	// Where is the mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class
	Player player;

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;

	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	// Prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = NULL;

	// Creating a bullet array of 100 bullets and settings all the gun's default values
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	
	// When was the fire button last pressed?
	Time lastPressed;

	// Hide the mouse pointer and replace it with the crosshair
	window.setMouseCursorVisible(false);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// Create a few pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);
	Pickup freezePickup(3);
	Pickup metalPickup(4);

	// Keeping track of score and high score
	int score = 0;
	int hiScore = 0;

	// Boolean for whether the zombie are currently frozen or not
	bool freeze = false;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(400, 900);
	gameOverText.setString("Press Enter to play");

	// Levelling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(350, 250);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1 - Increased rate of fire" <<
		"\n2 - Increased clip size(next reload)" <<
		"\n3 - Increased max health" <<
		"\n4 - Increased run speed" <<
		"\n5 - More and better health pickups" <<
		"\n6 - More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(125, 980);

	// Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);

	// Load the high score from a text file
	std::ifstream inputFile("gamedata/scores.txt");
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}

	// Hi Score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());

	// Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);
		
	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;

	// What time was the last update
	Time timeSinceLastUpdate;

	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = 1000;

	// Damage number pop-up
	Text damageNum;
	damageNum.setFont(font);
	damageNum.setCharacterSize(35);
	damageNum.setFillColor(Color::Red);
	damageNum.setPosition(100, 100);
	damageNum.setString(to_string(player.getDamage()));
	bool displayDmgNum = false;

	// Dodge variables
	bool isDodging = false;
	bool canDodge = true;
	Clock dodgeClock;
	Clock cooldownClock;
	float dodgeDuration = 0.2f; // 200ms dodge
	float dodgeCooldown = 1.0f; // 1 second cooldown

	// Boolean to switch between pistol and shotgun weapons
	bool shotgunActive = false;

	// Sound object, used to play sounds throughout the game
	SoundManager sound;

	// Populating soundtrack
	sound.populateSoundtrack();

	// The main game loop
	while (window.isOpen())
	{
		/************
		Handle input
		************/

		// Handle events
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				// Pause a game while playing
				if (event.key.code == Keyboard::Return && state == State::PLAYING)
				{
					state = State::PAUSED;
				}

				// Restart while paused
				else if (event.key.code == Keyboard::Return && state == State::PAUSED)
				{
					state = State::PLAYING;
					// Reset the clock so there isn't a frame jump
					clock.restart();
				}

				// Start a new game while in GAME_OVER state
				else if (event.key.code == Keyboard::Return &&
					state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;
					wave = 0;
					score = 0;

					// Prepare the gun and ammo for next game
					currentBullet = 0;
					bulletsSpare = 24;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;

					// Reset the player's stats
					player.resetPlayerStats();
				}

				if (state == State::PLAYING)
				{
					// Reloading
					if (event.key.code == Keyboard::R)
					{
						int reloadBullets = clipSize - bulletsInClip;

						if (bulletsSpare >= clipSize)
						{
							// Plenty of bullets. Reload.
							bulletsSpare -= reloadBullets;
							bulletsInClip += reloadBullets;
							sound.playReload();
						}
						else if (bulletsSpare > 0)
						{
							// Only few bullets left
							bulletsInClip += bulletsSpare;
							bulletsSpare = 0;
							sound.playReload();
						}
						else
						{
							// More here soon?!
							sound.playReloadFailed();
						}
					}
				}
			}
		}// End event polling

		 // Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Handle controls while playing
		if (state == State::PLAYING)
		{
			// If no song from the soundtrack is playing then play a song
			if (!sound.isSoundtrackPlaying()) {
				sound.playSoundtrack();
			}

			// Handle the pressing and releasing of the WASD keys for movement
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			// Fire a bullet with left click
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
				{
					// Gun acts as shotgun
					if (shotgunActive) {
						// Pass the centre of the player and the centre of the crosshair to the shoot function
						bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x + 50, mouseWorldPosition.y + 50);
						currentBullet++;

						if (currentBullet > 99)
						{
							currentBullet = 0;
						}

						bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
						currentBullet++;

						if (currentBullet > 99)
						{
							currentBullet = 0;
						}

						bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x - 50, mouseWorldPosition.y - 50);
						currentBullet++;

						if (currentBullet > 99)
						{
							currentBullet = 0;
						}
					}
					// Gun acts as pistol
					else {
						bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
						currentBullet++;

						if (currentBullet > 99)
						{
							currentBullet = 0;
						}
					}

					lastPressed = gameTimeTotal;
					sound.playShoot();
					bulletsInClip--;
				}
			} // End of soundtrack management, WASD movemnet & firing of bullets

			// Swithing between pistol and shotgun weapons with right click
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) 
			{
				shotgunActive = !shotgunActive; // Toggling between the shotgun being active and inactive
			} // End of switching weapons

			// Upgrading weapon damage using the m key
			if (event.key.code == Keyboard::M)
			{
				// Checks that we have enough metal to upgrade the weapon
				if (player.getMetalCount() > 0)
				{
					player.decreaseMetalCount(); // Decreases the amount of metal the player has
					player.incrementDamage(); // Increments the amount of damage the player does
					damageNum.setString(to_string(player.getDamage())); // Updates floating damage numbers
					sound.playGunLevelUp(); // Plays level up sound
				}
			}

			// Dodging enemies using the space key
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canDodge) {
				isDodging = true;
				canDodge = false;
				dodgeClock.restart();
				cooldownClock.restart();
				player.startDodge();
			}

			// After 200ms stop dodge
			if (isDodging && dodgeClock.getElapsedTime().asSeconds() > dodgeDuration) {
				isDodging = false;
				player.stopDodge();
			}

			// Allows the player to dodge again
			if (!canDodge && cooldownClock.getElapsedTime().asSeconds() > dodgeCooldown) {
				canDodge = true;
			}
		}// End WASD while playing

		// Handle the levelling up state
		if (state == State::LEVELING_UP)
		{
			// Handle the player levelling up
			if (event.key.code == Keyboard::Num1)
			{
				// Increase fire rate
				fireRate++;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num2)
			{
				// Increase clip size
				clipSize += clipSize;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num3)
			{
				// Increase health
				player.upgradeHealth();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num4)
			{
				// Increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num5)
			{
				// Increase number of health pickups
				healthPickup.upgrade();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num6)
			{
				// Increase number of ammo pickups
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				// Increase the wave number
				wave++;

				// Prepare the level
				arena.width = 500 * wave;
				arena.height = 500 * wave;
				arena.left = 0;
				arena.top = 0;

				// Pass the vertex array by reference to the createBackground function
				int tileSize = createBackground(background, arena);

				// Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// Configure the pick-ups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);
				freezePickup.setArena(arena);
				metalPickup.setArena(arena);

				// Create a horde of zombies
				numZombies = 5 * wave;

				// Delete the previously allocated memory (if it exists)
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				// Play the powerup sound
				sound.playPowerup();

				// Reset the clock so there isn't a frame jump
				clock.restart();
			}
		} // End levelling up

		 /****************
		 UPDATE THE FRAME
		 ****************/

		if (state == State::PLAYING)
		{
			// Update the delta time
			Time dt = clock.restart();

			// Update the total game time
			gameTimeTotal += dt;

			// Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			// Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// Make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// Make the view centre around the player				
			mainView.setCenter(player.getCenter());

			// Loop through each Zombie and update them
			for (int i = 0; i < numZombies; i++)
			{
				// Checks if zombies are frozen or not, if not, alive zombies move towards player
				if (!freeze) 
				{
					if (zombies[i].isAlive()) 
					{
						zombies[i].update(dt.asSeconds(), playerPosition);
					}
				}
			}

			// Thaw the zombies after 4 seconds
			if (freeze && freezeTime.getElapsedTime().asSeconds() >= 4.0f) {
				freeze = false;
			}

			// Update any bullets that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			// Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);
			freezePickup.update(dtAsSeconds);
			metalPickup.update(dtAsSeconds);

			// Collision detections
			// Have any zombies been shot?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() && zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							// Stop the bullet
							bullets[i].stop();

							displayDmgNum = true; // Displays damage number on screen
							dmgNumTimer.restart();
							damageNum.setPosition(zombies[j].getXPos() + 5, zombies[j].getYPos() - 10); // Sets damage number position close to the shot zombie

							// Register the hit and see if it was a kill
							if (zombies[j].hit(player.getDamage()))
							{
								// Not just a hit but a kill too
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}

								numZombiesAlive--;

								// When all the zombies are dead (again)
								if (numZombiesAlive == 0) {
									state = State::LEVELING_UP;
								}
							}	
							// Make a splat sound
							sound.playSplat();
						}
					}
				}
			} // End zombie being shot

			// Have any zombies attacked the player
			if (!freeze) // Zombies can't do damage to player while frozen
			{
				for (int i = 0; i < numZombies; i++)
				{
					if (player.getPosition().intersects
					(zombies[i].getPosition()) && zombies[i].isAlive())
					{
						if (player.hit(gameTimeTotal))
						{
							sound.playHit();
						}

						if (player.getHealth() <= 0)
						{
							state = State::GAME_OVER;

							std::ofstream outputFile("gamedata/scores.txt");
							outputFile << hiScore;
							outputFile.close();
						}
					}
				} // End player attacked
			}

			// Hide damage number after 1 second
			if (displayDmgNum && dmgNumTimer.getElapsedTime().asSeconds() >= 1.0f) {
				displayDmgNum = false;
			}

			// Has the player touched health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());

				// Play the pickup sound
				sound.playPickup();
			}

			// Has the player touched ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();

				// Play the reload sound
				sound.playReload();
			}

			// Has the player touched freeze pickup
			if (player.getPosition().intersects(freezePickup.getPosition()) && freezePickup.isSpawned()) {
				freeze = true; // Freezing all enemies in place
				freezeTime.restart(); // Starting the 4 second freeze duration timer

				freezePickup.gotIt();

				// Play the freezing sound
				sound.playFreezing();
			}

			// Has the player touched metal pickup
			if (player.getPosition().intersects(metalPickup.getPosition()) && metalPickup.isSpawned())
			{
				metalPickup.gotIt();
				player.increaseMetalCount();

				// Play the pickup sound
				sound.playPickup();
			}

			// Size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 70));

			// Increment the amount of time since the last HUD update
			timeSinceLastUpdate += dt;

			// Increment the number of frames since the last HUD calculation
			framesSinceLastHUDUpdate++;

			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{
				// Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;
				timeSinceLastUpdate = Time::Zero;
			}// End HUD update
		}// End updating the scene

		 /*
		 **************
		 Draw the scene
		 **************
		 */

		if (state == State::PLAYING)
		{
			window.clear();

			// set the mainView to be displayed in the window and draw everything related to it
			window.setView(mainView);

			// Draw the background
			window.draw(background, &textureBackground);

			// Draw the zombies
			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].getType() != 3 || ((int)zombieInvisTimer.getElapsedTime().asSeconds() % 6 != 0 
					&& (int)zombieInvisTimer.getElapsedTime().asSeconds() % 7 != 0)) 
				{
					window.draw(zombies[i].getSprite());
				}
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			// Draw the player
			window.draw(player.getSprite());

			// Draw the pickups is currently spawned
			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}
			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}
			if (freezePickup.isSpawned())
			{
				window.draw(freezePickup.getSprite());
			}
			if (metalPickup.isSpawned()) 
			{
				window.draw(metalPickup.getSprite());
			}

			if (displayDmgNum) {
				window.draw(damageNum);
			}

			//Draw the crosshair
			window.draw(spriteCrosshair);

			// Switch to the HUD view
			window.setView(hudView);

			// Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (freeze) {
			window.draw(frozenOverlay);
		}

		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}

		window.display();

	}// End game loop

	return 0;
}
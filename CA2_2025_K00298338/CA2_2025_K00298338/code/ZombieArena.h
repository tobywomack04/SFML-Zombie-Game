#pragma once
#include "Zombie.h"

using namespace sf;

// Function to creates the areana
int createBackground(VertexArray& rVA, IntRect arena);

// Function to create a horde of zombies each wave
Zombie* createHorde(int numZombies, IntRect arena);

#pragma once
#include <SDL.H>

typedef struct PlayerPower_S
{
	bool used;
	bool duration;
	bool CDMax;
	bool CDTimer;
	bool uses;
	bool canUse;
	SDL_Scancode key;
}PlayerPower;


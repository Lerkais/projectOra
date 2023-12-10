#pragma once

typedef struct Weapon_S
{
	float damageAdditive;
	float defenseAdditive;
	float speedAdditive;
	float critAdditive;
	unsigned char id;
	//ID denoted by position in array
}Weapon;

void initWeapons(char* filepath);

Weapon getWeaponById(int id);

int getWeaponId(Weapon wep);

char* getWeaponNameById(int id);

void saveWeapons(char* filepath, unsigned char data);

unsigned char readWeapons(char* filepath);
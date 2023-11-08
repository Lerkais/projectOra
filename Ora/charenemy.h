#ifndef _CHARENEMY_H_
#define _CHARENEMY_H_

#include <stdbool.h>

#include "gfc_vector.h"
#include "gf3d_model.h"
#include "weapon.h"

typedef struct Stat_S {
	int maxHealth;
	int attack;
	int speed;
	int defense;
	float critRate;
} Stat;

typedef struct cooldowns_S {
	float basic;
	float heavy;
	float special;
}cooldowns;

typedef struct Charenemy_S {
	//TODO: Vector3D velocity;
	//TODO: Vector3D acceleration;

	bool isDead;
	
	char* name;

	unsigned char uid;

	Stat stats;

	float health;
	cooldowns cooldownTimers;
	cooldowns cooldownMax;

	Weapon weapon;

	void (*takedamage)(struct Charenemy_S* self, float damage); /**<pointer to the damage function*/
	
	//TODO: weapon
}Charenemy;

typedef enum charenemyFlags {
	hidden,
	isDead,
	hasSpecial
};

/// <summary>
/// Main initialization function
/// </summary>
void initCharenemy();

/// <summary>
/// Frees all data from memory for this script
/// </summary>
void freeCharenemy();

/// <summary>
/// Creates a new character at location in memory
/// </summary>
Charenemy* newCharacter();


//
Stat* newStat(); 

unsigned char tickcds(Charenemy* self);

/// <summary>
/// Loads charaenemies into a master charenemy list from given folder containing
/// json folders with the data from each character,last character is a null to indicate end of array
/// </summary>
/// <param name="names"></param>
/// <returns></returns>
Charenemy* populateCharacters(char* filepath);

/// <summary>
/// Returs size of masterlist of charaenemys
/// </summary>
unsigned char getMasterListSize();

/// <summary>
/// Returns pointer to masterlist's first element
/// </summary>
Charenemy** getMasterList();


/// <summary>
/// Changes a character's state to dead directly, this can also happen when their health hits 0
/// </summary>
/// <param name="chara"></param>
void kill(Charenemy* chara);

/// <summary>
/// 
/// </summary>
/// <param name="chara"></param>
void destroy(Charenemy* chara);

void DEBUGprintCharacters();

/// <summary>
/// Read only stats struct with values modified by the weapon struct
/// </summary>
Stat getModifiedStats(Charenemy* self);

void takeDamageCharenemy(Charenemy* self, float amount);


#endif

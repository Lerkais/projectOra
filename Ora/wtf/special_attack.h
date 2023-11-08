#ifndef __SPECIAL_ATTACK_H__
#define __SPECIAL_ATTACK_H__

#include "charenemy.h"

typedef struct SAttack_s {
	char* name;
	Charenemy* originCharacter;
	Charenemy* target;
}SAttack;

/// <summary>
/// Defualt special move
/// </summary>
void defaultExecution();

/// <summary>
/// Executes special move by name
/// </summary>
void executeByName();


#endif 

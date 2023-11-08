#ifndef __SPECIAL_ATTACK_H__
#define __SPECIAL_ATTACK_H__

#include "charenemy.h"
#include "object.h"

Model* basicModel;
Model* heavyModel;

typedef struct SAttack_s {
	Charenemy* originCharacter;
	Charenemy* target;
}SAttack;

Object* basicAttack(Object* creator, Object* target);
Object* heavyAttack(Object* creator, Object* target);
Object* uniAttack(Object* creator, Object* target);

void initAttacks();

void projUpdate(Object* self);

void freeProj(Object* self);

void markDestroyProj(Object* self);



#endif 

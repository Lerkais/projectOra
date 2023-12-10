
#include "charenemy.h"

#include "simple_json.h"
#include "simple_logger.h"
#include "sceneManager.h"
#include "oratime.h"




Charenemy* *CharenemyList;
unsigned char CharenemyListSize;

void takeDamageCharenemy(Charenemy* self, float amount);

Charenemy* populateCharacters(char* filepath)
{
	SJson *json,*keys;

	json = sj_load(filepath);

	keys = sj_object_get_value(json, "characters");
	
	
	
	unsigned char c = sj_array_get_count(keys);

	CharenemyList = malloc(sizeof(Charenemy *) * c);
	if (!CharenemyList) return;
	CharenemyListSize = c;
	
	for(int i = 0; i < c; i++)
	{
		Charenemy* refp = newCharacter();
		if (!refp) return;
		SJson* idp = sj_array_get_nth(keys, i);
		strcpy(refp->name, sj_object_get_value_as_string(idp, "name"));
		sj_object_get_value_as_int(idp, "attack", &(refp->stats.attack));
		sj_object_get_value_as_int(idp, "maxHealth", &(refp->stats.maxHealth));
		sj_object_get_value_as_int(idp, "speed", &(refp->stats.speed));
		sj_object_get_value_as_int(idp, "defense", &(refp->stats.defense));
		sj_object_get_value_as_float(idp, "critRate", &(refp->stats.critRate));
		sj_object_get_value_as_int(idp, "UID",&(refp->uid));
		//char* name = sj_object_get_value_as_string(idp, "name");

		refp->weapon.critAdditive = 1;
		refp->weapon.damageAdditive = 1;
		refp->weapon.defenseAdditive = 1;
		refp->weapon.speedAdditive = 1;

		refp->cooldownMax.basic = 1;
		refp->cooldownMax.heavy = 5;
		refp->cooldownMax.special = 20;

		refp->takedamage = takeDamageCharenemy;

		refp->health = refp->stats.maxHealth;

		refp->cooldownTimers = refp->cooldownMax;

		refp->isDead = false;



		if(CharenemyList != NULL)
			CharenemyList[i] = refp;
		//todo figure out how to make stats scalable
	}

	if(json)
	sj_free(json);
	return CharenemyList;
}



Charenemy* newCharacter() 
{
	Charenemy* ret = malloc(sizeof(Charenemy));
	
	
	if (ret == NULL) {
		slog("Malloc for new character failed");
		exit(29);
	}
	ret->name = calloc(64, sizeof(char));
	if (ret->name == NULL) {
		slog("Malloc for new character failed - name");
		exit(30);
	}


	return ret;
}


Charenemy** getMasterList() 
{
	return CharenemyList;
}

unsigned char getMasterListSize() 
{
	return CharenemyListSize;
}

void DEBUGprintCharacters() 
{
	for (int i = 0; i < CharenemyListSize; i++) {
		printf("%s\n-------------------------------\n", CharenemyList[i]->name);
		printf("attack: %i\n",CharenemyList[i]->stats.attack);
		printf("crit: %f\n", CharenemyList[i]->stats.critRate);
		printf("defense: %i\n", CharenemyList[i]->stats.defense);
		printf("maxHealth: %i\n", CharenemyList[i]->stats.maxHealth);
		printf("speed: %i\n\n", CharenemyList[i]->stats.speed);
	}
	printf("done");
}

Stat getModifiedStats(Charenemy* self)
{
	Stat ret = self->stats;
	Weapon src = self->weapon;
	ret.attack += src.damageAdditive;
	ret.critRate += src.critAdditive;
	ret.defense += src.defenseAdditive;
	ret.speed += src.speedAdditive;
	return ret;
}

unsigned char tickcds(Charenemy* self)
{
	float *b, *h, *s; //basic heavy, special
	float bm, hm, sm;
	unsigned char ret = 0b000;

	b = &self->cooldownTimers.basic;
	h = &self->cooldownTimers.heavy;
	s = &self->cooldownTimers.special;

	bm = self->cooldownMax.basic;
	hm = self->cooldownMax.heavy;
	sm = self->cooldownMax.special;

	if (*b < 0)
	{
		*b = bm;
		ret = ret | 0b100;
	}
	else
		*b -= deltaTime;

	if (*h < 0)
	{
		*h = hm;
		ret = ret | 0b010;
	}
	else
		*h -= deltaTime;

	if (*s < 0)
	{
		ret = ret | 0b001;
	}
	else
		*s -= deltaTime;

	return ret;
}

void takeDamageCharenemy(Charenemy* self, float amount)
{
	self->health -= amount;
	if (self->health <= 0)
	{
		kill(self);
	}
}

void kill(Charenemy* chara)
{
	chara->isDead = true;
}

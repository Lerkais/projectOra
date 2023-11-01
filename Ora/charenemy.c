
#include "charenemy.h"

#include "simple_json.h"
#include "simple_logger.h"
#include "sceneManager.h"




Charenemy* *CharenemyList;
unsigned char CharenemyListSize;

void initCharenemy() 
{
}

Charenemy* populateCharacters(char* filepath)
{
	SJson* json;

	json = sj_load(filepath);


	
	SJList* keys = sj_list_new();

	if (!keys)
		keys = (SJList*)sj_object_get_value(json, "characters");
	
	
	
	unsigned char c = sj_list_get_count(keys);

	CharenemyList = malloc(sizeof(Charenemy *) * c);
	CharenemyListSize = c;
	
	for(int i = 0; i < c; i++)
	{
		Charenemy* refp = newCharacter();
		if (!refp) return;
		SJson* idp = sj_list_get_nth(keys, i);
		refp->name = sj_object_get_value_as_string(idp, "name");
		sj_object_get_value_as_int(idp, "attack", &(refp->stats.attack));
		sj_object_get_value_as_int(idp, "maxHealth", &(refp->stats.maxHealth));
		sj_object_get_value_as_int(idp, "speed", &(refp->stats.speed));
		sj_object_get_value_as_int(idp, "defense", &(refp->stats.defense));
		sj_object_get_value_as_float(idp, "critRate", &(refp->stats.critRate));
		//char* name = sj_object_get_value_as_string(idp, "name");

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
	Scene* ret1 = malloc(sizeof(Scene));
	
	if (ret == NULL) {
		slog("Malloc for new character failed");
		return NULL;
	}
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
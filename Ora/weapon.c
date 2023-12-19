#include "weapon.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "sceneManager.h"

Weapon* weapons;
unsigned char c;
char** names;


void initWeapons(char* filepath)
{
	SJson* json, * keys;

	json = sj_load(filepath);

	keys = sj_object_get_value(json, "weapons");
	c = sj_array_get_count(keys);

	names = calloc(c, sizeof(char*));
	weapons = calloc(c, sizeof(Weapon));
	if (!weapons || !names)
	{
		sj_free(json);
		return;
	}

	for (int i = 0; i < c; i++)
	{
		Weapon ref;
		SJson* idp = sj_array_get_nth(keys, i);
		names[i] = calloc(64,sizeof(char)); //max 64 character name 
		if (!names[i]) { slog("malloc failure"); return NULL; }
		strcpy(names[i], sj_object_get_value_as_string(idp, "name"));
		sj_object_get_value_as_float(idp, "damageAdditive", &(ref.damageAdditive));
		sj_object_get_value_as_float(idp, "defenseAdditive", &(ref.defenseAdditive));
		sj_object_get_value_as_float(idp, "speedAdditive", &(ref.speedAdditive));
		sj_object_get_value_as_float(idp, "critAdditive", &(ref.critAdditive));

		ref.id = i;

		printf("%s,%f,%f,%f,%f\n",names[i], ref.damageAdditive, ref.defenseAdditive, ref.speedAdditive, ref.critAdditive);

		weapons[i] = ref;
	}


	if (json)
		sj_free(json);

}

Weapon getWeaponById(int id)
{

	return weapons[id];
}

char* getWeaponNameById(int id)
{
	return names[id];
}

bool weapcomp(Weapon a, Weapon b)
{
	return (
		a.critAdditive == b.critAdditive
		&& a.damageAdditive == b.damageAdditive
		&& a.defenseAdditive == b.defenseAdditive
		&& a.speedAdditive == b.speedAdditive
		);
}

int getWeaponId(Weapon wep)
{
	for (int i = 0; i < c; i++) { if (weapcomp(wep, weapons[i])) return i; }
}


unsigned char readWeapons(char* filepath)
{
	SJson * json = sj_load(filepath);
	int val = 0;

	sj_object_get_value_as_int(json,"weapons",&val);

	sj_free(json);

	return val;

}

void saveWeapons(char* filepath,unsigned char data)
{
	SJson * json = sj_object_new();

	char* key = calloc(12,sizeof(char));
	if (!key)return;
	strcpy(key, "weapons");

	SJson* sjdata = sj_new_int(data);

	sj_object_insert(json, key, sjdata);

	sj_save(json, filepath);

	if(json)
	sj_free(json);

}

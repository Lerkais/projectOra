#ifndef __DATA_LOADER_H__
#define __DATA_LOADER_H__

#define CHARENEMY_FILEPATH "\\Ora\\charenemyData\\charenemyData\.json"

typedef enum DataType_E {
	charenemy,
	sattack,
	scene3D,
	scene2D
}DataType;

void loadData();

void loadCharenemyData();

void loadSAttackData();

void loadScene2DData();



#endif // !__DATA_LOADER_H__

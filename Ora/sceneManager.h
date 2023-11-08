#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "charenemy.h"
#include "object.h"


typedef enum SceneType_E {
	menuScene,
	explorationScene,
	battleScene,
	otherscene
}SceneType;

typedef struct BattleSceneData_S {
	Object* playerChar;
	Object* enemyChar;
}BattleSceneData;

typedef union SceneData_U {
	BattleSceneData battleData;
}SceneData;

typedef struct Scene_S{
	unsigned int Id;
	char* name;
	SceneType type;
	unsigned int objectCount;
	unsigned int objectSize;
	Object** objects;
	SceneData data;
}Scene;

typedef struct SceneManager_S {
	Scene** scenes; //Max 10 scenes allowed for now
	unsigned int count;
	unsigned int size;
	Scene* activeScene;

}SceneManager;

/// <summary>
/// Initialises SceneManager
/// </summary>
/// <returns></returns>
void InitSceneManager();

SceneManager* GetSceneManager();

/// <summary>
/// Creates a new scene
/// </summary>
/// <param name="sceneManager"></param>
/// <param name="type"></param>
/// <returns></returns>
Scene* NewScene(SceneType type);

/// <summary>
/// Add a scene to scenes array
/// </summary>
/// <param name="sceneManager"></param>
/// <param name="scene"></param>
/// <returns></returns>
bool AddSceneToManager(Scene* scene);

void RemoveSceneObject(Scene* scene, Object* object);



/// <summary>
/// creates a new unique id for the given scene within the manager
/// </summary>
/// <param name="sceneManager"></param>
/// <returns></returns>
int NewID(SceneManager* sceneManager);

/// <summary>
/// Adds an object to a scene's object list
/// </summary>
/// <param name="scene"></param>
/// <param name="object"></param>
void AddSceneObject(Scene* scene,Object* object);



/// <summary>
/// Frees scene and all scene data and removes refrences to scene
/// </summary>
/// <param name="scene"></param>
/// <returns></returns>
bool FreeScene(Scene* scene);

/// <summary>
/// Frees scene based on scene ID
/// </summary>
/// <param name="sceneManager"></param>
/// <param name="sceneID"></param>
/// <returns></returns>
bool FreeScene(int sceneID);

/// <summary>
/// Loads a scene into game
/// </summary>
/// <param name="scene"></param>
void LoadScene(Scene* scene);

/// <summary>
/// Updates all objects in a scene
/// </summary>
/// <param name="scene"></param>
void UpdateObjects(Scene* scene);

/// <summary>
/// Adds an oject to the active scene if there is one
/// </summary>
/// <param name="object"></param>
void AddObjectToActiveScene(Object* object);

/// <summary>
/// Removes an object from the given scene by given pointer
/// </summary>
/// <param name="scene"></param>
/// <param name="object"></param>
void RemoveAndDestroySceneObject(Scene* scene, Object* object);

void RemoveSceneObject(Scene* scene, Object* object);




#endif
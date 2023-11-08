#include <stdlib.h>

#include "sceneManager.h"
#include "oratime.h"

SceneManager* sceneManager = NULL;

void InitSceneManager() 
{
	sceneManager = malloc(sizeof(SceneManager));
	Scene* scene = NewScene(otherscene);
	if (!sceneManager || !scene) {

		printf("FATAL ERROR: Scene Manager Failed to initialize");
		exit(01134);
	}

	sceneManager->activeScene = scene;
	sceneManager->count = 0;
	sceneManager->size = 2;
	sceneManager->scenes = calloc(sceneManager->size, sizeof(Scene*));
}

SceneManager* GetSceneManager() {
	return sceneManager;
}


Scene* NewScene(SceneType type)
{
	Scene* ret = malloc(sizeof(Scene));
	if (!ret)return;
	switch (type) {
	case otherscene:
		ret->Id = NewID(sceneManager);
		char* buff = calloc(50, sizeof(char));
		sprintf(buff, "Scene%i", ret->Id);
		ret->name = buff;
		ret->objectCount = 0;
		ret->objectSize = 4;
		ret->objects = malloc(sizeof(Object*)*4);
		break;
	default:
		return NULL;
	}
	return ret;
}

bool AddSceneToManager(Scene* scene) {
	if (sceneManager->size <= sceneManager->count) 
	{
		sceneManager->size *= 2;
		Scene** newl = calloc(sceneManager->size, sizeof(Scene*));
		free(sceneManager->scenes);
		sceneManager->scenes = newl;
		for (Scene* iter = sceneManager->scenes[0]; iter != NULL; iter += sizeof(Scene*),newl+=sizeof(Scene*)) 
		{
			*newl = iter;
		}
	}
	sceneManager->scenes[sceneManager->count] = scene;
	sceneManager->count++;
}

int NewID(SceneManager* sceneManager)
{
	return sceneManager->count;
}

void AddSceneObject(Scene* scene, Object* object)
{
	if (scene->objectSize < scene->objectCount + 1) 
	{
		scene->objectSize *= 2;
		Object** temp = malloc(sizeof(Object*) * scene->objectSize);
		for (int i = 0; i < scene->objectCount; i++) 
		{
			temp[i] = scene->objects[i];
		}
		free(scene->objects);
		scene->objects = temp;
		
	}
	scene->objects[scene->objectCount] = object;
	scene->objectCount++;
	if (scene = sceneManager->activeScene) object->isActive = true;
}

void DrawSceneObjects(Scene* scene)
{
	for (int i = 0; i < scene->objectCount; i++) 
	{
		DrawObject(scene->objects[i]);
	}
}

bool FreeScene(int sceneID)
{
	for (int i = 0; i < sceneManager->count; i++)
	{
		if (sceneManager->scenes[i]->Id = sceneID)
		{
			//free(sceneManager->scenes[i].)
			//sceneManger->scenes[i] =
		}
	}
	return false;
}

void UpdateObjects(Scene* scene)
{
	if (!scene)return;

	int count = scene->objectCount;

	for (int i = 0; i < scene->objectCount; i++)
	{
		scene->objects[i]->update(scene->objects[i]);
	}
}

void AddObjectToActiveScene(Object* object)
{
	if(sceneManager->activeScene)
	AddSceneObject(sceneManager->activeScene, object);
}

void LoadScene(Scene* scene)
{
	if (!scene||!sceneManager)return;

	sceneManager->activeScene = scene;

	for (int i = 0; sceneManager->count; i++)
	{
		scene->objects[i]->isActive = true;
	}

}
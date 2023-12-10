#include <stdlib.h>

#include "sceneManager.h"
#include "oratime.h"
#include "object.h"

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
	default:
		ret->Id = NewID(sceneManager);
		char* buff = calloc(50, sizeof(char));
		if (buff == NULL) return;
		sprintf(buff, "Scene%i", ret->Id);
		ret->name = buff;
		ret->objectCount = 0;
		ret->objectSize = 4;
		ret->objects = malloc(sizeof(Object*)*4);
		ret->type = type;
		break;
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
		if (temp == NULL)return;
		for (int i = 0; i < scene->objectCount; i++) 
		{
			temp[i] = scene->objects[i];
		}
		free(scene->objects);
		scene->objects = temp;
		
	}
	scene->objects[scene->objectCount] = object;
	scene->objectCount++;
	if (scene == sceneManager->activeScene) object->isActive = true;
}

void RemoveSceneObject(Scene* scene, Object* object)
{
		if (!scene || !object || scene->objectCount == 0)return;
		for (unsigned int i = 0; i < scene->objectCount; i++)
		{
			if (scene->objects[i] == object)
			{
				if (i != scene->objectCount-1)
				{
					FreeObject(scene->objects[i]);
					scene->objects[i] = scene->objects[scene->objectCount-1];
				}					
				scene->objectCount--;
				scene->objects[scene->objectCount] = NULL;
				break;
			}
		}
}

void RemoveAndDestroySceneObject(Scene* scene, Object* object)
{
	RemoveSceneObject(scene, object);
}

void DrawSceneObjects(Scene* scene)
{
	for (int i = 0; i < scene->objectCount; i++) 
	{
		if(scene->objects[i]->isActive && !scene->objects[i]->toBeDestroyed)
			DrawObject(scene->objects[i]);
	}
}

bool FreeScene(Scene* scene)
{
	if (!scene)return false;
	for (int i = 0; i < sceneManager->count; i++)
	{
		if (sceneManager->scenes[i] == scene)
		{
			//remove scene from manager
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
		if (vector3d_magnitude(scene->objects[i]->position) > 10000) //World radial contraint
			scene->objects[i]->toBeDestroyed = true;
		if (scene->objects[i]->toBeDestroyed) {
			RemoveAndDestroySceneObject(scene,scene->objects[i]);
			i--;
		}
		else
			scene->objects[i]->update(scene->objects[i]);
	}
}

void AddObjectToActiveScene(Object* object)
{
	if (object == NULL)return;
	if(sceneManager->activeScene)
	AddSceneObject(sceneManager->activeScene, object);
}

void LoadScene(Scene* scene)
{
	if (!scene||!sceneManager)return;

	sceneManager->activeScene = scene;

	for (int i = 0; i < sceneManager->activeScene->objectCount; i++)
	{
		scene->objects[i]->isActive = true;
	}

}

Scene* UnloadActiveScene()
{
	Scene* actS = sceneManager->activeScene;
	for (int i = 0; i < actS->objectCount; i++)
	{
		actS->objects[i]->isActive = false;
	}

	sceneManager->activeScene = NewScene(otherscene);
	sceneManager->activeScene->name = "BLANK";

	return actS;

}




//todo void UnloadScene();
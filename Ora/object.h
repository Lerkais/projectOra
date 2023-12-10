#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "gfc_vector.h"
#include <stdbool.h>
#include "charenemy.h"
#include "simple_logger.h"

struct Object_S;

typedef enum ObjectType_E {
	charenemy,
	enviorment,
	projectile,
	particle,
	specialAttack,
	camera,
	other
}ObjectType;

typedef struct CharenemyObject_S {
	Charenemy* charenemy;
}CharenemyObject;


typedef struct ProjectileObject_S {
	//Projectile 
	struct Object_S* creator;
	struct Object_S* target;
	float damage;
	float speed;
}ProjectileObject;

typedef struct SpecialObject_S {
	Charenemy charenemy;
}SpecialObject;

typedef struct ParticleObject_S {
	Charenemy charenemy;
}ParticleObject;

typedef union ObjectData_S {
	CharenemyObject charenemyObj;
	ProjectileObject projectileObj;
	SpecialObject specialObj;
	ParticleObject particleObj;
	void* Other;
}ObjectData;

typedef struct Object_S {
	unsigned char ID;
	bool isActive;
	bool isStatic;
	bool toBeDestroyed;
	Vector3D position;
	Vector3D velocity;
	Vector3D acceleration;
	Vector3D scale;
	Vector3D rotation;

	Model* model;
	Matrix4 modelMat;
	Color color;
	Vector4D ambiant;
	ObjectType Type;
	ObjectData Data;

	void (*think)(struct Object_S* self); /**<pointer to the think function*/
	void (*update)(struct Object_S* self); /**<pointer to the update function*/
	void (*draw)(struct Object_S* self); /**<pointer to an optional extra draw funciton*/
	void (*damage)(struct Object_S* self, float damage); /**<pointer to the think function*/
	void (*destroy)(struct Object_S* self);
	void (*onDeath)(struct Object_S* self); /**<pointer to an funciton to call when the entity dies*/
}Object;

Object* NewObject();

void FreeObject(Object* object);

void DrawObject(Object* self);

void UpdateMe(Object* self);








#endif // !__OBJECT_H__

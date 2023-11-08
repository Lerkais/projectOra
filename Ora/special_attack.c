#include "special_attack.h"

#include "gf3d_model.h"
#include "oratime.h"
#include "charenemy.h"

#include "gfc_vector.h"

#include "object.h"




void projUpdate(Object* self);
void freeProj(Object* self);
void markDestroyProj(Object* self);

void uCheckXY(Object* proj);
void u1(Object* proj);
void u2(Object* proj);
void u3(Object* proj);
void u4(Object* proj);
void u5(Object* proj);
void u6(Object* proj);
void u7(Object* proj);
void u8(Object* proj);
void u9(Object* proj);

#define SPEED 20

void initAttacks()
{
	basicModel = gf3d_model_load("models/dino.model");
	heavyModel = gf3d_model_load("models/c3.model");
}

Object* basicAttack(Object* creator,Object* target)
{
	Object* proj = NewObject();
	void* bleh = creator->Data.charenemyObj.charenemy;
	Stat stats = getModifiedStats(creator->Data.charenemyObj.charenemy);
	proj->model = basicModel;
	proj->scale = vector3d(.3, .3, .3);
	proj->color = GFC_COLOR_BLUE;
	proj->update = projUpdate;
	proj->position = creator->position;
	proj->Type = projectile;
	proj->destroy = freeProj;

	proj->rotation = vector3d(.5, 0, 0);
	

	proj->Data.projectileObj.creator = creator;
	proj->Data.projectileObj.target = target;
	proj->Data.projectileObj.damage = 30;
	//if (rand() % 100 < stats.critRate * 100) proj->Data.projectileObj.damage *= 2;
	//proj->Data.projectileObj.damage -= stats.defense;
	proj->Data.projectileObj.speed = 40 + stats.speed;

	projUpdate(proj);

	return proj;
}

Object* heavyAttack(Object* creator, Object* target)
{
	Object* proj = NewObject();
	proj->model = heavyModel;
	proj->scale = vector3d(.04, .04, .04);
	proj->color = GFC_COLOR_RED;
	proj->update = projUpdate;
	proj->position = creator->position;
	proj->Type = projectile;
	proj->destroy = freeProj;


	proj->Data.projectileObj.creator = creator;
	proj->Data.projectileObj.target = target;
	proj->Data.projectileObj.damage = 60;
	proj->Data.projectileObj.speed = 20;

	projUpdate(proj);

	return proj;
}

Object* uniAttack(Object* creator, Object* target)
{
	Object* proj = NULL;

	switch (creator->Data.charenemyObj.charenemy->uid)
	{
	case 1:
		proj = basicAttack(creator,target);
		proj->update = u1;
		
		break;
	case 2:
		proj = basicAttack(creator, target);
		proj->update = u2;
		break;
	case 3:
		proj = basicAttack(creator, target);
		proj->update = u3;
		break;
	case 4:
		proj = basicAttack(creator, target);
		proj->update = u4;
		break;
	case 5:
		proj = basicAttack(creator, target);
		proj->update = u5;
		break;
		break;
	case 6:
		proj = basicAttack(creator, target);
		proj->update = u6;
		break;
		break;
	case 7:
		proj = basicAttack(creator, target);
		proj->update = u7;
		break;
		break;
	case 8:
		proj = basicAttack(creator, target);
		proj->update = u8;
		break;
	case 9:
		proj = basicAttack(creator, target);
		proj->update = u9;
		break;
	default:
		return;
	}
	return proj;
}

void projUpdate(Object* self) //move straight towards target
{
	ProjectileObject pointers = self->Data.projectileObj;

	
	Vector3D movePoint;
	Vector3D curPoint = self->position;
	Vector3D dif;
	vector3d_sub(dif,pointers.target->position, curPoint);

	if (vector3d_magnitude(dif) < .5)
	{
		markDestroyProj(self);
		pointers.target->Data.charenemyObj.charenemy->takedamage(pointers.target->Data.charenemyObj.charenemy,pointers.damage);
		return;
	}

	vector3d_normalize(&dif);
	vector3d_scale(dif, dif, (pointers.speed * deltaTime));
	vector3d_add(movePoint,curPoint,dif);

	self->position = movePoint;
	self->rotation.z += .3;


	UpdateMe(self);
}

void markDestroyProj(Object* self)
{
	self->toBeDestroyed = true;
}

void freeProj(Object* self)
{
	free(self);
}

//Special update functions

void uCheckXY(Object* proj)
{
	Vector2D a = vector2d(proj->position.x, proj->position.y);
	Vector2D b = vector2d(proj->Data.projectileObj.target->position.x, proj->Data.projectileObj.target->position.y);
	Vector2D dif;
	vector2d_sub(dif,b, a);
	float distance = vector2d_magnitude(dif);
	if (distance < 1)
	{
		ProjectileObject pointers = proj->Data.projectileObj;
		markDestroyProj(proj);
		pointers.target->Data.charenemyObj.charenemy->takedamage(pointers.target->Data.charenemyObj.charenemy, pointers.damage);
		return;
	}

}

void u1(Object* proj) //sin ?
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = sin(proj->position.x);
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u2(Object* proj)
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = cos(proj->position.x);
	proj->Data.projectileObj.damage = 110;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u3(Object* proj)//fast sin
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = sin(proj->position.x/2);
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u4(Object* proj)
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = cos(proj->position.x/2);
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u5(Object* proj)//sawtooth
{
	projUpdate(proj);
	uCheckXY(proj);
	float x = proj->position.x;
	float d = 10;
	proj->position.z = d*(x/d-floor(x/d));
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u6(Object* proj)//3d spiral
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = sin(proj->position.x / 2);
	proj->position.y = sin(proj->position.x / 2);
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u7(Object* proj)//Idk something
{
	projUpdate(proj);
	uCheckXY(proj);
	float x = proj->position.x;
	float d = 10;
	proj->position.z = sin(proj->position.x / 2);
	proj->position.z += d*(x/d - floor(x/d));
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u8(Object* proj)
{
	projUpdate(proj);
	uCheckXY(proj);
	proj->position.z = 10* cos(proj->position.x);
	proj->Data.projectileObj.damage = 110;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

void u9(Object* proj)
{
	projUpdate(proj);
	uCheckXY(proj);
	float x = proj->position.x;
	float d = 10;
	proj->position.z = sin(proj->position.x / 2);
	proj->position.z += d * (x / d - ceil(x / d));
	proj->Data.projectileObj.damage = 100;
	proj->Data.projectileObj.speed = 10;
	proj->color = GFC_COLOR_CYAN;
}

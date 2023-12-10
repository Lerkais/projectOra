#pragma once

#include "gfc_vector.h"
#include "gf3d_particle.h"

#include "ray.h"
#include "object.h"
#include "oratime.h"

#include "simple_logger.h"



//2d for now


typedef struct OraParticle_S
{
	Particle part;
	float radius;
	Vector2D pos;
	Vector2D npos;
	Vector2D vel;
	float maxVel;
	Vector2D accel;
}OraParticle;

typedef struct ParticleSpawner_S
{
	OraParticle* ownedParticles;
	//OraParticle partToSpawn;
	int ownedCount;
	int ownedSize;
	int spawnRate;
	float spawnTimer;
	Vector3D position;

}ParticleSpawner;

ParticleSpawner* newpSpawner(int spawnRate,Vector3D position);

void particleSpawnerStep(ParticleSpawner* p);

void drawOraParticles(ParticleSpawner* s);

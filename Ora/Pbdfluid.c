#include "Pbdfluid.h"


void resolveCollisions(ParticleSpawner* s);
void ppcol(OraParticle* a, OraParticle* b); //hide
void prectcol(OraParticle p, Vector4D rect); //hide

#define GRAVITY -5

void resolveCollisions(ParticleSpawner* s)
{
	OraParticle* a,*b;



	for (int i = 0; i < s->ownedCount; i++)
	{
		a = &(s->ownedParticles[i]);
		for (int j = 0; j < s->ownedCount; j++)
		{
			b = &(s->ownedParticles[j]);
			ppcol(a, b);
		}
	}
}

void ppcol(OraParticle* a, OraParticle* b)
{
	if (vector2d_distance_between_less_than(a->pos, b->pos, a->radius + b->radius))
	{
		Vector2D diff;
		vector2d_sub(diff, a->pos, b->pos);
		
		vector2d_normalize(&diff);


		int nx = a->radius * diff.x;
		int ny = a->radius * diff.y;

		a->pos.x += nx;
		a->pos.y += ny;
		b->pos.x -= nx;
		b->pos.y -= ny;
	}
}

void prectcol(OraParticle p, Vector4D rect)
{

}

void initspawner(ParticleSpawner* s)
{
	s->ownedParticles = calloc(100, sizeof(OraParticle));
	s->ownedSize = 100;
	s->ownedCount = 0;
	s->spawnTimer = 1 / s->spawnRate;



}


void spawn(ParticleSpawner* s)
{
	if (s->ownedCount+1 > s->ownedSize)
	{
		void* temp = realloc(s->ownedParticles, sizeof(OraParticle) * s->ownedCount * 2);
		if (!temp)
			return;
		else
		{
			s->ownedSize *= 2;
		}
		s->ownedParticles = temp;
	}

	OraParticle* p = &(s->ownedParticles[s->ownedCount]);
	s->ownedCount += 1;

	p->pos = vector2d(s->position.x,s->position.z);
	p->radius = 1;
	p->vel = vector2d(rand() % 10 -5, rand() % 10 - 5);
	p->part.color = GFC_COLOR_BLUE;
	p->part.size = 5;

}

void step(ParticleSpawner* s)
{
	resolveCollisions(s);

	for (int i = 0; i < s->ownedCount; i++)
	{
		OraParticle* p = &(s->ownedParticles[i]);

		float x= p->npos.x, y = p->npos.y;


		//upate pos
		x = p->pos.x + (p->vel.x + (rand() % 100 - 50) /100.0f) * deltaTime;
		y = p->pos.y + p->vel.y * deltaTime;

		if (y >= -5)
			p->pos.y = y;


		p->part.position = vector3d(p->pos.x, s->position.y, p->pos.y);


		p->vel.x = p->vel.x * .95;
		p->vel.y = p->vel.y * .95 + GRAVITY * .5;

	}

	if (s->spawnTimer <= 0)
	{
		spawn(s);
		s->spawnTimer = 1 / s->spawnRate;
	}
	else
		s->spawnTimer -= deltaTime;

	

}

void drawOraParticles(ParticleSpawner* s)
{
	for (int i = 0; i < s->ownedCount; i++)
	{
		gf3d_particle_draw(&(s->ownedParticles[i].part));
	}
}

ParticleSpawner* newpSpawner(int spawnRate, Vector3D position)
{
	ParticleSpawner* s = calloc(1, sizeof(ParticleSpawner));
	if (!s) return;

	s->spawnRate = spawnRate;
	s->position = position;
	initspawner(s);

	spawn(s);
	spawn(s);

	return s;
}

void particleSpawnerStep(ParticleSpawner* s)
{
	step(s);
	
}
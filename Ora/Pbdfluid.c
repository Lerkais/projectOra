#include "Pbdfluid.h"

#define GRAVITY -75
#define MAX_SPEED_LIMIT 200.0
#define DAMPING_FACTOR 0.999
#define RESTITUTION 1.0

void maxVelocity(Vector2D* vel) {
    float speed = sqrt(vel->x * vel->x + vel->y * vel->y);
    if (speed > MAX_SPEED_LIMIT) {
        vel->x = (vel->x / speed) * MAX_SPEED_LIMIT;
        vel->y = (vel->y / speed) * MAX_SPEED_LIMIT;
    }
}

void constrainParticleHeight(OraParticle* p) {
    if (p->pos.y <= 0.0f) {
        p->pos.y = 0.0f; // Set the height to 0

        p->vel.y = 0.0f;
    }
}

void applyGravity(OraParticle* p) {
    p->vel.y += GRAVITY * deltaTime;
}

void dampVelocity(OraParticle* p) {
    p->vel.x *= DAMPING_FACTOR;
    p->vel.y *= DAMPING_FACTOR;
}

void ppcol(OraParticle* a, OraParticle* b);

void resolveCollisions(ParticleSpawner* s) {
    for (int i = 0; i < s->ownedCount; i++) {
        for (int j = i + 1; j < s->ownedCount; j++) {
            ppcol(&s->ownedParticles[i], &s->ownedParticles[j]);
        }
    }
}

void ppcol(OraParticle* a, OraParticle* b) {
    Vector2D diff;
    vector2d_sub(diff, b->pos, a->pos);
    float distance = vector2d_magnitude(diff);
    float overlap = (a->radius + b->radius) - distance;

    // Only apply repulsion if particles are overlapping
    if (overlap > 0) {
        // Normalize the difference vector
        vector2d_normalize(&diff);

        // Repulsion force (could be adjusted for different fluid behaviors)
        float repulsionStrength = overlap * 0.5;

        // Apply repulsion to separate the particles
        vector2d_add(a->pos, a->pos, vector2d(-diff.x * repulsionStrength, -diff.y * repulsionStrength));
        vector2d_add(b->pos, b->pos, vector2d(diff.x * repulsionStrength, diff.y * repulsionStrength));

        // Adjusting velocities for a more fluid-like interaction
        Vector2D averageVelocity;
        Vector2D temp; 
        vector2d_add(temp, a->vel, b->vel);

        vector2d_scale(averageVelocity,temp , 0.5);
        a->vel = averageVelocity;
        b->vel = averageVelocity;
    }
}



void initspawner(ParticleSpawner* s) {
    s->ownedParticles = calloc(100, sizeof(OraParticle));
    s->ownedSize = 100;
    s->ownedCount = 0;
    s->spawnTimer = 1.0f / s->spawnRate;
    s->color = GFC_COLOR_WHITE;
}

void spawn(ParticleSpawner* s) {
    if (s->ownedCount >= s->ownedSize) {
        void* temp = realloc(s->ownedParticles, sizeof(OraParticle) * (s->ownedCount * 2));
        if (!temp) return;
        s->ownedParticles = temp;
        s->ownedSize *= 2;
    }

    OraParticle* p = &s->ownedParticles[s->ownedCount++];
    p->pos = vector2d(s->position.x + (rand() % 10 - 5)/10, s->position.z + (rand() % 10 - 5) / 10);
    p->radius = .1;
    p->vel = vector2d(rand() % 10 - 5, rand() % 10 - 5);
    p->part.color = s->color;
    p->part.size = 2;
    p->mass = 1;
}

void step(ParticleSpawner* s) {
    resolveCollisions(s);

    for (int i = 0; i < s->ownedCount; i++) {
        OraParticle* p = &s->ownedParticles[i];

        applyGravity(p);
        vector2d_add(p->pos, p->pos, vector2d(p->vel.x * deltaTime, p->vel.y * deltaTime));
        dampVelocity(p);
        maxVelocity(&p->vel);

        constrainParticleHeight(p);

        p->part.position = vector3d(p->pos.x, s->position.y, p->pos.y);

        

    }

    if (s->spawnTimer <= 0) {
        spawn(s);
        s->spawnTimer = 1.0f / s->spawnRate;
    }
    else {
        s->spawnTimer -= deltaTime;
    }
}

void drawOraParticles(ParticleSpawner* s) {
    for (int i = 0; i < s->ownedCount; i++) {
        gf3d_particle_draw(&s->ownedParticles[i].part);
    }
}

ParticleSpawner* newpSpawner(int spawnRate, Vector3D position) {
    ParticleSpawner* s = calloc(1, sizeof(ParticleSpawner));
    if (!s) return NULL;

    s->spawnRate = spawnRate;
    s->position = position;
    initspawner(s);

    spawn(s);
    spawn(s);

    return s;
}

void particleSpawnerStep(ParticleSpawner* s) {
    step(s);
}

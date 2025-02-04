#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "oratime.h"

static int thirdPersonMode = 0;
void player_think(Entity *self);
void player_update(Entity *self);

ObjectLinked* playerChildren; //linked list
float speed;

Vector3D deltaPos;

void moveChildren();

ObjectLinked* getChildLink()
{
    return playerChildren;
}

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;

    float speed = 30;

    deltaPos = vector3d(0, 0, 0);
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 1;

    ent->canMove = true;

    playerChildren = malloc(sizeof(ObjectLinked));
    if (!playerChildren)return;

    playerChildren->next = NULL;
    playerChildren->self = NULL;


    return ent;
}


void player_think(Entity *self)
{
    Vector3D forward = {0};
    Vector3D right = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    Vector3D posPrime = self->position;

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = 0;
    forward.y = deltaTime*speed;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = deltaTime*speed;
    right.y = 0;

    if (self->canMove) {
        if (keys[SDL_SCANCODE_LSHIFT])
            speed = 60;
        else
            speed = 30;
        if (keys[SDL_SCANCODE_W])
        {
            //vector3d_add(self->position, self->position, forward);
        }
        if (keys[SDL_SCANCODE_S])
        {
            //vector3d_add(self->position, self->position, -forward);
        }
        if (keys[SDL_SCANCODE_D])
        {
            vector3d_add(self->position, self->position, right);
        }
        if (keys[SDL_SCANCODE_A])
        {
            vector3d_add(self->position, self->position, -right);
        }
        if (keys[SDL_SCANCODE_SPACE])self->position.z += 1 * deltaTime * speed;
        if (keys[SDL_SCANCODE_LCTRL])self->position.z -= 1 * deltaTime * speed;
    }
    vector3d_sub(deltaPos,  self->position, posPrime);


    if(vector3d_magnitude(deltaPos))moveChildren();
   // if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    //if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    //if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    //if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;
    
    //if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    //if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    //if (keys[SDL_SCANCODE_F3])
    //{
    //    thirdPersonMode = !thirdPersonMode;
    //    self->hidden = !self->hidden;
    //}
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);
}

void addChild(Object* object) 
{
    if (!object)return;
    ObjectLinked* obj = malloc(sizeof(ObjectLinked));
    if (!object)return;
    obj->self = object;
    obj->next = playerChildren;
    playerChildren = obj;
}

void moveChildren()
{
    ObjectLinked* iter = playerChildren;

    if (playerChildren->self == NULL) return;
    do 
    {
        vector3d_add(iter->self->position, iter->self->position, deltaPos);
        iter = iter->next;
    } while (iter->self != NULL);

}

void removeChild();//todo


/*eol@eof*/

#include "object.h"
#include "charenemy.h"
#include <stdlib.h> // Include the necessary headers

// Function to create a new object
Object* NewObject()
{
    Object* newObject = (Object*)malloc(sizeof(Object));
    if (newObject == NULL) {
        return NULL; // Memory allocation failed
    }

    newObject->ID = 0; // Set the ID as needed
    
    newObject->Type = other; // Set the type as needed

    newObject->Data.Other = NULL; // Initialize as needed

    newObject->color = GFC_COLOR_WHITE;
    newObject->ambiant = vector4d(1, 1, 1, 1);

    newObject->model = NULL;

    newObject->position = vector3d(0, 0, 0); // Set the position as needed
    newObject->velocity = vector3d(0, 0, 0); // Set the position as needed
    newObject->acceleration = vector3d(0, 0, 0); // Set the position as needed
    newObject->rotation = vector3d(0, 0, 0);
    newObject->scale = vector3d(.3, .3, .3);

    newObject->update = &UpdateMe;

    newObject->isStatic = false;
    newObject->isActive = false;

    gfc_matrix_identity(newObject->modelMat);



    //Remember to set model,modelmat and position!!!!

    return newObject;
}

// Function to free an object
void FreeObject(Object* object)
{
    if (object == NULL) {
        return;
    }

    // Free any object-specific data
    switch (object->Type) {
    case charenemy:
        // Free CharenemyObject-specific data
        // You'll need to free any Charenemy-related data here
        break;
    case enviorment:
        // Free enviorment-specific data
        // Your implementation here for freeing enviorment-specific data
        break;
    case projectile:
        // Free projectile-specific data
        // Your implementation here for freeing projectile-specific data
        break;
    case specialAttack:
        // Free specialAttack-specific data
        // Your implementation here for freeing specialAttack-specific data
        break;
    case particle:
        // Free particle-specific data
        // Your implementation here for freeing particle-specific data
        break;
    default:
        // Handle other types as needed
        break;
    }

    free(object);
}

void DrawObject(Object* self) {
    if (!self)return;
    if (!(self->isActive))return;
    if (!self->model)return;
    if (self->model) gf3d_model_draw(self->model, self->modelMat, gfc_color_to_vector4f(self->color), vector4d(1, 1, 1, 1));
}

void UpdateMe(Object* self) {
    if (!self)return;
    if (!self->isStatic) 
    {
        vector3d_add(self->position, self->position, self->velocity);
        vector3d_add(self->velocity, self->acceleration, self->velocity);

        gfc_matrix_identity(self->modelMat);

        gfc_matrix_scale(self->modelMat, self->scale);
        gfc_matrix_rotate_by_vector(self->modelMat, self->modelMat, self->rotation);
        gfc_matrix_translate(self->modelMat, self->position);
    }
}

void UpdateObject(Object* self) 
{
    if (self->update)self->update(self);
}

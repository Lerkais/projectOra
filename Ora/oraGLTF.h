#pragma once

#include "gf3d_model.h"
#include "gf3d_mesh.h"
#include "simple_json.h"

// Forward declaration
typedef struct Node_S Node;

typedef struct Buffer_S
{
    unsigned long byteLength;
    char* uri; // file path relative
    void* data;
} Buffer;

typedef struct BufferView_S
{
    unsigned int buffer;
    unsigned long byteOffset;
    unsigned long byteLength;
    unsigned int byteStride;
    unsigned long target;
    char* name;

} BufferView;

typedef struct Accessor_S
{
    BufferView bufferView;
    unsigned long byteOffset;
    unsigned int componentType;
    unsigned int count;
    char* type;
    int* max; // used differently based on type
    int* min; // ditto
} Accessor;

typedef struct Node_S
{
    Node* children;
    unsigned int childrenCount;
    unsigned int mesh;
    Vector3D translation;
    Vector3D rotation;
    Vector3D scale;
    Matrix4 mat;
    char* name;
} Node;

typedef struct gltfScene_S
{
    Node* nodes;
    unsigned int nodeCount;
} gltfScene;

typedef struct Image_S
{
    char* name;
    char* uri; // file path relative
} Image;

typedef struct OraTexture_S
{
    Image image;
} OraTexture;

typedef struct Material_S //todo
{
    char* name;
} Material;

typedef struct MeshPrimitive_S
{
    unsigned int vertexBufferIndex;
    unsigned int indexBufferIndex;
    unsigned int materialIndex;
    unsigned int mode;
} MeshPrimitive;

typedef struct OraMesh_S
{
    MeshPrimitive* primitives;
    unsigned int numPrimitives;
} OraMesh;

typedef struct gltfModel_S
{
    gltfScene scene;
    Node* nodes;
    unsigned int nodeCount;
    Buffer* buffers;
    unsigned int bufferCount;
    BufferView* bufferViews;
    unsigned int bufferViewsCount;
    Accessor* accessors;
    unsigned int accessorCount;
    Texture* textures;
    unsigned int textureCount;
    Material* materials;
    unsigned int materialCount;
    OraMesh* meshes;
    unsigned int meshCount;
    Image* images;
    unsigned int imageCount;
} gltfModel;

// Load glTF model from file
gltfModel* loadGltfModel(const char* filePath);


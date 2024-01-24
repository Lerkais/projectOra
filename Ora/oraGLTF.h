#pragma once

#include "gf3d_model.h"
#include "gf3d_mesh.h"
#include "simple_json.h"
#include <stdbool.h>

// Forward declaration
typedef struct Node_S Node;

typedef struct Buffer_S
{
    unsigned long byteLength;
    char uri[256]; // file path relative
    char* data;
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
    int bufferView;
    unsigned long byteOffset;
    unsigned int componentType;
    unsigned int count;
    char type[16];
    int* max; // used differently based on type
    int* min; // ditto
} Accessor;

typedef struct Node_S
{
    int* children;
    unsigned int childrenCount;
    unsigned int mesh;
    bool hasMesh;
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
    char uri[64]; // file path relative
    char* filepath[256];
    Texture* imageFileLoaded;
} Image;

typedef struct OraTexture_S
{
    unsigned int source;
    unsigned int sampler;
} OraTexture;

typedef struct Material_S {
    // Base Color
    float baseColorFactor[4]; // RGBA
    int baseColorTexture; // Index to texture

    // Metallic-Roughness Workflow
    float metallicFactor; // 0.0 to 1.0
    float roughnessFactor; // 0.0 to 1.0
    int metallicRoughnessTexture; //Index of texture

    // Normal, Occlusion, and Emissive Maps
    int normalTexture;
    int occlusionTexture;
    float emissiveFactor[3]; // RGB
    int emissiveTexture;

    // Alpha Mode and Cutoff
    int alphaMode;           // Opaque, Mask, or Blend
    float alphaCutoff;       // Cutoff threshold for alpha mask

    // Double Sided
    int doubleSided;         // 0 or 1 (false/true)
} Material;

typedef struct OraAtribute_S
{
    int normal;
    int position;
    unsigned int* texcoords;
    unsigned int texcoordscount;
}OraAtribute;

typedef struct MeshPrimitive_S
{
    OraAtribute atributes; //geometric data
    unsigned int indices;
    unsigned int material;
    unsigned int mode;
} MeshPrimitive;

typedef struct OraMesh_S
{
    MeshPrimitive* primitives;
    unsigned int primcount;
} OraMesh;

typedef enum SamplerType_E
{
    STEPst = 0,
    LINEARst = 1
}SamplerType;

typedef struct Sampler_S
{
    unsigned int input;
    SamplerType interpolation;
    unsigned int output;
}Sampler;

typedef enum ChannelType_E
{
    translationct = 0,
    rotationct = 1,
    scalect = 2
}ChannelType;

typedef struct Channel_S
{
    unsigned int sampler;
    ChannelType targetpath;
    unsigned targetnode;
}Channel;

typedef struct Animation_S
{
    //animation stuff
    Sampler* samplers;
    unsigned int samplercount;
    Channel* channels;
    unsigned int channelcount;
    //todo name
}Animation;

typedef struct Skin_S
{
    unsigned int mat;
    unsigned int* joints;
    unsigned int jointCount;
}Skin;

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
    
    Material* materials;
    unsigned int materialCount;
    OraMesh* meshes;
    unsigned int meshCount;
    Mesh* gfmeshes; //same count as meshes
    Image* Images;
    unsigned int imageCount;
    OraTexture* textures;
    Texture* loadedtextures;
    unsigned int textureCount;
    Model model;

    Animation* animations;
    unsigned int animationcount;

    Skin* skins;
    unsigned int skincount;



} gltfModel;

// Load glTF model from file into standard model struct
Model* loadModelfromgltf(const char* filename);

void gltf_model_draw(gltfModel* g, Matrix4 modelMat, Vector4D colorMod, Vector4D ambientLight);

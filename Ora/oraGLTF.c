#include "oraGLTF.h"



void getfolderpath(const char* filePath, char* folderPath) {
	//Find the last occurrence of the directory separator ('/' or '\') in the file path
	const char* lastSlash = strrchr(filePath, '/');
	const char* lastBackslash = strrchr(filePath, '\\');

	// Use the max of the two positions to get the last
	const char* lastSeparator = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;

	if (lastSeparator != NULL)
	{
		size_t folderPathLength = lastSeparator - filePath + 1;

		strncpy(folderPath, filePath, folderPathLength);
		folderPath[folderPathLength] = '\0'; //Null-terminate
	}
	else
	{
		strcpy(folderPath, "./");
	}
}
// Load glTF model from file
gltfModel* loadGltfModel(const char* filePath)
{
	SJson* json, *bvjson,*bjsons,*bjson;

	json = sj_load(filePath);

	//Asuming 1 buffer for now

	gltfModel* model = calloc(1, sizeof(gltfModel)); //initalized all memory as null

	bjsons = sj_object_get_value(json, "buffers");
	bjson = sj_array_get_nth(bjsons, 0);
	char* binfileuri = sj_object_get_value_as_string(bjson, "uri");
	char binfilename [256],folder[256];
	getfolderpath(filePath, folder);
	strcpy(binfilename, folder);
	strcat(binfilename, binfileuri);

	FILE* binfile = fopen(binfilename, "rb");
	long filesize;
	if (fseek(binfile, 0, SEEK_END) != 0)
	{
		//error
	}

	filesize = ftell(binfile);

	if (fseek(binfile, 0, SEEK_SET) != 0)
	{
		//error
	}

	char* bindata = malloc(filesize);

	size_t bytesRead = fread(bindata, 1, filesize, binfile);

	if (bytesRead != filesize) {
		//error
	}
	

	//mesh data
	SJson* meshes, * mesh, * primitives, * primitive,*atri; //todo set up mesh array and mesh structs and for prims
	int pos = 0;

	meshes = sj_object_get_value(json, "meshes");
	mesh = sj_array_get_nth(meshes, 0);
	primitives = sj_object_get_value(mesh, "primitives");
	primitive = sj_array_get_nth(primitives, 0);
	atri = sj_object_get_value(primitive, "attributes");
	sj_object_get_value_as_int(atri, "POSITION", pos);

	//accessor for position //todo do this for each primitive/atribute on meshes
	Accessor accessor;
	SJson* accs, * acc;
	accs = sj_object_get_value(json, "accessors");
	acc = sj_array_get_nth(accs, pos);
	int bvpos; sj_object_get_value_as_int(acc, "bufferView", &bvpos);


	//buffer views
	int bvcount;

	bvjson = sj_object_get_value(json, "bufferViews");
	model->bufferViewsCount = sj_array_get_count(bvjson);
	BufferView* bvs = calloc(model->bufferViewsCount, sizeof(BufferView));
	model->bufferViews = bvs;

	for (int i = 0; i < model->bufferViewsCount; i++)
	{
		BufferView bv={0};
		SJson* idp = sj_array_get_nth(bvjson, i);
		bv.name = calloc(64, sizeof(char));
		strcpy(bv.name, sj_object_get_value_as_string(idp, "name"));
		sj_object_get_value_as_int(idp, "buffer", bv.buffer);
		sj_object_get_value_as_int(idp, "byteLength", bv.byteLength);
		sj_object_get_value_as_int(idp, "byteOffset", bv.byteOffset);
		sj_object_get_value_as_int(idp, "byteStride", bv.byteStride);
		sj_object_get_value_as_int(idp, "target", bv.target);
		model->bufferViews[i] = bv;
	}

	BufferView bv = bvs[bvpos];


	char* buffer = (bindata + bv.byteOffset); 

	int count; sj_object_get_value_as_int(acc, "count", &count);

	for (int i = 0; i < count; i++)
	{
		float* v = buffer + i * bv.byteStride;
		if(v[0]+v[1]+v[2] > .0001f)
		printf("Vertex: (%f,%f,%f)\n", v[0], v[1], v[2]);
	}

	printf("%i vertexes found", count);

}

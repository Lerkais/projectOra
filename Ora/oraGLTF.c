#include "oraGLTF.h"
#include "simple_logger.h"

void testModel(gltfModel* m);

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
	Buffer buffer = {0};

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

	fclose(binfile);

	if (bytesRead != filesize) {
		//error
	}

	buffer.byteLength = bytesRead;
	strcpy(buffer.uri, binfileuri);
	buffer.data = bindata;

	model->bufferCount = 1;
	model->buffers = calloc(model->bufferCount, sizeof(Buffer));
	model->buffers[0] = buffer;
	

	
	//accessor setup
	SJson* accs;
	accs = sj_object_get_value(json, "accessors");
	model->accessorCount = sj_array_get_count(accs);
	model->accessors = calloc(model->accessorCount, sizeof(Accessor)); if (!model->accessors) return;
	for (unsigned int i = 0; i < model->accessorCount; i++)
	{
		Accessor accessor = {0};
		SJson* sjacc;
		sjacc = sj_array_get_nth(accs, i);
		sj_object_get_value_as_int(sjacc, "componentType",&accessor.componentType);
		sj_object_get_value_as_int(sjacc, "bufferView", &accessor.bufferView);
		sj_object_get_value_as_int(sjacc, "count", &accessor.count);
		char* type = sj_object_get_value_as_string(sjacc, "type");
		strcpy(accessor.type, type);
		model->accessors[i] = accessor;
	}


	//mesh json data
	SJson* meshes, * sjmesh, * primitives, * sjprim,*atri; //todo set up mesh array and mesh structs and for prims
	int meshcount = 0;


	meshes = sj_object_get_value(json, "meshes");
	meshcount = sj_array_get_count(meshes);
	model->meshCount = meshcount;
	model->meshes = calloc(meshcount, sizeof(OraMesh)); if (!model->meshes) return;
	for (int i = 0; i < meshcount; i++)
	{
		OraMesh mesh = { 0 };

		sjmesh = sj_array_get_nth(meshes, i);
		primitives = sj_object_get_value(sjmesh, "primitives");
		mesh.primcount = sj_array_get_count(primitives);
		mesh.primitives = calloc(mesh.primcount, sizeof(MeshPrimitive));
		for (unsigned int j = 0; j < mesh.primcount; j++)
		{
			sjprim = sj_array_get_nth(primitives, j);
			atri = sj_object_get_value(sjprim, "attributes");
			MeshPrimitive prim = {0};
			unsigned int texcount = sj_list_get_count(sj_object_get_keys_list(atri)); texcount = texcount > 2 ? texcount-2 : 0;
			prim.atributes.texcoordscount = texcount;
			prim.atributes.texcoords = calloc(texcount, sizeof(int));
			sj_object_get_value_as_int(atri, "NORMAL", &prim.atributes.normal);
			sj_object_get_value_as_int(atri, "POSITION", &prim.atributes.position);
			for (unsigned int k = 0; k < texcount; k++)
			{
				char key[16] = "TEXCOORD_",toapp[8];
				sprintf(toapp, "%i", k);
				strcat(key, toapp);
				//printf("texcoord key: %s\n", key);
				sj_object_get_value_as_int(atri, key, &prim.atributes.texcoords[k]);
			}
			if (sj_object_get_value_as_int(sjprim, "indices", &prim.indices))
			{
				

			}
			if (sj_object_get_value_as_int(sjprim, "material", &prim.material))
			{
				//todo material
			}
			if (sj_object_get_value_as_int(sjprim, "mode", &prim.mode))
			{
				//todo mode
			}
			mesh.primitives[j] = prim;
		}

		model->meshes[i] = mesh;

	}

	//buffer views setup
	int bvcount;

	bvjson = sj_object_get_value(json, "bufferViews");
	model->bufferViewsCount = sj_array_get_count(bvjson);
	BufferView* bvs = calloc(model->bufferViewsCount, sizeof(BufferView));
	if (!bvs)
		return;
	model->bufferViews = bvs;

	for (unsigned int i = 0; i < model->bufferViewsCount; i++)
	{
		BufferView bv={0};
		SJson* idp = sj_array_get_nth(bvjson, i);
		sj_object_get_value_as_int(idp, "buffer", &bv.buffer);
		sj_object_get_value_as_int(idp, "byteLength", &bv.byteLength);
		sj_object_get_value_as_int(idp, "byteOffset", &bv.byteOffset);
		sj_object_get_value_as_int(idp, "byteStride", &bv.byteStride);
		bv.byteStride = max(bv.byteStride, 1);
		sj_object_get_value_as_int(idp, "target", &bv.target);
		model->bufferViews[i] = bv;
	}


	//image setup


	testModel(model);

	return model;

}

Model* loadModelfromgltf(const char* filename)
{
	gltfModel* gm = loadGltfModel(filename);

	Model* m = gf3d_model_new();

	m->mesh = &(gm->gfmeshes[0]);
	m->meshcount = gm->meshCount;
	


	m->texture = gf3d_texture_load("images/default.png");



	

	return m;
}

void testModel(gltfModel* m)
{
	//find all vertices and normals
	Mesh* meshes = calloc(m->meshCount, sizeof(Mesh));

	for (unsigned int i = 0; i < m->meshCount;i++)
	{
		Mesh mesh;

		Vertex* verts = NULL;
		Face* faces = NULL;

		int vcount = 0, fcount = 0;

		if (!verts || !faces)
		{
			//printf("fail");
		}

		for (unsigned int j = 0; j < m->meshes[i].primcount; j++)
		{
			int accPosInd = m->meshes[i].primitives[j].atributes.position;
			int accNorInd = m->meshes[i].primitives[j].atributes.normal;
			int indices = m->meshes[i].primitives[j].indices;

			Accessor a1, a2;
			a1 = m->accessors[accPosInd];
			a2 = m->accessors[accNorInd];

			BufferView b1, b2;
			b1 = m->bufferViews[a1.bufferView];
			b2 = m->bufferViews[a2.bufferView];

			Buffer bu1, bu2;
			bu1 = m->buffers[b1.buffer];
			bu2 = m->buffers[b2.buffer];

			char* data1, * data2;
			data1 = (bu1.data + b1.byteOffset);
			data2 = (bu2.data + b2.byteOffset);

			//counts are how many vector3's there are
			int c1, c2;
			c1 = a1.count;
			c2 = a2.count;
			
			vcount += c1;//A pos is 3 floats, count is given by json file
			
			//face's vertex's are indicaited by three indexs stored in accessor at index "indices"

			Accessor inAcc = m->accessors[indices];
			BufferView inbv = m->bufferViews[inAcc.bufferView];
			Buffer inb = m->buffers[inbv.buffer];

			char* indata = (inb.data + inbv.byteOffset);
			int incount = inAcc.count; //5123 for component type means unsigned short

			fcount += incount/3;

			void* temp1, *temp2;

			if (!verts || !faces)
			{
				verts = calloc(vcount, sizeof(Vertex));
				faces = calloc(fcount, sizeof(Face));
			}
			else
			{
				temp1 = realloc(verts, vcount * sizeof(Vertex));
				temp2 = realloc(faces, fcount * sizeof(Face));

				if (!temp1 || !temp2)
				{
					printf("fail");
					return;
				}

				verts = temp1;
				faces = temp2;
			}


			size_t traversalSize = sizeof(float) * 3;

			//normals and positions are vector3's
			//traverse by 3 floats at a time
			for (unsigned int k = 0; k < c1; k++)
			{
				Vertex ver = { 0 };
				float* v = data1 + k * traversalSize;
				ver.vertex = vector3d(v[0], v[2], v[1]);
				if (abs(v[0]) > .0001f || abs(v[1]) > .0001f || abs(v[2]) > .0001f) {
					//printf("Vert:\n(%f,%f,%f)\n", v[0], v[1], v[2]);
				}
				verts[k] = ver;
			}
			for (unsigned int k = 0; k < c1; k++)
			{
				float* v = data2 + k * traversalSize;//, b2.byteStride);
				//if (abs(v[0]) > .0001f || abs(v[1]) > .0001f || abs(v[2] > .0001f)) { printf("Normal:\n(%f,%f,%f)\n", v[0], v[2], v[1]); }
				verts[k].normal = vector3d(v[0], v[1], v[2]);
			}

			//parse faces

			//printf("Indices count to vert count: %i,%i\n", incount,c1);

			for (unsigned int k = 0; k < incount/3; k++)
			{
				Face face = {0};
				unsigned short* ind = indata + 3 * k * sizeof(unsigned short);
				face.verts[0] = ind[0];
				face.verts[1] = ind[1];
				face.verts[2] = ind[2];
				faces[k] = face;
			}
			unsigned short* as = indata;

			printf("indata: (%i,%i,%i), bufferview: %i\n", as[0], as[1], as[2],inAcc.bufferView);


			gf3d_mesh_create_vertex_buffer_from_vertices(&mesh, verts, vcount, faces, fcount);
			printf("done with mesh %i of %i\n",i+1,m->meshCount);
			if (verts)
				free(verts);
			if (faces)
				free(faces);
			
			meshes[i] = mesh;

		}
	}

	printf("Mesh Query: \n");
	for (int i = 0; i < m->meshCount; i++)
	{
		printf("Mesh %i face count: %i\n", i + 1, meshes[i].faceCount);
	}

	m->gfmeshes = meshes;

}
#include "oraGLTF.h"
#include "simple_logger.h"


typedef struct {
	float x, y, z, w;
} Quaternion;

//Adapted from wiki
Vector3D QuaternionToEuler(const Quaternion* q) {
	Vector3D euler;

	// Roll (X-axis rotation)
	float sinr_cosp = 2 * (q->w * q->x + q->y * q->z);
	float cosr_cosp = 1 - 2 * (q->x * q->x + q->y * q->y);
	euler.x = atan2(sinr_cosp, cosr_cosp);

	// Pitch (Y-axis rotation)
	float sinp = 2 * (q->w * q->y - q->z * q->x);
	if (fabs(sinp) >= 1)
		euler.y = copysign(M_PI / 2, sinp); // Use 90 degrees if out of range
	else
		euler.y = asin(sinp);

	// Yaw (Z-axis rotation)
	float siny_cosp = 2 * (q->w * q->z + q->x * q->y);
	float cosy_cosp = 1 - 2 * (q->y * q->y + q->z * q->z);
	euler.z = atan2(siny_cosp, cosy_cosp);

	return euler;
}

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
		folderPath[folderPathLength] = '\0'; //null-terminator
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

	// Buffer Setup
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
			if (sj_object_get_value_as_int(sjprim, "mode", &prim.mode))
			{
				//todo mode
			}
			if (sj_object_get_value_as_int(sjprim, "material", &prim.material))
			{
			
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


	//material setup
	SJson* sjmaters;
	sjmaters = sj_object_get_value(json, "materials");
	if (sjmaters)
	{
		model->materialCount = sj_array_get_count(sjmaters);
		Material* maters = calloc(model->materialCount, sizeof(Material));
		//todo if maters etc
		model->materials = maters;
		for (unsigned int i = 0; i < model->materialCount; i++)
		{
			Material mater = { 0 };
			SJson* sjmater = sj_array_get_nth(sjmaters, i);

			//todo baseColorFactor int[4]


			// Metallic Roughness Setup
			SJson* sjmetal = sj_object_get_value(sjmater, "pbrMetallicRoughness");
			if (sjmetal)
			{
				// Base Colors
				SJson* sjBaseColors = sj_object_get_value(sjmetal, "baseColorFactor");
				if (sjBaseColors)
				{
					for (unsigned int j = 0; j < 4; j++)
					{
						SJson* sjcolor = sj_array_get_nth(sjBaseColors, j);
						float color; sj_get_float_value(sjcolor, &color);
						mater.baseColorFactor[j] = color;
					}
				}

				// Base Color Texture
				SJson* bct = sj_object_get_value(sjmetal, "baseColorTexture");
				if (bct)
					sj_object_get_value_as_int(bct, "index", &mater.baseColorTexture);

				// Metalic and roughness factors
				sj_object_get_value_as_float(sjmetal, "metallicFactor", &mater.metallicFactor);
				sj_object_get_value_as_float(sjmetal, "roughnessFactor", &mater.roughnessFactor);
			}

			// Emissive Setup
			SJson* et = sj_object_get_value(sjmater, "emissiveTexture");
			if (et)
			{
				sj_object_get_value_as_int(et, "index", &mater.emissiveTexture);
			}
			SJson* ef = sj_object_get_value(sjmater, "emissiveFactor");
			if (ef)
			{
				for (unsigned int j = 0; j < 3; j++)
				{
					SJson* sjf = sj_array_get_nth(ef, j);
					float f; sj_get_float_value(sjf, &f);
					mater.emissiveFactor[j] = f;
				}
			}

			//todo normal and occlusion map textures

			//todo Alpha Mode and Cutoff

			//todo doublesided

			model->materials[i] = mater;
		}
	}
	else
	{
		model->materialCount = 1;
		Material* mater = calloc(1,sizeof(Material));
		Material smater = { 0 };
		for (int j = 0; j < 4; j++)
		{
			smater.baseColorFactor[j] = 1;
		}
		smater.baseColorTexture = 0;
		*mater = smater;
		model->materials = mater;
	}
	

	// Image URI Setup
	SJson* sjimags = sj_object_get_value(json, "images");

	if (sjimags)
	{
		model->imageCount = sj_array_get_count(sjimags);
		model->Images = calloc(model->imageCount, sizeof(Image));
		for (unsigned int i = 0; i < model->imageCount; i++)
		{
			Image imag = { 0 };
			SJson* sjimag = sj_array_get_nth(sjimags, i);
			strcpy(imag.uri ,sj_object_get_value_as_string(sjimag, "uri"));
			strcpy(imag.filepath, folder);
			strcat(imag.filepath, imag.uri);
			//iamge loading
			Texture* gf3dtexture = gf3d_texture_load(imag.filepath);
			if (!gf3dtexture)
			{
				//error
				slog("Death by missing texture file");
			}
			imag.imageFileLoaded = gf3dtexture;
			model->Images[i] = imag;
		}
	}

	// Textures Setup
	SJson* sjtexts = sj_object_get_value(json, "textures");
	if (sjtexts)
	{
		model->textureCount = sj_array_get_count(sjtexts);
		model->textures = calloc(model->textureCount, sizeof(OraTexture));
		model->loadedtextures = calloc(model->textureCount, sizeof(Texture)); //todo check image count always matches texture count
		for (unsigned int i = 0; i < model->textureCount; i++)
		{
			OraTexture text = { 0 };
			SJson* sjtext = sj_array_get_nth(sjtexts, i);
			sj_object_get_value_as_int(sjtext, "source", &text.source);
			sj_object_get_value_as_int(sjtext, "sampler", &text.sampler);
			model->loadedtextures[i] = *(model->Images[text.source].imageFileLoaded);
		}
	}
	else
	{
		model->textureCount = 0;
		model->textures = calloc(model->textureCount, sizeof(OraTexture));
		model->loadedtextures = calloc(model->textureCount, sizeof(Texture));
	}

	// Node Setup
	SJson* sjnodes = sj_object_get_value(json, "nodes");
	if (sjnodes)
	{
		model->nodeCount = sj_array_get_count(sjnodes);
		model->nodes = calloc(model->nodeCount, sizeof(Node));
		if (!model->nodes)
		{
			slog("err");
		}
		for (int i = 0; i < model->nodeCount; i++)
		{
			Node node = { 0 };
			SJson* sjnode = sj_array_get_nth(sjnodes, i);
			SJson* sjtrans = sj_object_get_value(sjnode, "translation");
			int stor[4] = { 0 };
			int c3 = sj_array_get_count(sjtrans);
			for (unsigned int j = 0; j < c3; j++) //watch for obo err
			{
				SJson* sjtranPart = sj_array_get_nth(sjtrans, j);

				sj_get_integer_value(sjtranPart, &stor[j]);

			}
			node.translation = vector3d(stor[0], -stor[2], stor[1]);
			
			SJson* sjrots = sj_object_get_value(sjnode, "rotation");
			c3 = sj_array_get_count(sjrots);
			Quaternion q = { 0 };
			for (unsigned int j = 0; j < c3; j++) //watch for obo err
			{
				SJson* sjPart = sj_array_get_nth(sjrots, j);

				sj_get_integer_value(sjPart, &stor[j]);

			}
			q.x = stor[0];
			q.y = -stor[2];
			q.z = stor[1];
			q.w = stor[3];
			node.rotation = QuaternionToEuler(&q);

			SJson* sjscale = sj_object_get_value(sjnode, "scale");
			c3 = sj_array_get_count(sjscale);
			if (sjscale)
			{
				for (unsigned int j = 0; j < c3; j++) //watch for obo err
				{
					SJson* sjPart = sj_array_get_nth(sjrots, j);

					sj_get_integer_value(sjPart, &stor[j]);

				}
				node.scale = vector3d(stor[0], -stor[2], stor[1]);
			}
			else
			{
				node.scale = vector3d(1, 1, 1);
			}
			
			
			SJson* sjchildren = sj_object_get_value(sjnode,"children");
			node.childrenCount = sj_array_get_count(sjchildren);
			node.children = calloc(node.childrenCount, sizeof(Node));
			for (int j = 0; j < node.childrenCount; j++)
			{
				SJson* sjchild = sj_array_get_nth(sjchildren, j);
				sj_get_integer_value(sjchild, node.children[j]);
			}

			SJson* sjmeshInd = sj_object_get_value(sjnode, "mesh");
			if (sjmeshInd)
			{
				sj_get_integer_value(sjmeshInd, &node.mesh);
				node.hasMesh = true;
			}
			else
			{
				node.hasMesh = false;
			}


			gfc_matrix_identity(node.mat);

			gfc_matrix_scale(node.mat, node.scale);
			//gfc_matrix_rotate_by_vector(node.mat, node.mat, node.rotation);
			
			gfc_matrix_translate(node.mat, node.translation);

			
			model->nodes[i] = node;
		}

		// Build node tranformation final matrices
		for (int i = 0; i < model->nodeCount; i++)
		{
			Node* node = &model->nodes[i];
			for (int j = 0; j < node->childrenCount; j++)
			{
				int ind = node->children[j];
				gfc_matrix_multiply(model->nodes[ind].mat, node->mat, model->nodes[ind].mat);
			}
		}



	}


	// Animation setup

	SJson* sjanimations = sj_object_get_value(json, "animations");
	if (sjanimations)
	{
		model->animationcount = sj_array_get_count(sjanimations);
		model->animations = calloc(model->animationcount, sizeof(Animation));

		for (unsigned int i = 0; i < model->animationcount; i++)
		{
			Animation ani = { 0 };

			SJson* sjani = sj_array_get_nth(sjanimations, i);

			SJson* sjchannels = sj_object_get_value(sjani, "channels"),
				* sjsamplers = sj_object_get_value(sjani, "samplers");
			ani.channelcount = sj_array_get_count(sjchannels);
			ani.samplercount = sj_array_get_count(sjsamplers);

			ani.channels = calloc(ani.channelcount, sizeof(Channel));
			ani.samplers = calloc(ani.samplercount, sizeof(Sampler));

			if (sjchannels && sjsamplers)
			{
				for (int j = 0; j < ani.samplercount;j++)
				{
					SJson* sjsam = sj_array_get_nth(sjsamplers, j);
					Sampler sam = { 0 };
					sj_object_get_value_as_int(sjsam, "input", &sam.input);
					sj_object_get_value_as_int(sjsam, "output", &sam.output);
					char* type;
					type = sj_object_get_value_as_string(sjsam, "interpolation");

					if (type && strcmp(type, "STEP"))
						sam.interpolation = STEPst;
					else if (type && strcmp(type, "LINEAR"))
						sam.interpolation = LINEARst;

					//wierd third case animation not handeled at this time

					ani.samplers[j] = sam;

				}
				for (int j = 0; j < ani.channelcount; j++)
				{
					SJson* sjcha = sj_array_get_nth(sjchannels, j);
					Channel cha = { 0 };

					sj_object_get_value_as_int(sjcha, "sampler", &cha.sampler);

					SJson* sjtar = sj_object_get_value(sjcha, "target");

					sj_object_get_value_as_int(sjtar, "node", &cha.targetnode);

					char* type;
					type = sj_object_get_value_as_string(sjtar, "path");

					if (type && strcmp(type, "translation"))
						cha.targetpath = translationct;
					else if (type && strcmp(type, "rotation"))
						cha.targetpath = rotationct;
					else if (type && strcmp(type, "scale"))
						cha.targetpath = scalect;

					ani.channels[j] = cha;

				}

			}

			model->animations[i] = ani;

		}		
	}

	// Skin
	SJson* sjskins = sj_object_get_value(json, "skins");
	if (sjskins)
	{
		model->skincount = sj_array_get_count(sjskins);
		for (unsigned int i = 0; i < model->skincount; i++)
		{
			SJson* sjskin = sj_array_get_nth(sjskins, i);
			Skin sk = { 0 };
			sj_object_get_value_as_int(sjskin, "inverseBindMatrices", &sk.mat);

			SJson* sjjoints = sj_object_get_value(sjskin, "joints");
			sk.jointCount = sj_array_get_count(sjjoints);
			sk.joints = calloc(sk.jointCount, sizeof(unsigned int));
			for (unsigned int j = 0; j < sk.jointCount; j++)
			{
				SJson* sjjoint = sj_array_get_nth(sjjoints, j);
				sj_get_integer_value(sjjoint, &sk.joints[j]);
			}

			//todo name
		}
	}





	testModel(model);

	return model;

}

Model* loadModelfromgltf(const char* filename)
{
	gltfModel* gm = loadGltfModel(filename);

	Model* m = gf3d_model_new();

	m->mesh = &(gm->gfmeshes[0]);
	m->meshcount = gm->meshCount;
	

	if (gm->textureCount == 0)
	{
		m->texture = gf3d_texture_load("images/default.png");
		m->textcount = 1;
	}
	else
	{
		m->textcount = gm->textureCount;
		m->texture = gm->loadedtextures;
	}
	 
	m->data = gm;
	

	return m;
}

typedef enum gltfCType_E
{
	gltfUSHORT = 5123,
	gltfUINT = 5125,
	gltfFLOAT = 5126
}gltfCType;

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
				ver.vertex = vector3d(v[0], -v[2], v[1]);
				if (abs(v[0]) > .0001f || abs(v[1]) > .0001f || abs(v[2]) > .0001f) {
					//printf("Vert:\n(%f,%f,%f)\n", v[0], v[1], v[2]);
				}
				verts[k] = ver;
			}
			for (unsigned int k = 0; k < c1; k++)
			{
				float* v = data2 + k * traversalSize;//, b2.byteStride);
				//if (abs(v[0]) > .0001f || abs(v[1]) > .0001f || abs(v[2] > .0001f)) { printf("Normal:\n(%f,%f,%f)\n", v[0], v[2], v[1]); }
				verts[k].normal = vector3d(v[0], v[2], v[1]);
			}

			//parse faces

			//printf("Indices count to vert count: %i,%i\n", incount,c1);

			unsigned int type = inAcc.componentType;




			switch (type)
			{
			case gltfUSHORT:
				for (unsigned int k = 0; k < incount / 3; k++)
				{
					Face face = { 0 };
					unsigned short* ind = indata + 3 * k * sizeof(unsigned short);
					face.verts[0] = ind[0];
					face.verts[1] = ind[1];
					face.verts[2] = ind[2];
					faces[k] = face;
				}
				unsigned short* as = indata;
				printf("indata: (%i,%i,%i), bufferview: %i\n", as[0], as[1], as[2], inAcc.bufferView);
				break;
			case gltfUINT:
				for (unsigned int k = 0; k < incount / 3; k++)
				{
					Face face = { 0 };
					unsigned int* ind = indata + 3 * k * sizeof(unsigned int);
					face.verts[0] = ind[0];
					face.verts[1] = ind[1];
					face.verts[2] = ind[2];
					faces[k] = face;
				}
				unsigned int* ai = indata;
				printf("indata: (%i,%i,%i), bufferview: %i\n", ai[0], ai[1], ai[2], inAcc.bufferView);
				break;
			default:
				printf("Err unhandled component type: %u\n",type);
				break;
			}
			
			

			

			//material link
			int materialInd = m->meshes[i].primitives[j].material;
			Material mat = m->materials[materialInd];



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



typedef struct RenderContainer_S
{
	Model model;
	VkDescriptorSet* descriptorSet;
	VkCommandBuffer commandBuffer;
	Uint32 bufferFrame;
	Matrix4 modelMat;
	Vector4D colorMod;
	Vector4D ambientLight;
}RenderContainer;

void gltf_model_draw(Model* model, Matrix4 modelMat, Vector4D colorMod, Vector4D ambientLight)
{	
	ModelManager gf3d_model = getModelManager();
	gltfModel* g = model->data;
	VkDescriptorSet* descriptorSet = NULL;
	VkCommandBuffer commandBuffer;
	Uint32 bufferFrame;
	if (!model)
	{
		return;
	}
	commandBuffer = gf3d_mesh_get_model_command_buffer();
	bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
	descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_model.pipe, bufferFrame);
	if (descriptorSet == NULL)
	{
		slog("failed to get a free descriptor Set for model rendering");
		return;
	}


	if (g->nodeCount > 0)
	{
		for (int i = 0; i < g->nodeCount; i++)
		{
			Node* node = &(g->nodes[i]);
			if (node->hasMesh)
			{
				int meshIndex = node->mesh;

				int matInd = g->meshes[meshIndex].primitives[0].material;
				Material* mater = &g->materials[matInd];
				int textInd = mater->baseColorTexture;
				Vector4D color = vector4d(mater->baseColorFactor[0], mater->baseColorFactor[1], mater->baseColorFactor[2], mater->baseColorFactor[3]);
				Matrix4 mat = { 0 };

				gfc_matrix_multiply(mat, node->mat, modelMat);

				gf3d_model_multimesh_update_basic_model_descriptor_set(model, textInd, *descriptorSet, bufferFrame, mat, vector4d(1, 1, 1, 1), ambientLight);
				gf3d_multimesh_render(model->mesh, meshIndex, commandBuffer, descriptorSet);

				//emissive
				textInd = mater->emissiveTexture;
				gf3d_model_multimesh_update_basic_model_descriptor_set(model, textInd, *descriptorSet, bufferFrame, mat, colorMod, ambientLight);
				gf3d_multimesh_render(model->mesh, meshIndex, commandBuffer, descriptorSet);
			}
		}
	}
}

void baseRender()
{

}

void metalRender()
{

}

void normOccEmiRender()
{

}


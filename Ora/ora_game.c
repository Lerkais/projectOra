#pragma once

//charenemy,buttons,time
//Visual studio implementation

#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_color.h"
#include "gfc_audio.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"


#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"

#include "charenemy.h"

#include "special_attack.h"

#include "oratime.h"


#include "gfc_list.h"

#include "button.h"
#include "sceneManager.h"

#include "collision.h"
#include "ray.h"
#include "Pbdfluid.h"
#include "oraGLTF.h"

#define CHARENEMY_FILEPATH "\\Ora\\data\\charenemyData\.json"
#define WEAPON_FILEPATH "\\Ora\\data\\weaponData\.json"
#define WEAPONSAVE_FILEPATH "\\Ora\\data\\weaponSave\.json"
#define MUSIC_FILEPATH "\\Ora\\data\\sound\\"


char path[256];

bool powers[6];

bool Initialized = false, DemoMode = false, clicked = false, camCanMove = true, showUI = true;
char* ImageDir; //Need to be init
Charenemy** charas;
char CWD[256];
SDL_Event ev;

Object* world;

typedef struct GameData_S
{
	int done,a;
	Sprite* mouse;
	int mousex, mousey;
	float mouseFrame;
	Uint8* keys;
	Object** charen;
	Object* camobj;
	SceneManager* sm;
	int w, h; //canvas sizes
	float xleftbound, xrightbound;
	unsigned char weapons;
	Mix_Music* bgmusic;
}GameData;
GameData gd;


int game();
void* menu();
void blipStart();
void blipEnd();
void events();
int interupts();
void explore();
void moveCamera();
void exploreMovePlayers(Object* player);
Vector2D** oraAlign(int rows, int cols, float w, float h);
void battleSelection(Object* enemy);
void battle(Object* a, Object* b);
void updateCamera(bool a);
void weaponCollection();

//todo wrap into another file/structure
//player interaction bools
bool used1 = false;
bool used2 = false;
float used2Duration = 10;
bool used4 = false;
float used4Duration = 5;
bool used5 = false;
float used5CDMax = 1;
float used5cd = 1;
bool used6 = false;
float used6cd = 1;
float used6CDMax = 1;
float demomodecd = 1;

void events()
{
	SDL_PollEvent(&ev);
	if (ev.type == SDL_MOUSEBUTTONDOWN)
	{
		clicked = true;
	}
	else if (ev.type == SDL_MOUSEBUTTONUP)
	{
		clicked = false;
	}
}


void mouseBlip()
{
	gd.mouseFrame += .01;
	if (gd.mouseFrame >= 16) gd.mouseFrame = 0;
	SDL_GetMouseState(&gd.mousex, &gd.mousey);
	gf2d_sprite_draw(gd.mouse, vector2d(gd.mousex, gd.mousey), vector2d(1, 1), vector3d(8, 8, 0), gfc_color(0.3, .9, 1, 0.9), (Uint32)gd.mouseFrame);
}

void initGame() 
{
	//init file path
	CWD[0] = '\0';
	getcwd(CWD, 256);
	unsigned char size = strlen(CWD);
	CWD[size - 5] = '\0';
	strcpy(path, CWD);
	char* cpath = strncat(path, CHARENEMY_FILEPATH, 37);
	slog(cpath);
	gd.w = 1000;
	gd.h = 500;

	powers[0] = true;
	powers[1] = true;

	//Init Keys enum
	gd.keys = SDL_GetKeyboardState(NULL);

	//Init Charenemys from path 
	//FILE* f = fopen(path, "r");
	charas = populateCharacters(cpath);
	DEBUGprintCharacters();

	//init weapons
	char wpath[256],wspath[256];
	strcpy(wpath, CWD);
	strncat(wpath, WEAPON_FILEPATH, 29);
	initWeapons(wpath);
	gd.weapons = 0b00000; 

	strcpy(wspath, CWD);
	strncat(wspath, WEAPONSAVE_FILEPATH, 29);
	gd.weapons = readWeapons(wspath);


	//init subsystems
	init_logger("gf3d.log", 0);
	gfc_input_init("config/input.cfg");
	slog("gf3d begin");
	gf3d_vgraphics_init("config/setup.cfg");
	gf2d_font_init("config/font.cfg");
	gf2d_draw_manager_init(10000);
	slog_sync();


	//init audio subsytem
	Uint32 maxSounds = 10;
	Uint32 channels = 2;
	Uint32 channelGroups = 2;
	Uint32 maxMusic = 3;
	bool enableMP3 = true;
	bool enableOgg = false;
	gfc_audio_init(maxSounds, channels, channelGroups, maxMusic, enableMP3, enableOgg);

	char mpath[256];
	strcpy(mpath, CWD);
	strncat(mpath, MUSIC_FILEPATH, 29);

	char tmpath[256];

	strcpy(tmpath, mpath);
	strncat(tmpath, "\\01 Laxcity - Elysian\.wav", 26);
	
	gd.bgmusic = Mix_LoadMUS(tmpath); //todo make music.h

	//Mouse sprite
	gd.mouse = gf2d_sprite_load("images/pointer.png", 32, 32, 16);


	// Enviornment
	world = NewObject();
	world->model = loadModelfromgltf("C:\\Users\\funru\\source\\repos\\projectOra\\gf3d\\models\\player\\world\.gltf");
	//world->model = gf3d_model_load_full("C:\\Users\\funru\\source\\repos\\projectOra\\gf3d\\models\\player\\world\.obj", "images/default.png");
	world->draw = gltf_model_draw;

	//init Charenemies
	// index 0 to 2 are player's, 3 to 8 are enemies
	gd.charen = calloc(9,sizeof(Object*));
	if (gd.charen == NULL)exit(546);
	for (int i = 0; i < 9; i++)
	{
		//set model filepath per character
		char buff[32];
		char filepath[100];
		gd.charen[i] = NewObject();
		char* str = "models/c";
		char* str2 = _itoa(i, buff, 10);
		char* str3 = ".model";
		strcpy(filepath, str);
		strcat(filepath, str2);
		strcat(filepath, str3);
		gd.charen[i]->model = gf3d_model_load(filepath);

		//set position 
		gd.charen[i]->position.x = (100 * i);
		
		gd.charen[i]->Type = charenemy;
		
		ObjectData cobj;
		cobj.charenemyObj.charenemy = charas[i]; //todo shuffle master list to randomize players AFTER INITIALIZING      
		gd.charen[i]->Data = cobj;

		vector3d_scale(gd.charen[i]->scale, gd.charen[i]->scale, .2f);

	}

	

	//init bullet models
	initAttacks();


	//init scenemanager
	InitSceneManager();
	gd.sm = GetSceneManager();


	//init camera
	Vector3D pos, rot;
	gd.camobj = NewObject();
	gd.camobj->position.z = 0;
	gd.camobj->position.y = -100;
	vector3d_copy(pos, gd.camobj->position);
	vector3d_copy(rot, gd.camobj->rotation);
	gf3d_camera_set_position(pos);
	gf3d_camera_set_rotation(rot);

	


	//init buttons
	__mouseClickedButton = &clicked;
	__mousexButton = &gd.mousex;
	__mouseyButton = &gd.mousey;

	//check invers matrix function
	float a[4] = { 1, 1, 1, 0 };
	float b[4] = {0, 3, 1, 2};
	float c[4] = {1, 0, 2, 1};
	float d[4] = {2, 3, 1, 0};

	Matrix4 test4;
	memcpy(test4[0], a, sizeof(a));
	memcpy(test4[1], b, sizeof(a));
	memcpy(test4[2], c, sizeof(a));
	memcpy(test4[3], d, sizeof(a));



	Initialized = true;
}



int game()
{
	bool cont = true;
	initGame();

	//gltf testing

	//Model* m10 = loadModelfromgltf("C:\\Users\\funru\\source\\repos\\projectOra\\gf3d\\models\\blackhole\\blackhole\.gltf");
	//Model* m10 = loadModelfromgltf("C:\\Users\\funru\\source\\repos\\projectOra\\gf3d\\models\\player\\player\.gltf");
	//gd.charen[0]->model = m10;
	//gd.charen[0]->draw = gltf_model_draw;
	//gd.charen[0]->scale = vector3d(.5, .5, .6);

	Mix_PlayMusic(gd.bgmusic,2);
	Mix_VolumeMusic(64);

	//menu();

	

	do 
	{
		
		blipStart();
		
		events();

		char buff[100];
		gf2d_font_draw_line_tag(_itoa(gd.mousex, buff, 10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(10,10));
		gf2d_font_draw_line_tag(_itoa(gd.mousey, buff, 10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(40,10));


		blipEnd();

		void* funct = menu();
		if (funct == exit)
		{
			exit(12);
		}
		else if (funct == explore)
		{
			explore();
		}
		else if (funct == weaponCollection)
		{
			weaponCollection();
		}
		

	} while (cont);


	return 0;
}

//Main Menu Scene
void* menu()
{
	if (!Initialized) return;
	//button init
	Button* startButton = newButton(vector4d(20, 20, 100, 120), "Start Button");
	Button* quitButton = newButton(vector4d(120, 120, 100, 120), "Quit Button");
	Button* ownedWeapons = newButton(vector4d(220, 220, 100, 120), "Owned Weapons");

	//Menu Loop
	while (true)
	{
		void* ret = NULL;
		blipStart();

		events();

		if (buttonBlip(*startButton))
		{
			ret = explore;
		}
		if (buttonBlip(*quitButton))
		{
			ret = exit;
		}
		if (buttonBlip(*ownedWeapons))
		{
			ret = weaponCollection;
		}
		blipEnd();
		if(ret) return ret;
	}

}

void weaponCollection()
{
	while (true) {
		blipStart();
		events();
		for (int i = 0; i < 5; i++)
		{
			char* name = getWeaponNameById(i);
			if (gd.weapons & (0b1 << i))
			{
				gf2d_draw_rect_filled(gfc_rect(10, 52 + 45 * i, 200, 40), gfc_color8(128, 128, 10, 255));
				gf2d_font_draw_line_tag(name, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52 + 45 * i));

			}
			else
			{
				gf2d_draw_rect_filled(gfc_rect(10, 52 + 45 * i, 200, 40), gfc_color8(50, 50, 50, 255));
				gf2d_font_draw_line_tag("???", FT_H1, gfc_color(.7, .7, .7, 1), vector2d(10, 52 + 45 * i));
			}

			

		}
		blipEnd();


		if (clicked)
		{
			clicked = false;
			break;
		}

	}
}

/// <summary>
/// Sets the positions of given objects to spawn locations in an exploration setting, NEEDS ARRAY OF 9
/// </summary>
void exploreSpawnLocations(Vector3D* postions[9])
{
	vector3d_copy((*postions[0]),vector3d(0, 0, 0));
	vector3d_copy((*postions[1]), vector3d(10, 0, 0));
	vector3d_copy((*postions[2]), vector3d(20, 0, 0));
	for (int i = 3; i < 9; i++)
	{
		float offset = (rand() % 10 + 3) * 3 + 0;
		float last = postions[i-1]->x;
		vector3d_copy((*postions[i]), vector3d(last + offset, 0, 0));
	}
	//init gameworld bounds
	gd.xleftbound = -10;
	gd.xrightbound = gd.charen[8]->position.x + 10;
}

int exploreUpdateDead(Object* chare)
{
	if (chare->Type != charenemy)
		return 0;
	else if (chare->Data.charenemyObj.charenemy->isDead)
	{
		chare->isActive = false;
		return 1;
	}

}

void explore()
{
	//explore init
	Scene* exploreSceneInstance = NewScene(explorationscene);
	AddSceneToManager(exploreSceneInstance);
	Vector3D* positions[9];
	Model* sky = gf3d_model_load("models/sky.model");
	Matrix4 skyMat;
	gfc_matrix_identity(skyMat);
	gfc_matrix_scale(skyMat, vector3d(100, 100, 100));

	const float COOLDOWNMAXEXPLORE = 1.5f;
	float cdex = COOLDOWNMAXEXPLORE;

	for (int i = 0; i < 9; i++) 
	{
		AddSceneObject(exploreSceneInstance, gd.charen[i]);
		positions[i] = &(gd.charen[i]->position);
	}

	AddSceneObject(exploreSceneInstance, world);

	exploreSpawnLocations(positions);

	LoadScene(exploreSceneInstance);

	Object* playerChars[3];
	playerChars[0] = gd.charen[0];
	playerChars[1] = gd.charen[1];
	playerChars[2] = gd.charen[2];


	//fluid
	ParticleSpawner* s = newpSpawner(10, vector3d(0, 0, 15));
	s->color = GFC_COLOR_CYAN;



	oratimeinit();
	while (true)
	{
		tick();//can oly be called once a frame, or time will distort
		UpdateObjects(exploreSceneInstance);
		events();
		particleSpawnerStep(s);


		for (int i = 0; i < 3; i++)
		{
			if (!gd.charen[i]->Data.charenemyObj.charenemy->isDead)
			{
				Object* objA = gd.charen[i];
				if (objA->Data.charenemyObj.charenemy->isDead)
					continue;
				for (int j = 0; j < gd.sm->activeScene->objectCount; j++)
				{
					Object* otherobj = gd.sm->activeScene->objects[j];
					if (otherobj->Type == charenemy && otherobj != gd.charen[i] && !otherobj->Data.charenemyObj.charenemy->isDead)
					{
						if (isColliding(objA, otherobj) && cdex < 0)
						{
							//battle selection
							printf("Collision detected: %s,%s\n", objA->Data.charenemyObj.charenemy->name, otherobj->Data.charenemyObj.charenemy->name);

							battleSelection(otherobj);

							cdex = COOLDOWNMAXEXPLORE;


							//LoadScene(exploreSceneInstance);
						}	
						cdex -= deltaTime;
					}
				}
			}
		}

		blipStart();

		for (int i = 0; i < 3; i++) exploreMovePlayers(playerChars[i]), exploreUpdateDead(gd.charen[i]);
		int wincount = 0;
		for (int i = 3; i < 9; i++) wincount += exploreUpdateDead(gd.charen[i]);

		if (wincount == 6)
		{
			//win function
			slog("youwon");
		}

		DrawSceneObjects(exploreSceneInstance);
		drawOraParticles(s);

		//gf3d_model_draw_sky(sky, skyMat, gfc_color(1, 1, 1, 1));

		
		moveCamera();

		updateCamera(true);

		for (int i = 0; i < 9; i++) {
			//gd.charen[i]->rotation.z += .1 * deltaTime;
		}

		

		blipEnd();
		//UniformBufferObject ubo = gf3d_vgraphics_get_uniform_buffer_object();
		//gfc_matrix3_slog(ubo.proj);


		int alive = 6;

		for (int j = 3; j < 9; j++)
		{
			if (gd.charen[j]->Data.charenemyObj.charenemy->isDead)
				alive--;
		}
		if (alive == 0)
		{
			unsigned char a = rand() % 100;
			if (a > 0) //for demo purpases it is 100% chance;
			{
				gd.weapons = gd.weapons | (1 << ((int)(rand() % 6)));
				printf("%i",gd.weapons);
			}
			saveWeapons(WEAPONSAVE_FILEPATH, gd.weapons);
			break;
		}

	}
}

void updateCamera(bool lookAt)
{	
	Vector3D pos, rot;
	vector3d_copy(rot, gd.camobj->rotation);
	vector3d_copy(pos, gd.camobj->position);
	Vector3D forward = vector3d(0,1,0);
	vector3d_add(forward, forward, pos);
	if (lookAt)gf3d_camera_look_at(pos, gd.charen[0]->position, vector3d(0, 0, 1));
	else  gf3d_camera_look_at(pos, forward, vector3d(0, 0, 1));
	//gf3d_camera_update_view();

	gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());
}

/// <summary>
/// Draws a 2D background over the whole screen
/// </summary>
void blipStart()
{
	gf3d_vgraphics_render_start();
}

void blipEnd()
{
	mouseBlip();
	gf3d_vgraphics_render_end();
	gf2d_font_update();
}

/// <summary>
/// Checks for high priority window calls
/// </summary>
int interupts() 
{
	//alt + f4

}

void moveCamera()
{
	
	float speed = 30;

	if (!camCanMove)return;
	if (gd.keys[SDL_SCANCODE_LSHIFT])
		speed = 60;
	else
		speed = 30;

	Vector3D forward = vector3d(0, deltaTime*speed, 0);
	Vector3D right = vector3d(deltaTime*speed, 0, 0);


	if (gd.keys[SDL_SCANCODE_W])
	{
		vector3d_add(gd.camobj->position, gd.camobj->position, forward);
	}
	if (gd.keys[SDL_SCANCODE_S])
	{
		vector3d_add(gd.camobj->position, gd.camobj->position, -forward);
	}
	if (gd.keys[SDL_SCANCODE_D] && gd.camobj->position.x < gd.xrightbound)
	{
		vector3d_add(gd.camobj->position, gd.camobj->position, right);
	}
	else if (gd.keys[SDL_SCANCODE_A] && gd.camobj->position.x > gd.xleftbound)
	{
		vector3d_add(gd.camobj->position, gd.camobj->position, -right);
	}
	if (gd.keys[SDL_SCANCODE_SPACE])gd.camobj->position.z += 1 * deltaTime * speed;
	if (gd.keys[SDL_SCANCODE_LCTRL])gd.camobj->position.z -= 1 * deltaTime * speed;
}

void exploreMovePlayers(Object* player)
{
	float speed = 30;

	if (!camCanMove)return;
	if (gd.keys[SDL_SCANCODE_LSHIFT])
		speed = 60;
	else
		speed = 30;

	Vector3D forward = vector3d(0, deltaTime * speed, 0);
	Vector3D right = vector3d(deltaTime * speed, 0, 0);




	if (gd.keys[SDL_SCANCODE_W])
	{
		//vector3d_add(player->position, player->position, forward);
	}
	if (gd.keys[SDL_SCANCODE_S])
	{
		//vector3d_add(player->position, player->position, -forward);
	}
	if (gd.keys[SDL_SCANCODE_D] && gd.charen[2]->position.x < gd.xrightbound)
	{
		vector3d_add(player->position, player->position, right);
	}
	else if (gd.keys[SDL_SCANCODE_A] && gd.charen[0]->position.x > gd.xleftbound)
	{
		vector3d_add(player->position, player->position, -right);
	}
	//if (gd.keys[SDL_SCANCODE_SPACE])player->position.z += 1 * deltaTime * speed;
	//if (gd.keys[SDL_SCANCODE_LCTRL])player->position.z -= 1 * deltaTime * speed;
}

int testing(Matrix4 viewMatrix, Matrix4 projectionMatrix) {
	// Example input values
	Vector3D screenPoint = { gd.mousex,gd.mousey , 10.0f };  // Example screen coordinates (x, y, depth)
	Vector2D viewportSize = { 1000.0f, 500.0f };  // Example viewport size (width, height)

	//  unproject 
	Vector3D worldPoint = gfc_unproject(screenPoint, viewMatrix, projectionMatrix, viewportSize);


	
	//printf("World Coordinates: (%.2f, %.2f, %.2f)\n", worldPoint.x, worldPoint.y, worldPoint.z);

	return 0;
}

void battleSelection(Object* enemy)
{
	//todo unload and save previous scene

	Scene* prevScene = UnloadActiveScene();

	//init 
	int w = 1000, h = 500;
	SDL_Window* wind = SDL_GL_GetCurrentWindow();
	//SDL_GetWindowSizeInPixels(wind, &w, &h);
	Vector2D** uiPos = oraAlign(3, 3, w, h);
	
	Button* buttC1 = newButton(vector4d(uiPos[1][0].x-50, uiPos[1][0].y-25, 50, 100), gd.charen[0]->Data.charenemyObj.charenemy->name);
	Button* buttC2 = newButton(vector4d(uiPos[1][1].x-50, uiPos[1][1].y-25, 50, 100), gd.charen[1]->Data.charenemyObj.charenemy->name);
	Button* buttC3 = newButton(vector4d(uiPos[1][2].x-50, uiPos[1][2].y-25, 50, 100), gd.charen[2]->Data.charenemyObj.charenemy->name);
	Button* buttQt = newButton(vector4d(uiPos[2][1].x - 50, uiPos[2][1].y - 25, 50, 100), "Quit");

	Object* batt1 = NewObject();
	Object* batt2 = NewObject();
	memcpy(batt2,enemy,sizeof(Object));

	//3d render init
	Object* charm1 = NewObject();
	Object* charm2 = NewObject();
	Object* charm3 = NewObject();
	memcpy(charm1, gd.charen[0], sizeof(Object));
	memcpy(charm2, gd.charen[1], sizeof(Object));
	memcpy(charm3, gd.charen[2], sizeof(Object));

	Scene* battleSelect = NewScene(otherscene);
	AddSceneObject(battleSelect, charm1);
	AddSceneObject(battleSelect, charm2);
	AddSceneObject(battleSelect, charm3);

	

	//camera init
	Vector3D camold = gd.camobj->position;
	gd.camobj->position = vector3d(0, -50, 0);
	updateCamera(false);

	
	//char position init
	UniformBufferObject ubo = gf3d_vgraphics_get_uniform_buffer_object();
	Matrix4 vm;
	float depth = 5;
	gf3d_camera_get_view_mat4(&vm);	

	//gfc_matrix_multiply(vm, ubo.model, vm);

	Vector3D t1, t2, t3,d1,d2,d3;


	LoadScene(battleSelect);

	d1 = gfc_unproject(vector3d(uiPos[1][0].x, uiPos[1][0].y-50, 0),
		vm,
		ubo.proj,
		gf3d_vgraphics_get_view_extent_as_vector2d()
	);

	d2 = gfc_unproject(vector3d(uiPos[1][1].x, uiPos[1][1].y-50, 0),
		vm,
		ubo.proj,
		gf3d_vgraphics_get_view_extent_as_vector2d()
	);

	d3 = gfc_unproject(vector3d(uiPos[1][2].x,uiPos[1][2].y-50,0),
		vm,
		ubo.proj,
		gf3d_vgraphics_get_view_extent_as_vector2d()
	);


	charm1->position = unprojToPos(gd.camobj->position, d1, depth);;
	charm2->position = unprojToPos(gd.camobj->position, d2, depth);;
	charm3->position = unprojToPos(gd.camobj->position, d3, depth);;


	bool end = false;
	//loop
	BattleSelectLabel:
	while (!end)
	{
		
		//gd.camobj->position.x += .1 * deltaTime;

		//charm1->rotation.z += 1 * deltaTime;
		//charm2->rotation.z += 1 * deltaTime;
		//charm3->rotation.z += 1 * deltaTime;

		

		blipStart();
		events();
		UpdateObjects(battleSelect);

		if (buttonBlip(*buttC1) && !charm1->Data.charenemyObj.charenemy->isDead)
		{
			memcpy(batt1, charm1, sizeof(Object));
			end = true;
		}
		else if (buttonBlip(*buttC2) && !charm2->Data.charenemyObj.charenemy->isDead)
		{
			memcpy(batt1, charm2, sizeof(Object));
			end = true;
		}
		else if (buttonBlip(*buttC3) && !charm3->Data.charenemyObj.charenemy->isDead)
		{
			memcpy(batt1, charm3, sizeof(Object));
			end = true;
		}

		DrawSceneObjects(battleSelect);

		//moveCamera();
		updateCamera(false);

		blipEnd();
		if (end)
		{
			battle(batt1, batt2);
		}
	}

		

	//cleanup
	free(uiPos);
	UnloadActiveScene();
	FreeScene(battleSelect);
	LoadScene(prevScene);
	gd.camobj->position = camold;
	slog("bs ended");
}

/// <summary>
/// Battle scene
/// </summary>
///<param name = "a">Player's character in battle</param>
///<param name = "b">Enemy character in battle</param>
void battle(Object* a, Object* b)
{
	//testing
	printf("Name of enemy: %s", b->Data.charenemyObj.charenemy->name);


	//todo unloadscene
	//unload previous scene and save it
	Scene* prevScene = UnloadActiveScene();

	//init battle scene
	Scene* battleScene = NewScene(battlescene);
	battleScene->data.battleData.playerChar = a;
	battleScene->data.battleData.enemyChar = b;
	LoadScene(battleScene);
	AddObjectToActiveScene(a);
	AddObjectToActiveScene(b);

	//set objects data
	Object* ab[2] = { a,b };
	a->position = vector3d(-20,0,0);
	b->position = vector3d(20,0,0);


	unsigned char cds;


	//init camera
	gd.camobj->position = vector3d(0, -50, 0);
	updateCamera(false);


	//todo wrap into another file
	

	//future button init here





	//battle scene loop
	while (!a->Data.charenemyObj.charenemy->isDead && !b->Data.charenemyObj.charenemy->isDead) {
		
		a->rotation = vector3d(0, 0, 2.0f);

		powers[0]; // Insta Kill
		powers[1]; // Speed Up Basics
		powers[2] = (gd.sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->cooldownTimers.special <= 0); //Special attack
		powers[3] = a->Data.charenemyObj.charenemy->isDead; // Stop enemy attacks
		powers[4] = true; //hide ui
		powers[5] = DemoMode;


		if (powers[0] && gd.keys[SDL_SCANCODE_1])
		{
			b->Data.charenemyObj.charenemy->isDead = true;
			b->Data.charenemyObj.charenemy->health = 0;
			powers[0] = false;
		}



		bool canAttak = true;
		if (powers[1] && gd.keys[SDL_SCANCODE_2] && used2Duration > 0)
		{
			used2Duration -= deltaTime;
			canAttak = false;
			powers[1] = false;
		}
		else if (used2Duration != 10 && used2Duration > 0)
		{
			used2Duration -= deltaTime;
			canAttak = false;
		}




		for (int i = 0; i < 2; i++)
		{
			//special attack timer change due to power 
			//if(ab[i]->Data.charenemyObj.charenemy->cooldownTimers.special)

			//tick cool downs, cds format 0b000 to 0b111
			cds = tickcds(ab[i]->Data.charenemyObj.charenemy);
			if (i == 1 && !canAttak)
				break;

			if (cds & 0b100)
			{
				Object* projtoadd = basicAttack(ab[i], ab[i ^ 1]);
				if (used2 && used2Duration > 0 && i == 0)
					projtoadd->color = GFC_COLOR_MAGENTA;
				AddObjectToActiveScene(projtoadd);
			}
			if (cds & 0b010)
			{
				Object* projtoadd = heavyAttack(ab[i], ab[i ^ 1]);
				AddObjectToActiveScene(projtoadd);
			}
			if (cds & 0b001)
			{
				//special attack
				if (gd.keys[SDL_SCANCODE_3] || i == 1)
				{
					Object* projtoadd = uniAttack(ab[i], ab[i ^ 1]);
					AddObjectToActiveScene(projtoadd);
					ab[i]->Data.charenemyObj.charenemy->cooldownTimers.special = ab[i]->Data.charenemyObj.charenemy->cooldownMax.special;
				}
			}


		}





		//graphics
		blipStart();
		events();
		UpdateObjects(battleScene);

		//3d
		DrawSceneObjects(battleScene);
		




		if (showUI)
		{
			//Names

			gf2d_draw_rect_filled(gfc_rect(890, 52, 200, 40), gfc_color8(128, 128, 10, 255));
			gf2d_font_draw_line_tag(gd.sm->activeScene->data.battleData.enemyChar->Data.charenemyObj.charenemy->name, FT_H1, gfc_color(1, 1, 1, 1), vector2d(890, 52));

			gf2d_draw_rect_filled(gfc_rect(10, 52, 200, 40), gfc_color8(128, 128, 10, 255));
			gf2d_font_draw_line_tag(gd.sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->name, FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52));


			//health
			char buff[30];

			gf2d_draw_rect_filled(gfc_rect(890, 52 + 25, 200, 34), gfc_color8(128, 128, 10, 255));
			gf2d_font_draw_line_tag(_itoa(gd.sm->activeScene->data.battleData.enemyChar->Data.charenemyObj.charenemy->health, buff, 10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(890, 52 + 25));

			gf2d_draw_rect_filled(gfc_rect(10, 52 + 25, 200, 32), gfc_color8(128, 128, 10, 255));
			gf2d_font_draw_line_tag(_itoa(gd.sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->health, buff, 10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52 + 25));

			//cooldown - player
			gf2d_draw_rect_filled(gfc_rect(10, 52 + 45, 200, 32), gfc_color8(128, 128, 10, 255));
			gf2d_font_draw_line_tag(_itoa(gd.sm->activeScene->data.battleData.playerChar->Data.charenemyObj.charenemy->cooldownTimers.special, buff, 10), FT_H1, gfc_color(1, 1, 1, 1), vector2d(10, 52 + 45));

			//mouse
			updateCamera(false);


			//powers

			for (int j = 0; j < 6; j++)
			{
				if (powers[j])
					gf2d_draw_rect_filled(gfc_rect(10 + 12 * j, 10, 10, 10), gfc_color8(128, 128, 10, 255));
				else
					gf2d_draw_rect_filled(gfc_rect(10 + 12 * j, 10, 10, 10), gfc_color8(128, 20, 10, 255));
			}
		}

		


		blipEnd();

	}

	//cleanup
	UnloadActiveScene();
	FreeScene(battleScene);
	LoadScene(prevScene);
}

Vector2D** oraAlign(int rows, int cols, float w, float h)
{
	Vector2D** ret = (Vector2D**)malloc(rows * sizeof(Vector2D*) + rows * cols * sizeof(Vector2D));
	if (!ret) return;

	int rowsize = cols * sizeof(Vector2D);
	// Set up the row pointers
	for (int i = 0; i < rows; i++) {
		ret[i] = (Vector2D*)((char*)ret + rows * sizeof(Vector2D*) + i * cols * sizeof(Vector2D));
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j].x = (w / cols) * j + (w / cols) / 2;
			ret[i][j].y = (h / rows) * i + (h / rows) / 2;
		}
	}



	return ret;
}

int main(int argc, char* argv[])
{
	game();

	return 0;
}
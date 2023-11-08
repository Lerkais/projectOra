#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "gfc_vector.h"

#include "simple_json.h"

typedef enum ButtonType_E {
	quit,
	loadScene,
	inGame,
	otherbutton
}ButtonType;

typedef struct Button_S {
	char* name;
	unsigned int ID;
	char* text;
	int x;
	int y;
	int w;
	int h;
	ButtonType type;
}Button;

Button** ButtonList;
unsigned char ButtonListSize;

void PopulateButtons(char* filepath) 
{
	SJson* json;
	json = sj_load(filepath);
	SJList* keys = sj_object_get_value(json, "Buttons")->v.array;
	unsigned char c = sj_list_get_count(keys);

	ButtonList = calloc(c, sizeof(Button*));
	ButtonListSize = c;

	for (int i = 0; i < c; i++) 
	{
		Button* refp = malloc(sizeof(Button));
		SJson* idp = sj_list_get_nth(keys, i);
		refp->name = sj_object_get_value_as_string(idp, "name");
		sj_object_get_value_as_int(idp, "x", &(refp->x));
		sj_object_get_value_as_int(idp, "y", &(refp->y));
		sj_object_get_value_as_int(idp, "w", &(refp->w));
		sj_object_get_value_as_int(idp, "h", &(refp->h));
		refp->text = sj_object_get_value_as_string(idp, "text");
		//todo ID
		//todo type
	}
}


#endif // !__BUTTON_H__

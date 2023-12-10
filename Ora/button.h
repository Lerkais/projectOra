#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gfc_input.h"

#include "gfc_vector.h"

#include "simple_json.h"


#define umin(a,b) (((a-b) >= 0) ? (a-b) : (0))

typedef struct Button_S {
	char* name;
	unsigned int ID;
	char* text;
	Rect dim;
	Color color;
}Button;

Button** ButtonList;
unsigned char ButtonListSize;
int *__mousexButton, *__mouseyButton; //HAS TO BE INITIALIZED IN ORA_GAME.C
bool* __mouseClickedButton; //HAS TO BE INITIALIZED IN ORA_GAME.C

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
		if (refp == NULL)return;
		SJson* idp = sj_list_get_nth(keys, i);
		refp->name = sj_object_get_value_as_string(idp, "name");
		sj_object_get_value_as_int(idp, "x", &(refp->dim.x));
		sj_object_get_value_as_int(idp, "y", &(refp->dim.y));
		sj_object_get_value_as_int(idp, "w", &(refp->dim.w));
		sj_object_get_value_as_int(idp, "h", &(refp->dim.h));
		refp->text = sj_object_get_value_as_string(idp, "text");
		//todo ID
		//todo type
	}
}

//Name does NOT need a malloc before calling function | z = height
Button* newButton(Vector4D dimensions, char* name)
{
	Button* refp = malloc(sizeof(Button));
	refp->dim.x = dimensions.x;
	refp->dim.y = dimensions.y;
	refp->dim.w = dimensions.w;
	refp->dim.h = dimensions.z;
	refp->name = malloc(sizeof(name));
	strcpy(refp->name, name);
	return refp;
}

bool buttonHover(Button b)
{

	bool xcond = *__mousexButton > b.dim.x && *__mousexButton < b.dim.w+ b.dim.x;
	bool ycond = *__mouseyButton > b.dim.y && *__mouseyButton < b.dim.h+ b.dim.y;

	return xcond && ycond;
}

bool buttonBlip(Button b) 
{
	int d = 50;
	Rect dim = b.dim;
	dim.h *= 2;
	dim.w *= 2;

	bool ret = false;

	if (buttonHover(b))
	{
		if (*__mouseClickedButton)
		{
			gf2d_draw_rect_filled(dim, gfc_color8(umin(128, 2 * d), umin(128, 2 * d), umin(10, 2 * d), 255));
			gf2d_font_draw_line_tag(b.name, FT_H1, gfc_color8(umin(255,2*d), umin(255, 2*d), umin(255, 2*d), 255), vector2d(dim.x + 7, dim.y + dim.h / 4 - 7));
			*__mouseClickedButton = false;
			ret = true;
		}
		else
		{
			gf2d_draw_rect_filled(dim, gfc_color8(umin(128, d), umin(128, d), umin(10, d), 255));
			gf2d_font_draw_line_tag(b.name, FT_H1, gfc_color8(umin(255, d), umin(255, d), umin(255, d), 255), vector2d(dim.x + 7, dim.y + dim.h / 4 - 7));
		}

	}
	else
	{
		gf2d_draw_rect_filled(dim, gfc_color8(128, 128, 10, 255));
		gf2d_font_draw_line_tag(b.name, FT_H1, gfc_color8(255, 255, 255, 255), vector2d(dim.x + 7, dim.y + dim.h / 4 - 7));
	}


	return ret;

}



#endif // !__BUTTON_H__

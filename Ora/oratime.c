#include "oratime.h"

void oratimeinit() 
{
	deltaTime = .1; //arbittrary, must be non zero
	oratime = clock();
}

void tick()
{

	deltaTime = ((double)(clock() - oratime))/CLOCKS_PER_SEC;
	oratime = clock();
}



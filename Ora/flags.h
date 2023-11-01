#ifndef _FLAGS_H_
#define _FLAGS_H_

#include <stdbool.h>
#include <SDL_stdinc.h>

typedef struct FlagContainer_S {
	unsigned long flagData;
}FlagContainer;

void setFlags(FlagContainer* flagContainer,unsigned char flags[],unsigned char size);

void getFlags(FlagContainer* flagContainer,unsigned char returnArray[]);

bool checkFlag(FlagContainer* flagContainer,unsigned char flag);



#endif
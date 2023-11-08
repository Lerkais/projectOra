#include "flags.h"

void setFlags(FlagContainer* fc, unsigned char flags[], unsigned char size)
{
	for (int i = 0; i < size; i++) 
	{
		fc->flagData^flags[i];
	}
}

void getFlags(FlagContainer* fc,unsigned char returnArray[])
{
	unsigned long reta = fc->flagData;
	unsigned char index = 0;
	for (unsigned char count = 0; count < 64; count++)
	{
		if (count & 0b1) 
		{
			returnArray[index] = count;
			index++;
		}
	}

}

bool checkFlag(FlagContainer* fc,unsigned char flag)
{
	return flag&fc->flagData;
}

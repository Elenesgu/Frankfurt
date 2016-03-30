#include "FRABitmap.h"
#include "FRAImageProcedure.h"
#include<stdlib.h>
#include<stdio.h>

int main() {
	FRARawImage* input;
	int ret = FRAOpenBitmapFile("../input/p.bmp", &input);
	printf("%d\n", ret);
	FRAInvertImage(input);
	FRARemoveHaze(input);
	//FRAInvertImage(input);
	FRASaveBitmapFile("pi_out.bmp", &input);
	return 0;
}
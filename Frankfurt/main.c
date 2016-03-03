#include "FRABitmap.h"
#include "FRAImageProcedure.h"
#include<stdlib.h>
#include<stdio.h>

int main() {
	FRARawImage* input;
	int ret = FRAOpenBitmapFile("test.bmp", &input);
	printf("%d\n", ret);
	FRAInvertImage(input);
	FRASaveBitmapFile("test_out.bmp", &input);
	return 0;
}
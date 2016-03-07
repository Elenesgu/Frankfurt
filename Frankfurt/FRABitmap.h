#pragma once

#pragma pack(push,1)
typedef struct {
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
} FRABitmapFileHeader;

//Windows V3 Header
//BITMAPINFOHEADER
typedef struct {
	unsigned int    biSize;
	int             biWidth;
	int             biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompression;
	unsigned int    biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	unsigned int    biClrUsed;
	unsigned int    biClrImportant;
} FRABitampInfoHeader;
#pragma pack(pop)



typedef struct {
	int width;
	int height;
	int bytesPerPixel;
	unsigned char* bits;
} FRARawImage;

int FRAOpenBitmapFile(const char* filename, FRARawImage** result);
int FRASaveBitmapFile(const char* filename, FRARawImage** input);



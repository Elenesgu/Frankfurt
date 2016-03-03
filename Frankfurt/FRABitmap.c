#include"FRABitmap.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

// result is allocated by this function.
int FRAOpenBitmapFile(const char* filename, FRARawImage** result) {
	*result = (FRARawImage*)malloc(sizeof(FRARawImage));
	if (*result == NULL) {
		return FRABIT_MEM_ALLOC_ERROR;
	}
	(*result)->bits = NULL;

	if (filename == NULL || filename[0] == '\0') {
		return FRABIT_FILE_ERROR;
	}

	FILE* filePtr;
	filePtr = fopen(filename, "rb");

	if (filePtr == NULL) {
		fclose(filePtr);
		return FRABIT_FILE_ERROR;
	}

	FRABitmapFileHeader fileHeader;
	fread(&fileHeader, sizeof(fileHeader), 1, filePtr);

	//Check if file is bitmap or not.
	if (fileHeader.bfType != ('B' | ((int)'M' << 8))) {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	FRABitampInfoHeader infoHeader;
	fread(&infoHeader, sizeof(infoHeader), 1, filePtr);
	int width, height;
	int bitsPerPixel, bytesPerPixel;
	int pitch;
	int dataSize;
	if (infoHeader.biSize == 40) {
		//Window V3 header

		//Check header size is equal to size of read data.
		const int frontOfDataPos = ftell(filePtr);
		if (frontOfDataPos != (int)(fileHeader.bfOffBits)) {
			fclose(filePtr);
			return FRABIT_NOT_SUPPORTED_FORMAT;
		}

		width = infoHeader.biWidth;
		height = infoHeader.biHeight;
		bitsPerPixel = infoHeader.biBitCount;
		bytesPerPixel = bitsPerPixel / 8;
		pitch = (width * bytesPerPixel + 3) & ~3;
		dataSize = pitch * height;
		
	}
	else {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	(*result)->width = width;
	(*result)->height = height;
	(*result)->bytesPerPixel = bytesPerPixel;
	(*result)->bits = (char*)malloc(width * height * bytesPerPixel);

	if ((*result)->bits == NULL) {
		fclose(filePtr);
		return FRABIT_MEM_ALLOC_ERROR;
	}

	if (infoHeader.biCompression == 0
		&& bitsPerPixel == 24
		&& ((int)infoHeader.biSizeImage == dataSize || (int)infoHeader.biSizeImage == 0)) {
		//Read 24bit and no compression bitmap
		const int destPitch = width * bytesPerPixel;
		const int gap = pitch - destPitch;
		char* curDest = (*result)->bits + height * destPitch;
		for (int y = height; y > 0; y--) {
			curDest -= destPitch;
			fread(curDest, destPitch, 1, filePtr);
			if (gap > 0) {
				fseek(filePtr, gap, SEEK_CUR);
			}
			for (int x = 0; x < width; x++) {
				char temp = curDest[x * 3];
				curDest[x * 3] = curDest[x * 3 + 2];
				curDest[x * 3 + 2] = temp;
			}
		}
	}
	else {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}

	//Check if there no data remain.
	const int expectedEnd =
		fileHeader.bfSize;
	if (expectedEnd == ftell(filePtr)) {
		fclose(filePtr);
		return FRABIT_SUCCESS;
	}
	else {
		fclose(filePtr);
		return FRABIT_NOT_SUPPORTED_FORMAT;
	}
}
int FRASaveBitmapFile(const char* filename, FRARawImage** input) {
	if (input == NULL || (*input) == NULL) {
		return FRABIT_MEM_NULL_ERROR;
	}

	if (filename == NULL || filename[0] == '\0') {
		return FRABIT_FILE_ERROR;
	}

	FILE* filePtr;
	filePtr = fopen(filename, "wb");

	if (filePtr == NULL) {
		fclose(filePtr);
		return FRABIT_FILE_ERROR;
	}
	const int pitch = ((*input)->width * (*input)->bytesPerPixel + 3) & ~3;
	const int height = (*input)->height;
	const int width = (*input)->width;

	FRABitmapFileHeader fileHeader;
	fileHeader.bfType = ('B' | ((int)'M' << 8));
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(FRABitmapFileHeader) + sizeof(FRABitampInfoHeader);
	int dataSize = pitch * height;
	fileHeader.bfSize = dataSize + fileHeader.bfOffBits;

	fwrite(&fileHeader, sizeof(fileHeader), 1, filePtr);

	FRABitampInfoHeader infoHeader;
	infoHeader.biSize = sizeof(FRABitampInfoHeader);
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = (*input)->bytesPerPixel * 8;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	fwrite(&infoHeader, sizeof(infoHeader), 1, filePtr);

	const int destPitch = width * (*input)->bytesPerPixel;
	const int gap = pitch - destPitch;
	char* curDest = (*input)->bits + height * destPitch;

	char* buffer;
	char* pad;
	buffer = (char*)malloc(destPitch);
	pad = (char*)malloc(gap);
	memset(pad, 0, gap);
	for (int y = height; y > 0; y--) {
		curDest -= destPitch;
		memcpy(buffer, curDest, destPitch);
		for (int x = 0; x < width; x++) {
			char tmp = buffer[x * 3];
			buffer[x * 3] = buffer[x * 3 + 2];
			buffer[x * 3 + 2] = tmp;
		}
		fwrite(buffer, destPitch, 1, filePtr);
		fwrite(pad, gap, 1, filePtr);
	}
	fclose(filePtr);
	free(buffer);
	free(pad);
	return FRABIT_SUCCESS;
}
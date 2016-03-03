#include "FRAImageProcedure.h"
#define _GPU_PARALLEL
//#define _CPU_PARALLEL

//Header files should be located in exact ifdef area.
#ifdef _CPU_ONLY

#endif

#ifdef _CPU_PARALLEL
#include <omp.h>
#endif

#ifdef _GPU_PARALLEL

#endif

int FRAInvertImage(FRARawImage* input) {
	//Invert image with no parallelization.
#ifdef _CPU_ONLY
	int maxBitsNum = input->height * input->width * 3;
	for (int x = 0; x < maxBitsNum; x++) {
		input->bits[x] = 255 - input->bits[x];
	}
#endif

	//Yeongseon Na
#ifdef _CPU_PARALLEL
	int maxBitsNum = input->height * input->width * 3;
	#pragma omp parallel for
	for( int x = 0; x < maxBitsNum; x++)
	{
		input->bits[x] = 255 - input->bits[x];
	}
#endif

	//Kangsan Kim
#ifdef _GPU_PARALLEL

#endif
}

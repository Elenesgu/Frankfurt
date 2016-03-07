#include "FRAImageProcedure.h"
#include "FRAEnum.h"
#include <stdio.h>
#include <stdlib.h>
#define _GPU_PARALLEL
//#define _CPU_PARALLEL

//Header files should be located in exact ifdef area.
#ifdef _CPU_ONLY

#endif

#ifdef _CPU_PARALLEL
#include <omp.h>
#endif

#ifdef _GPU_PARALLEL

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL\cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
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
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	cl_mem memobj;

	
	size_t maxBitsNum = input->height * input->width * 3;

	FILE *fp;
	char fileName[] = "./FRAInvertImage.cl";
	char *source_str;
	size_t source_size;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		return FRA_NO_CL_KERNEL_FILE;
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/* Get Platform and Device Info */
	if (clGetPlatformIDs(1, &platform_id,
		&ret_num_platforms) != CL_SUCCESS)	{
		return FRA_NO_CL_PLATORM;
	}

	if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT,
		1, &device_id, &ret_num_devices) != CL_SUCCESS){
		return FRA_CL_DEVICE_ERROR;
	}

	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,
		maxBitsNum * sizeof(unsigned char), NULL, &ret);

	clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0,
		maxBitsNum * sizeof(unsigned char), input->bits, 0, NULL, NULL);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, 
		(const char **)&source_str, (const size_t *)&source_size, &ret);

	/* Build Kernel Program */
	if (clBuildProgram(program, 1, &device_id,
		NULL, NULL, NULL) != CL_SUCCESS) {
		return FRA_CL_PROGRAM_BUILD_ERROR;
	}

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "FRAInvertImage", &ret);

	/* Set OpenCL Kernel Parameters */
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);

	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		&maxBitsNum, NULL, 0, NULL, NULL);
	clFinish(command_queue);

	/* Copy results from the memory buffer */
	clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
		maxBitsNum * sizeof(char), input->bits, 0, NULL, NULL);

	/* Display Result */

	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);
#endif

	return FRA_SUCCESS;
}

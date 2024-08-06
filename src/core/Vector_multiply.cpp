#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "Vector_func.h"
#include "file.h"
#include "time_chrono.h"
#include <functional>

using namespace std;

TimeKeeper timedog;

void arrayMultiplay(int array_size = 1000) {
	timedog.Init();
	cl_int err;
	//build array A,B,  result container C
	int lenA = array_size;
	int lenB = array_size;
	int lenC = array_size * 2 + 1;
	vector<int> A(lenA);
	vector<int> B(lenB);
	vector<int> C(lenC);

	for (int i = 0; i < lenA; ++i) {
		A[i] = static_cast<int>(i % 10);
	}
	for (int i = 0; i < lenB; ++i) {
		B[i] = static_cast<int>(i % 10);
	}
	//Get platform and device
	cl_platform_id platform,plats[2];
	cl_device_id device;
	clGetPlatformIDs(2, plats, NULL);
	platform = plats[1];
	clGetDeviceIDs(
		platform, CL_DEVICE_TYPE_GPU,
		1, &device, NULL);
	char device_name[128];
	err = clGetDeviceInfo(device, CL_DEVICE_NAME,
		sizeof(device_name), device_name,
		NULL);
	printf("%s\n", device_name);
	//Create context
	cl_context context = clCreateContext(
		NULL, 1, &device,
		NULL, NULL, &err);
	/*if (err != CL_SUCCESS) {
		cerr << "Error in creating context:" << err << endl;
		return;
	}*/

	//Create and Build command queue
	cl_command_queue_properties properties = 0;
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, &properties, &err);
	if (err != CL_SUCCESS) {
		cerr << "Error in creating Command Queue:" << err << endl;
		return;
	}

	cl_mem buffer_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * lenA, A.data(), &err);
	cl_mem buffer_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * lenB, B.data(), &err);
	cl_mem A_length = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &lenA, &err);
	cl_mem B_length = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &lenB, &err);
	cl_mem buffer_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * (lenC), NULL, &err);

	if (err != CL_SUCCESS) {
		cerr << "Error in creating Command Queue:" << err << endl;
		return;
	}

	//build kernel Source
	const char* kernelSource = R"(
		__kernel void multiply_arrays(
			__global const int* A,
			__global const int* B,
			__global const int* len_A,
			__global const int* len_B,
			__global       int* C)
			{
				__local int Alen;
				__local int Blen;
				Alen = *len_A;
				Blen = *len_B;
				int gid = get_global_id(0);
				if (gid >= Alen){
					return ;
				}
				for (int i=0;i < Blen; ++i){
					atomic_add(&C[gid+i],A[gid]*B[i]);
				}
			}
	)";
	cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
	
	/*char* Program_Source;
	char file[] = "multiply_kernel.cl";
	char* fileName = file;
	readCLs(fileName, Program_Source);
	cout << Program_Source << endl;

	const char* kernelSource[1] = {
		Program_Source
	};
	size_t programSize[] = { strlen(kernelSource[0])};

	cl_program program = clCreateProgramWithSource(context, 1, kernelSource, programSize, &err);
	*/
	clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	//Create Kernel
	cl_kernel kernel = clCreateKernel(program, "multiply_arrays", &err);

	//setting var for kernel
	WithTime([kernel,buffer_A,buffer_B,A_length,B_length,buffer_C]() {
		clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_A);
		clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_B);
		clSetKernelArg(kernel, 2, sizeof(cl_mem), &A_length);
		clSetKernelArg(kernel, 3, sizeof(cl_mem), &B_length);
		clSetKernelArg(kernel, 4, sizeof(cl_mem), &buffer_C);
		}, "SetKernelArg");

	timedog.dur_withPre("Init:");
	//running kernel in WorkSpace
	size_t globalWorkSize = lenA;
	WithTime([queue, kernel,&globalWorkSize]() {
		clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL); 
	},"Kernel运行时间");
	

	//reading result in Buffer
	//CL_TRUE : waitting kernel running end;
	void* Cdata = C.data();
	WithTime([queue,buffer_C,lenC,Cdata]() {
	//	clEnqueueReadBuffer(queue, buffer_C, CL_TRUE, 0, sizeof(float) * lenC, Cdata, 0, NULL, NULL);
		},"ReadBuffer");
	clEnqueueReadBuffer(queue, buffer_C, CL_TRUE, 0, sizeof(float) * lenC, C.data(), 0, NULL, NULL);

	//print result of C.data
	/*cout << "C array:" << endl;
	int k = 0;
	for (int i = 0; i < lenC; ++i) {
		k += C[i];
		cout << k % 10;
		k = k / 10;
	}
	cout << endl;
	*/
	timedog.Init();
	clReleaseMemObject(A_length);
	clReleaseMemObject(B_length);
	clReleaseMemObject(buffer_A);
	clReleaseMemObject(buffer_B);
	clReleaseMemObject(buffer_C);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	timedog.dur_withPre("Release:");
	return;
}

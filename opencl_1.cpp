#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>

#include <CL/cl.h>

using namespace std;

// 检查返回值错误
#define CHECK_ERRORS(ERR) \
	if(ERR != CL_SUCCESS){ \
		cerr << "OpenCL error code" << ERR \
			 << "file: " << __FILE__ \
			 << "line: " << __LINE__ \
			 << ".\nExiting..." << endl; \
		exit(1); \
	}

int check_platforms(int argc, const char** argv)
{
	cl_int err = CL_SUCCESS;

	// 1. 获取当前设备所有支持OpenCL的平台的数量
	cl_uint num_of_platforms = 0;
	err = clGetPlatformIDs(0, 0, &num_of_platforms);
	CHECK_ERRORS(err);

	// 2. 获取当前设备所有支持OpenCL的平台的信息
	cl_platform_id* platforms = new cl_platform_id[num_of_platforms];
	err = clGetPlatformIDs(num_of_platforms, platforms, 0);
	CHECK_ERRORS(err);

	cout << "平台信息：\n";
	// 3. 打印平台信息
	for (cl_uint i = 0; i < num_of_platforms; i++)
	{
		// 获取平台字符串的长度
		size_t platform_name_length = 0;
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, 0, &platform_name_length);
		CHECK_ERRORS(err);

		// 获取平台字符串
		char* platform_name = new char[platform_name_length];
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, platform_name_length, platform_name, 0);
		CHECK_ERRORS(err);

		cout << "    [" << i << "] " << platform_name << endl;
	}	



	return 0;
}

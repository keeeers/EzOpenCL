#include "Vector_func.h"
#include "time_chrono.h"
#include "PlatAndDevice.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <functional>
#include <CL/cl.h>

using namespace std;

void arrayMultiplay_normal(int);

void test(void (*func)(int),int); // 正确声明 test 函数


int main() {
	//arrayAdd();
	//arrayMultiplay(10);
	// 测试 arrayMultiplay 函数
	//test(arrayMultiplay,500000000+1);
	//test(arrayMultiplay, 5001);
	// 测试 arrayMultiplay_normal 函数
	//test(arrayMultiplay_normal,50001);
	cl_platform_id platform = GetPlatform();
	cl_device_id device = GetDevice(platform);
	ShowDeviceInfo(device);
	return 0;
}

void test(void (*func)(int),int MAXNUM=5001) { // 正确定义 test 函数
	for (int num = 50; num < MAXNUM; num *= 10) {
		WithTime([num,func](){ func(num); }, "运行完成");
		cout << "num = " << num << endl;
		cout << "_______________" << endl;
	}
}


void arrayMultiplay_normal(int array_size = 1000) {
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
	for (int i = 0; i < lenA; i++) {
		for (int j = 0; j < lenB; j++) {
			C[i + j] += A[i] * B[j];
		}
		C[i + 1] += C[i] / 10;
		C[i] = C[i] % 10;
	}
	return;
}


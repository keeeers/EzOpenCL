#include "includeALL.h"
#include "clBLASALL.h"
#include "clSTFT_core.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <functional>
#include <CL/cl.h>

using namespace std;

void arrayMultiplay_normal(int);

void test(void (*func)(int),int); // 正确声明 test 函数
void testmultiply();// 测试高精度乘法
void testplatanddevice();// 测试平台获取与设备获取及信息
void testclBLAS();// 测试clBLAS库
void test_time_chrono_class_Watcher();


int main() {
	//testmultiply();
	//testplatanddevice();
	//testclBLAS();
	clSTFT_test();
	//test_time_chrono_class_Watcher();
	return 0;
}

void test(void (*func)(int),int MAXNUM=5001) { // 正确定义 test 函数
	for (int num = 50; num < MAXNUM; num *= 10) {
		WithTime([num,func](){ func(num); }, "运行完成");
		cout << "num = " << num << endl;
		cout << "_______________" << endl;
	}
}

void testmultiply() {

	arrayAdd();
	arrayMultiplay(10);
	//测试 arrayMultiplay 函数
	test(arrayMultiplay,500000000+1);
	test(arrayMultiplay, 5001);
	//测试 arrayMultiplay_normal 函数
	test(arrayMultiplay_normal,501);
}

void testplatanddevice() {
	cl_platform_id platform = GetPlatform_PD();
	cl_device_id device = GetDevice_PD(platform);
	ShowDeviceInfo_PD(device);
}

void testclBLAS() {
	int (*fuc)(void);
	TimeKeeper timedog;
	timedog.Init();
	//example_chemm();
	timedog.dur_withPre_Init("testclBLAS");
}

void test_time_chrono_class_Watcher() {
	Watcher timeWatcher(5);
	timeWatcher.levelup("进入总层级1");
	timeWatcher.levelup("层级1-1");
	timeWatcher.leveldown();
	timeWatcher.levelup("层级1-2");
	timeWatcher.levelup("层级1-2-1");
	timeWatcher.leveldown();
	timeWatcher.leveldown();
	timeWatcher.leveldown();
}
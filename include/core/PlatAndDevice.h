#include <CL/cl.h>
#ifndef PLATANDDEVICE
#define PLATANDDEVICE
cl_platform_id GetPlatform();
cl_device_id GetDevice(cl_platform_id platform);
void ShowDeviceInfo(cl_device_id device);
#endif
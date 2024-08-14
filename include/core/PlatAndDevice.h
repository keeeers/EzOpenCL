#include "clfft_class.h"
#include <CL/cl.h>

#ifndef PLATANDDEVICE
#define PLATANDDEVICE

cl_platform_id GetPlatform_PD();
cl_device_id GetDevice_PD(cl_platform_id platform);
void ShowDeviceInfo_PD(cl_device_id device);
#endif
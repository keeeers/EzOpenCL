#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include "PlatAndDevice.h"

cl_platform_id GetPlatform_PD(){
    cl_uint platformsNum;
    clGetPlatformIDs(0, NULL, &platformsNum);
    cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformsNum);
    if (!platforms) {
        fprintf(stderr, "Failed to allocate memory for platforms.\n");
        exit(EXIT_FAILURE);
    }
    clGetPlatformIDs(platformsNum, platforms, NULL);

    // Print platform information and assign numbers
    printf("Available Platforms:\n");
    for (cl_uint i = 0; i < platformsNum; i++) {
        char platformName[128];
        char platformVersion[128];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platformVersion), platformVersion, NULL);
        printf("  %d: %s (Version: %s)\n", i + 1, platformName, platformVersion);
    }

    // Select a platform based on the user's choice
    int selectedPlatformIndex;
    printf("Select a platform by entering its number: ");
    scanf("%d", &selectedPlatformIndex);

    if (selectedPlatformIndex < 1 || selectedPlatformIndex >(int)platformsNum) {
        fprintf(stderr, "Invalid platform number selected.\n");
        exit(EXIT_FAILURE);
    }

    cl_platform_id SelectPlatform = platforms[selectedPlatformIndex - 1];

    free(platforms);

    return SelectPlatform;
}

cl_device_id GetDevice_PD(cl_platform_id platform) {
    cl_uint devicesNum;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &devicesNum);
    cl_device_id* devices = (cl_device_id*)malloc(sizeof(cl_device_id) * devicesNum);
    if (!devices) {
        fprintf(stderr, "Failed to allocate memory for devices.\n");
        exit(EXIT_FAILURE);
    }
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, devicesNum, devices, NULL);

    // Print device information and assign numbers
    printf("Available Devices:\n");
    for (cl_uint i = 0; i < devicesNum; i++) {
        char deviceName[128];
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
        printf("  %d: %s\n", i + 1, deviceName);
    }

    // Select a device based on the user's choice
    int selectedDeviceIndex;
    printf("Select a device by entering its number: ");
    scanf("%d", &selectedDeviceIndex);

    if (selectedDeviceIndex < 1 || selectedDeviceIndex >(int)devicesNum) {
        fprintf(stderr, "Invalid device number selected.\n");
        exit(EXIT_FAILURE);
    }

    cl_device_id SelectDevice = devices[selectedDeviceIndex - 1];

    free(devices);

    return SelectDevice;
}

void ShowDeviceInfo_PD(cl_device_id device) {
    char deviceName[128];
    cl_uint computeUnits;
    cl_uint maxWorkGroupSize;
    size_t maxWorkItemSizes[3];
    size_t globalMemorySize;
    size_t localMemorySize;
    cl_ulong maxClockFrequency;
    cl_device_fp_config doubleFPConfig;

    // 获取设备名称
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
    printf("Device Name: %s\n", deviceName);

    // 获取计算单元数量
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);
    printf("Max Compute Units: %zu\n", computeUnits);

    // 获取最大工作组大小
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &maxWorkGroupSize, NULL);
    printf("Max Work Group Size: %u\n", maxWorkGroupSize);

    // 获取最大工作项尺寸
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, maxWorkItemSizes, NULL);
    printf("Max Work Item Sizes: %zu x %zu x %zu\n", maxWorkItemSizes[0], maxWorkItemSizes[1], maxWorkItemSizes[2]);

    // 获取全局内存大小
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size_t), &globalMemorySize, NULL);
    printf("Global Memory Size: %zu bytes\n", globalMemorySize);

    // 获取局部内存大小
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), &localMemorySize, NULL);
    printf("Local Memory Size: %zu bytes\n", localMemorySize);

    // 获取最大时钟频率
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_ulong), &maxClockFrequency, NULL);
    printf("Max Clock Frequency: %lu MHz\n", maxClockFrequency);

    // 获取双精度浮点配置
    clGetDeviceInfo(device, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &doubleFPConfig, NULL);
    printf("Double FP Config: ");
    if (doubleFPConfig & CL_FP_DENORM) {
        printf("Denorms ");
    }
    if (doubleFPConfig & CL_FP_INF_NAN) {
        printf("Inf and NaNs ");
    }
    if (doubleFPConfig & CL_FP_ROUND_TO_NEAREST) {
        printf("Round to nearest ");
    }
    if (doubleFPConfig & CL_FP_ROUND_TO_ZERO) {
        printf("Round to zero ");
    }
    if (doubleFPConfig & CL_FP_ROUND_TO_INF) {
        printf("Round to infinity ");
    }
    if (doubleFPConfig & CL_FP_FMA) {
        printf("FMA ");
    }
    printf("\n");
}


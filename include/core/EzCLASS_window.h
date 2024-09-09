#ifndef EZCLASS_WINDOW_H
#define EZCLASS_WINDOW_H

#include <CL/cl.h>
#include <EzSTFT.h>

class EzCLASS_Window {
    /*
    * EzCLASS_Window contains the basic functions of a window func 
    * It is designed to apply to process STFT origindata from EzCLASS_data class.
    * It includes the following functions:
    * 
    * public args:
    * 
    * EzCLASS_Window 包括了窗口函数的基本功能，适用于处理STFT EzCLASS_data类 原始数据。
    * 它包含以下功能：
    * 
    * 公共参数：
    * 
    */
    public:
        //Class Config
        EzCLASS_EzSTFT* Parent;     //clFFT父类指针
        //Window Config
        double* windowData;         //窗函数数组
        int windowSize;             //窗函数长度
        char* windowType;           //窗函数类型
        //OpenCL Window Config Memory
        cl_mem window_data;         //窗函数数组在设备端的存储
        cl_mem window_len;          //窗函数长度在设备端的存储
        cl_mem window_steplen;      //窗函数步长在设备端的存储
        //OpenCL Args
        cl_int err;                 //OpenCL错误码
        const char* kernelSource;   //OpenCL内核源码
        cl_kernel kernel;           //OpenCL内核
        cl_program program;         //OpenCL程序

        EzCLASS_Window();
        ~EzCLASS_Window();

        int init();
        int initWindow();
        int checkConfig();
    private:
        void createWindowData(); 
};


#endif
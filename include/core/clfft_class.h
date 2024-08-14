#ifndef CLFFT_CLASS
#define CLFFT_CLASS

#include "clSTFT_class.h"

#include <CL/cl.h>
#include <clFFT.h>
#include <vector>
#include <complex>

class Mem_manger;
class Window_class;
class clFFT_class;
class clSTFT_class;


class clFFT_class {
public:
    cl_int err;
    cl_uint num_platforms;
    cl_platform_id* platforms;
    cl_platform_id platform;
    cl_device_id device;
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
    cl_context _ctx;
    cl_command_queue _queue;
    cl_mem _bufferIn;
    cl_mem _bufferMid;
    clfftPlanHandle _planHandle;
    clfftDim clfftDim;
    clfftSetupData fftSetup;
    size_t* clLengths;
    unsigned int N;
    double* STFT_Result;

    //ex parameter
    double* data;
    int dataLen;
    Mem_manger* class_mem;
    Window_class* class_window;
    clSTFT_class* Parent;

    clFFT_class();

    clFFT_class(clSTFT_class*);

    /*Init*/
    //Init ALL
    int INIT();
    // getPlatformAndDevice
    int getDevice();
    // Init_args
    int Init_arg();
    // Init Origin data
    int Init_data(std::vector<std::complex<double>>* array, int len);
    // Init pre window func
    int Init_Window();
    // Init clfft plan
    int Init_clfft();

    /*Runtimes*/
    int RUN();
    // run window func ; read form origin_data ; write to dur_data
    int Windowfigure();
    // run clfft
    int clFFT_Apply();

    /*Result and Release*/
    int ReadDurData();

    int clFFT_GetResult();

    int show_clfft_result();
};

class Mem_manger {
public:
    // ��ʼ��������
    cl_mem origin_data;
    size_t origin_data_len;
    size_t origin_data_step;
    // �������ݡ���������������
    cl_mem dur_data;
    size_t dur_data_len;
    int dur_data_step;
    // ����/�������
    cl_mem final_data;
    size_t final_data_len;
    size_t final_data_step;

    clFFT_class* Parent;

    Mem_manger();

    Mem_manger(clFFT_class* );
};

class Window_class {
public:
    double* windowData;
    int windowLen;
    cl_mem window_data;
    cl_mem window_len;
    cl_mem window_steplen;
    clFFT_class* Parent;
    cl_int err;
    const char* kernelSource;
    cl_kernel kernel;
    cl_program program;
    Window_class();

    Window_class(clFFT_class* );

    int Init_Window();
    int Apply_Window();

};


#endif
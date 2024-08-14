#include "clfft_class.h"
#include "PlatAndDevice.h"
#include <CL/cl.h>
#include "time_chrono.h"
#include <iostream>

// scoped utils
void checkError(cl_int err, const char* message) {
    if (err != CL_SUCCESS) {
        std::cout << "OpenCL Error: " << message << " (" << err << ")" << std::endl;
        exit(-1);
    }
}

// Class Init

class Mem_manger;
class Window_class;
class clFFT_class;
class clSTFT_class;

Mem_manger::Mem_manger() :origin_data(NULL), origin_data_len(0), origin_data_step(0),
dur_data(NULL), dur_data_len(0), dur_data_step(0),
final_data(NULL), final_data_len(0), final_data_step(0)
, Parent(NULL) {};

Mem_manger::Mem_manger(clFFT_class* father) :
    origin_data(NULL), origin_data_len(0), origin_data_step(0),
    dur_data(NULL), dur_data_len(0), dur_data_step(0),
    final_data(NULL), final_data_len(0), final_data_step(0)
    , Parent(NULL) {
};


Window_class::Window_class():
    windowData(NULL), windowLen(0), window_data(NULL),Parent(NULL)
{};

Window_class::Window_class(clFFT_class* father) :
    windowData(NULL), windowLen(0), window_data(NULL),Parent(NULL) 
{
    Parent = father;
};


clFFT_class::clFFT_class() :
    num_platforms(2), device(0), _ctx(0),
    _queue(0), clfftDim(CLFFT_1D), clLengths(NULL),
    N(0), data(NULL), dataLen(0), class_mem(NULL)
    , class_window(NULL), Parent(NULL) {
        class_mem = new Mem_manger(this);
        class_window = new Window_class(this);
    };

clFFT_class::clFFT_class(clSTFT_class* father) :
    num_platforms(2), device(0), _ctx(0),
    _queue(0), clfftDim(CLFFT_1D), clLengths(NULL),
    N(0), data(NULL), dataLen(0), class_mem(NULL)
    , class_window(NULL), Parent(NULL) {
        Parent = father;
        class_mem = new Mem_manger(this);
        class_window = new Window_class(this);
        getDevice();
        Init_arg();
    };

// Init

int clFFT_class::INIT() {
    TimeKeeper timedog;
    // getPlatformAndDevice
    //getDevice();
    
    timedog.Init();
    // Init_args
    Init_arg();
    timedog.dur_withPre_Init("___Init_arg");
    // Init Origin data
    Init_data(&Parent -> input_data.array,Parent->input_data.len);
    timedog.dur_withPre_Init("___Init_data");
    // Init pre window func
    Init_Window();
    timedog.dur_withPre_Init("___Init_Window");
    // Init clfft plan
    Init_clfft();
    timedog.dur_withPre_Init("___Init_clfft");

    return 1;
}

int clFFT_class::getDevice() {
	platform = GetPlatform_PD();
	device = GetDevice_PD(platform);
    return 1;
}

int clFFT_class::Init_arg() {
    props[1] = (cl_context_properties)platform;
    _ctx = clCreateContext(props, 1, &device, NULL, NULL, &err);
    cl_command_queue_properties properties = 0;
    _queue = clCreateCommandQueueWithProperties(_ctx, device, &properties, &err);


    return 1;
}

int clFFT_class::Init_data(std::vector<std::complex<double>>* array, int len) {
    dataLen = len;
    data = new double[dataLen*2];
    for (int i = 0; i < dataLen; i++){
        data[2*i] = (*array)[i].real();
        data[2 * i + 1] = (*array)[i].imag();
    }
    class_mem->origin_data_len = 2;
    class_mem->origin_data_step = Parent -> input_data.len;
    class_mem->origin_data = clCreateBuffer(
        _ctx,
        CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
        sizeof(double)*class_mem->origin_data_len*class_mem->origin_data_step,
        data, 
        &err
    );

    class_mem->dur_data_len = Parent->WindowSize * 2;
    class_mem->dur_data_step = Parent -> TimeCount;
    class_mem->dur_data = clCreateBuffer(
        _ctx,
        CL_MEM_READ_WRITE,
        sizeof(double)*class_mem->dur_data_len*class_mem->dur_data_step,
        NULL, 
        &err
    );

    class_mem->final_data_len = Parent->WindowSize*2;
    class_mem->final_data_step = Parent -> TimeCount;
    class_mem->final_data = clCreateBuffer(
        _ctx,
        CL_MEM_READ_WRITE,
        sizeof(double)*class_mem->final_data_len*class_mem->final_data_step,
        NULL, 
        &err
    );

    return 1;

}

int clFFT_class::Init_Window() {
    return class_window->Init_Window();;
}

int Window_class::Init_Window() {
    windowLen = Parent->Parent->WindowSize;
    //cout << "windowLen : " << windowLen << endl;
    //windowLen = 1024;
    // create empty windowData
    if (windowData == NULL) {
        windowData = new double[windowLen];
    }
    // copy windowData form clSTFT_class WindowData
    for (int i = 0; i < windowLen; i++) {
        windowData[i] = Parent->Parent->WindowData[i];
        //printf("%4.2lf \n", windowData[i]);
    }
    // create windowData Buffer
    window_data = clCreateBuffer(
        Parent->_ctx,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(double) * windowLen,
        &windowData[0],
        &err
    );
    //double* wind = new double[windowLen];
    //err = clEnqueueReadBuffer(
    //    Parent->_queue, Parent->class_window->window_data,
    //    CL_TRUE, 0,
    //    sizeof(double) * windowLen,((void *)wind),
    //    0, NULL, NULL
    //);
    //for (int i = 0; i < windowLen; i++) {
    //    printf("%4.2lf \n", wind[i]);
    //}
    //printf("________________)))__________________\n");
    window_len = clCreateBuffer(
        Parent->_ctx,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int),
        &windowLen,
        &err
    );
    window_steplen = clCreateBuffer(
        Parent->_ctx,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_int),
        &Parent->class_mem->dur_data_step,
        &err
    );
    // create kernel program
    kernelSource = R"(
        __kernel void WindowApply(
            __global const double* A,
            __global const double* B,
            __global const int* WindowSize,
            __global const int* Steplen,
            __global       double* C)
			{
				int gid = get_global_id(0);
				if (gid >= *Steplen){
					return ;
				}
                int offset = gid*(*WindowSize)*2;
				for (int i=0;i < (*WindowSize); ++i){
                    C[2*i  +offset] = A[2*i  +offset] * B[i];
                    C[2*i+1+offset] = A[2*i+1+offset] * B[i];
				}
			}
	)";
    program = clCreateProgramWithSource(Parent->_ctx, 1, (const char**)&kernelSource, NULL, &err);

    err = clBuildProgram(program, 1, &Parent->device, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "WindowApply", &err);

    //set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Parent->class_mem->origin_data);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &window_data);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &window_len);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &window_steplen);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &Parent->class_mem->dur_data);

    return 1;
}

int clFFT_class::Init_clfft() {
    clfftSetupData fftSetup;
    TimeKeeper timedog;
    err = clfftInitSetupData(&fftSetup);
    err = clfftSetup(&fftSetup);

    clLengths = new size_t[1];
    clLengths[0] = class_mem->dur_data_len/2;
    // setup clFFT
    err = clfftInitSetupData(&fftSetup);
    checkError(err, "SetupData");
    err = clfftSetup(&fftSetup);
    checkError(err, "Setup");
    // Create Plan
    err = clfftCreateDefaultPlan(&_planHandle, _ctx, clfftDim, clLengths);
    checkError(err, "CreateDefaultPlan");
    err = clfftSetPlanPrecision(_planHandle, CLFFT_DOUBLE);
    checkError(err, "Precision");
    err = clfftSetLayout(_planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
    //err = clfftSetLayout(_planHandle, CLFFT_HERMITIAN_INTERLEAVED, CLFFT_HERMITIAN_INTERLEAVED);
    checkError(err, "Layout");
    err = clfftSetResultLocation(_planHandle, CLFFT_OUTOFPLACE);
    checkError(err, "ResultLocation");
    err = clfftSetPlanBatchSize(_planHandle, class_mem->dur_data_step);
    checkError(err, "BatchSize");
    //err = clfftSetPlanBatchSize(_planHandle, 1);
    err = clfftSetPlanDistance(_planHandle, 
        Parent->WindowStep, 
        class_mem->final_data_len/2);
    checkError(err, "PlanDistance");
    timedog.Init();
    err = clfftBakePlan(_planHandle, 1, &_queue, NULL, NULL);
    checkError(err, "Bake");
    timedog.dur_withPre_Init("______clfftBakePlan");
    return 1;
}




// Runtime
int clFFT_class::RUN() {
    TimeKeeper timedog;
    timedog.Init();
    Windowfigure();
    timedog.dur_withPre_Init("__Windowfigure");
    //ReadDurData();
    //timedog.dur_withPre_Init("__ReadDurData");
    clFFT_Apply();
    timedog.dur_withPre_Init("__clFFT_Apply");
    return 1;
}

int clFFT_class::Windowfigure() {
    return class_window->Apply_Window();
}

int Window_class::Apply_Window() {
    size_t global_work_size[1] = { Parent->class_mem->origin_data_step };
    //cout << "Parent->class_mem->origin_data_step" << Parent->class_mem->origin_data_step << endl;
    size_t local_work_size[1] = { 1 };

    // execute kernel
    err = clEnqueueNDRangeKernel(Parent->_queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);

    // wait for kernel execution to finish
    err = clFinish(Parent->_queue);
    return 1;
}

int clFFT_class::clFFT_Apply() {
    // Execute Plan
    err = clfftEnqueueTransform(_planHandle, CLFFT_FORWARD, 1, &_queue, 0, NULL, NULL, &class_mem->dur_data, &class_mem->final_data, NULL);
    err = clFinish(_queue);
    return 1;
}

/*Result and Release*/
int clFFT_class::ReadDurData() {
    double* Dur_Result = new double[class_mem->dur_data_len * class_mem->dur_data_step];
    for (int i = 0; i < class_mem->dur_data_step; i++) {
        err = clEnqueueReadBuffer(
            _queue, class_mem->dur_data,
            CL_TRUE,
            i* class_mem->dur_data_len,
            sizeof(double)*class_mem->dur_data_len,
            ((void*)Dur_Result),
            0,NULL,NULL
        );
        if (i == 0) {
            for (int j = 0; j < class_mem->dur_data_len/2; j++) {
                printf("%.2lf %.2lf\n", Dur_Result[j * 2], Dur_Result[j * 2 + 1]);
            }
        }
    }
    return 1;
}

int clFFT_class::clFFT_GetResult() {
    //STFT_Result = new double[class_mem->final_data_step*class_mem->final_data_len];
    STFT_Result = new double[class_mem->final_data_len* class_mem->final_data_step];
    for (int i = 0; i < class_mem->final_data_step; i++) {
        err = clEnqueueReadBuffer(
            _queue, class_mem->final_data,
            CL_TRUE,
            i * class_mem->final_data_len * sizeof(double), // offset
            //0,// offset test
            class_mem->final_data_len * sizeof(double), //read buffer size
            ((void *)(&STFT_Result[i*class_mem->final_data_len])), // Host array
            0,
            NULL,
            NULL);
    }
    
    //cout << "final"<<class_mem->final_data_len <<" " << class_mem->final_data_step << endl;
    return 1;
}

int clFFT_class::show_clfft_result() {

    printf("_________________clfft_result_START_______________\n");
    for (int i = 0; i < class_mem->final_data_step;i++) {
        int offset = i * class_mem->final_data_len;
        for (int j = 0; j < class_mem->final_data_len/4; j++) {
            printf("%10.3lf %10.3lf\n", STFT_Result[offset+2*j],STFT_Result[offset+2*j+1]);
        }
        if (i >= 0) {
            break;
        }
    }
    printf("_________________clfft_result_END_______________\n");
    return 1;
}
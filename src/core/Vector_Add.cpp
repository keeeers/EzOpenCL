#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "Vector_func.h"

void arrayAdd() {
    const int array_size = 10;
    cl_int err;

    // ��ʼ������������ A �� B
    std::vector<float> A(array_size);
    std::vector<float> B(array_size);
    std::vector<float> C(array_size);

    // ��� A �� B ����
    for (int i = 0; i < array_size; ++i) {
        A[i] = static_cast<float>(i);
        B[i] = static_cast<float>(i + 1);
    }

    // ��ȡƽ̨���豸
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    // ���������� R
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error creating context: " << err << std::endl;
        return ;
    }

    // �����������
    // �ɰ汾����:
    // cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    // �°汾����: R
    cl_command_queue_properties properties = 0; // ��������Ϊ CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE ������
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, &properties, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error creating command queue: " << err << std::endl;
        return ;
    }

    // ��������������
    cl_mem buffer_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * array_size, A.data(), &err);
    cl_mem buffer_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * array_size, B.data(), &err);
    cl_mem buffer_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * array_size, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error creating buffers: " << err << std::endl;
        return ;
    }

    // �����ں�Դ����
    const char* kernelSource = R"(
        __kernel void add_arrays(__global const float* A,
                                 __global const float* B,
                                 __global       float* C,
                                 const int N) {
            int gid = get_global_id(0);
            if (gid < N) {
                C[gid] = A[gid] + B[gid];
            }
        }
    )";

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // �����ں˺��� R
    cl_kernel kernel = clCreateKernel(program, "add_arrays", &err);

    // �����ں˲��� R
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_A);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_B);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_C);
    clSetKernelArg(kernel, 3, sizeof(int), &array_size);

    // ִ���ں� R
    size_t globalWorkSize = array_size;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL);

    // ��ȡ��� R
    clEnqueueReadBuffer(queue, buffer_C, CL_TRUE, 0, sizeof(float) * array_size, C.data(), 0, NULL, NULL);

    // ������ O
    std::cout << "Result array C: ";
    for (int i = 0; i < array_size; ++i) {
        std::cout << C[i] << " ";
    }
    std::cout << std::endl;

    // ������Դ R
    clReleaseMemObject(buffer_A);
    clReleaseMemObject(buffer_B);
    clReleaseMemObject(buffer_C);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return ;
}
#include "example_chemm.h"
#include "time_chrono.h"

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <clBLAS.h>
#define M  4
#define N  3
size_t nrows;
static void printResult(void);

static const clblasOrder order = clblasRowMajor;
static const cl_float2 alpha = { {10, 10} };
static const clblasSide side = clblasLeft;
static const clblasUplo uplo = clblasLower;
static const cl_float2 A[M * M] = {
    {{11, 12}}, {{-1, -1}}, {{-1, -1}}, {{-1, -1}},
    {{21, 22}}, {{22, 23}}, {{-1, -1}}, {{-1, -1}},
    {{31, 32}}, {{32, 33}}, {{33, 34}}, {{-1, -1}},
    {{41, 61}}, {{42, 62}}, {{43, 73}}, {{44, 23}}
};
static const size_t lda = M;
static const cl_float2 B[M * N] = {
    {{11, -21}},  {{-12, 23}}, {{13, 33}},
    {{21, 12}},   {{22, -10}}, {{23, 5}},
    {{31, 1}},    {{-32, 65}}, {{33, -1}},
    {{1, 41}},    {{-33, 42}}, {{12, 43}},
};
static const size_t ldb = N;
static const cl_float2 beta = { {20, 20} };
static cl_float2 C[M * N] = {
    {{11, 11}},  {{-12, 12}}, {{13, 33}},
    {{21, -32}}, {{22,  -1}}, {{23, 0}},
    {{31, 13}},  {{-32, 78}}, {{33, 45}},
    {{41, 14}},  {{0,   42}}, {{43, -1}},
};
static const size_t ldc = N;
int example_chemm()
{
    TimeKeeper timedog;
    timedog.Init();

    cl_int err;
    cl_platform_id platform = 0;
    cl_device_id device = 0;
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
    cl_context ctx = 0;
    cl_command_queue queue = 0;
    cl_mem bufA, bufB, bufC;
    cl_event event = NULL;
    int ret = 0;
    /* Setup OpenCL environment. */
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        printf("clGetPlatformIDs() failed with %d\n", err);
        return 1;
    }
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        printf("clGetDeviceIDs() failed with %d\n", err);
        return 1;
    }
    props[1] = (cl_context_properties)platform;
    ctx = clCreateContext(props, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("clCreateContext() failed with %d\n", err);
        return 1;
    }

    cl_command_queue_properties properties = 0;
    queue = clCreateCommandQueueWithProperties(ctx, device, &properties, &err);
    if (err != CL_SUCCESS) {
        printf("clCreateCommandQueue() failed with %d\n", err);
        clReleaseContext(ctx);
        return 1;
    }
    /* Setup clblas. */
    err = clblasSetup();
    if (err != CL_SUCCESS) {
        printf("clblasSetup() failed with %d\n", err);
        clReleaseCommandQueue(queue);
        clReleaseContext(ctx);
        return 1;
    }
    /* Prepare OpenCL memory objects and place matrices inside them. */
    bufA = clCreateBuffer(ctx, CL_MEM_READ_ONLY, M * M * sizeof(*A),
        NULL, &err);
    bufB = clCreateBuffer(ctx, CL_MEM_READ_ONLY, M * N * sizeof(*B),
        NULL, &err);
    bufC = clCreateBuffer(ctx, CL_MEM_READ_WRITE, M * N * sizeof(*C),
        NULL, &err);
    err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0,
        M * M * sizeof(*A), A, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0,
        M * N * sizeof(*B), B, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0,
        M * N * sizeof(*C), C, 0, NULL, NULL);
    timedog.dur_withPre_Init("ex_chemm_Init:");
    /* Call clblas function. */
    err = clblasChemm(order, side, uplo, M, N, alpha, bufA,
        0, lda, bufB, 0, ldb, beta, bufC, 0, ldc, 1, &queue,
        0, NULL, &event);
    if (err != CL_SUCCESS) {
        printf("clblasSsymm() failed with %d\n", err);
        ret = 1;
    }
    else {
        /* Wait for calculations to be finished. */
        err = clWaitForEvents(1, &event);
        timedog.dur_withPre_Init("clblasChemm:");
        /* Fetch results of calculations from GPU memory. */
        err = clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, M * N * sizeof(*C),
            C, 0, NULL, NULL);
        /* At this point you will get the result of SYMM placed in C array. */
        printResult();
    }

    /* Release OpenCL events. */
    clReleaseEvent(event);

    /* Release OpenCL memory objects. */
    clReleaseMemObject(bufC);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufA);
    /* Finalize work with clblas. */
    clblasTeardown();
    /* Release OpenCL working objects. */
    clReleaseCommandQueue(queue);
    clReleaseContext(ctx);
    timedog.dur_withPre("ex_chemm release:");
    return ret;
}


static void printResult(void)
{
    size_t i, j;
    printf("Result:\n");

    nrows = (sizeof(C) / sizeof(cl_float2)) / ldc;
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ldc; j++) {
            printf("<%9.2f, %-9.2f> ", CREAL(C[i * ldc + j]), CIMAG(C[i * ldc + j]));
        }
        printf("\n");
    }
}
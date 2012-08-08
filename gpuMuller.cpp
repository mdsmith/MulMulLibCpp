// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/5/2012
//
// XXX Implement scaling

#include "muller.h"
#include <iostream>
#include <stdio.h>
#include "helperFunctions.h"
#include <sys/stat.h>
#include <sys/time.h>
#if defined(__APPLE__) || defined(APPLE)
    #include <OpenCL/OpenCL.h>
#else
    #include <CL/opencl.h>
#endif
using namespace std;

#define BLOCK_SIZE 16

cl_context ctx;
cl_kernel kernel;
cl_command_queue queue;
size_t global_work_size[2];
size_t local_work_size[2];
cl_int err_num;

cl_mem d_A;
cl_mem d_B;
cl_mem d_C;

GPUMuller::GPUMuller()
{
    //cout << "naiveMuller constructed" << endl;
    A.set = false;
    B.set = false;
    C.set = false;
}


void GPUMuller::setup_context()
{
    set_C(new float[A.h*B.w], A.h, B.w);

    cl_platform_id plat = NULL;
    cl_device_id *devices = NULL;
    cl_device_id device = NULL;
    cl_uint dev_count = 0;
    err_num = CL_SUCCESS;

    // Plat setup
    err_num = clGetPlatformIDs(1, &plat, NULL);
    if (err_num != CL_SUCCESS)
    {
        cout << "Plat fail" << endl;
        exit(err_num);
    }

    // Dev setup
    err_num = clGetDeviceIDs(plat, CL_DEVICE_TYPE_GPU, 0, NULL, &dev_count);
    devices = (cl_device_id *)malloc(dev_count * sizeof(cl_device_id));
    err_num = clGetDeviceIDs(plat, CL_DEVICE_TYPE_GPU, dev_count, devices, NULL);
    device = devices[0];
    if (err_num != CL_SUCCESS)
    {
        cout << "Dev fail" << endl;
        exit(err_num);
    }

    // Context setup
    // 1 == my device count (arbitrary)
    ctx = clCreateContext(0, 1, &device, NULL, NULL, &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "Ctx fail" << endl;
        exit(err_num);
    }

    // get device info
    size_t returned_size = 0;
    cl_char vendor_name[1024] = {0};
    cl_char device_name[1024] = {0};
    err_num = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor_name),
                             vendor_name, &returned_size);
    err_num |= clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name),
                              device_name, &returned_size);
    if (err_num != CL_SUCCESS)
    {
        cout << "Name fetch fail" << endl;
        exit(err_num);
    }
    printf("Connecting to %s %s...\n", vendor_name, device_name);

    // queue setup
    queue = clCreateCommandQueue(   ctx,
                                    device,
                                    0,
                                    &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "queue fail" << endl;
        exit(err_num);
    }


    // prog setup
    const char* source = load_program_source("oclKernels.cl");
    cl_program prog = clCreateProgramWithSource(ctx, 1, &source, NULL, &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "compile fail" << endl;
        exit(err_num);
    }

    // build program
    err_num = clBuildProgram(prog, 1, &device, "-cl-mad-enable", NULL, NULL);
    if (err_num != CL_SUCCESS)
    {
        cout << "build fail" << endl;
        exit(err_num);
    }

    // kernel setup
    kernel = clCreateKernel(prog, "matMul", &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make kernel fail" << endl;
        exit(err_num);
    }

    update_buffers();
}

// XXX this is pretty dang inefficient
void GPUMuller::update_buffers()
{
    // array rounding
    int pad_to = 64;

    int bw_round = round_up(B.w, pad_to);
    int ud_round = round_up(A.w, pad_to);
    int ah_round = round_up(A.h, pad_to);

    int a_h_bound = A.h;
    int a_w_bound = A.w;
    int b_w_bound = B.w;

    set_A(pad(A.data, A.h, A.w, pad_to), ah_round, ud_round);
    set_B(pad(B.data, B.h, B.w, pad_to), ud_round, bw_round);
    set_C(pad(C.data, C.h, C.w, pad_to), ah_round, bw_round);

    // work dim setup
    global_work_size[0] = ah_round;
    global_work_size[1] = bw_round;
    local_work_size[0] = BLOCK_SIZE;
    local_work_size[1] = BLOCK_SIZE;

    if (d_A != NULL)
        err_num = clReleaseMemObject(d_A);
    d_A = clCreateBuffer(   ctx,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            ah_round*ud_round*sizeof(float),
                            A.data,
                            &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_B != NULL)
        err_num = clReleaseMemObject(d_B);
    d_B = clCreateBuffer(   ctx,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            bw_round*ud_round*sizeof(float),
                            B.data,
                            &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_C != NULL)
        err_num = clReleaseMemObject(d_C);
    // XXX You don't need to copy C! Fix this!
    d_C = clCreateBuffer(   ctx,
                            CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                            ah_round*bw_round*sizeof(float),
                            C.data,
                            &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }
}


// XXX what if the buffer needs to get bigger... I need to make a new buffer
//void GPUMuller::update_buffers()
//{
    // XXX when arrays are updated, we also need to update the buffers on the
    // device

    // XXX update Global and local work sizes
    // XXX update padding
    // XXX update buffers on device
//}


void GPUMuller::multiply()
{
    cl_int temp_ah = A.h;
    cl_int temp_bw = B.w;
    cl_int temp_bw_round = B.num_cols;
    cl_int temp_ud = A.w;
    cl_int temp_ud_round = A.num_cols;

    // set kernel args
    err_num  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &d_A);
    err_num |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &d_B);
    err_num |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &d_C);
    err_num |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *) &temp_ud);
    err_num |= clSetKernelArg(kernel, 4, sizeof(cl_int), (void *) &temp_ud_round);
    err_num |= clSetKernelArg(kernel, 5, sizeof(cl_int), (void *) &temp_bw_round);
    err_num |= clSetKernelArg(kernel, 6, sizeof(cl_int), (void *) &temp_bw);
    err_num |= clSetKernelArg(kernel, 7, sizeof(cl_int), (void *) &temp_ah);
    if (err_num != CL_SUCCESS)
    {
        cout << "kernel arg set fail" << endl;
        exit(err_num);
    }

    // launch kernel
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    err_num = clEnqueueNDRangeKernel(   queue,
                                        kernel,
                                        2,
                                        0,
                                        global_work_size,
                                        local_work_size,
                                        0,
                                        NULL,
                                        NULL);
    if (err_num != CL_SUCCESS)
    {
        cout << "kernel launch fail" << endl;
        exit(err_num);
    }
    clFinish(queue);
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec -t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    cout << "Multiplication: " << elapsedTime << " ms.\n";

    // get results
    err_num = clEnqueueReadBuffer(  queue,
                                    d_C,
                                    CL_FALSE,
                                    0,
                                    A.num_rows*B.num_cols*sizeof(float),
                                    C.data,
                                    0,
                                    NULL,
                                    NULL);
    if (err_num != CL_SUCCESS)
    {
        cout << "read fail" << endl;
        exit(err_num);
    }
    clFinish(queue);
    C.set = true;
}


float* GPUMuller::get_C(int offset, int width, int height)
{
    int a_o = A.offset;
    int a_w = A.w;
    int a_h = A.h;

    int b_o = B.offset;
    int b_w = B.w;
    int b_h = B.h;

    if (ctx == NULL)
        setup_context();
    else
        update_buffers();

    bound_A(a_o, a_h, a_w);
    bound_B(b_o, b_h, b_w);

    multiply();

    return matrix_slice(C, offset, width, height);
}


void GPUMuller::test()
{
    cout << "GPUMuller checking in" << endl;
}


GPUMuller::~GPUMuller()
{
    // cleanup
    err_num |= clReleaseMemObject(d_A);
    err_num |= clReleaseMemObject(d_B);
    err_num |= clReleaseMemObject(d_C);
    err_num |= clReleaseKernel(kernel);
    err_num |= clReleaseCommandQueue(queue);
    err_num |= clReleaseContext(ctx);
    if (err_num != CL_SUCCESS)
    {
        cout << "free fail" << endl;
        exit(err_num);
    }

    //cout << "naiveMuller destructed" << endl;
}

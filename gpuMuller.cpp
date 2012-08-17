// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/5/2012
//
// XXX Implement scaling

#include "gpuMuller.h"
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

bool cleanBuff;
bool a_dirt;
bool b_dirt;
bool c_dirt;
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
    cleanBuff = false;
    a_dirt = false;
    b_dirt = false;
    c_dirt = false;
}


const char* GPUMuller::get_name()
{
    return "GPU";
}


void GPUMuller::set_A(float* A, int num_rows, int num_cols)
{
    if (ctx != NULL)
        cleanBuff = false;
    Muller::set_A(A, num_rows, num_cols);
}

void GPUMuller::set_B(float* B, int num_rows, int num_cols)
{
    if (ctx != NULL)
        cleanBuff = false;
    Muller::set_B(B, num_rows, num_cols);
}

void GPUMuller::set_C(float* C, int num_rows, int num_cols)
{
    if (ctx != NULL)
        cleanBuff = false;
    Muller::set_C(C, num_rows, num_cols);
}

void GPUMuller::update_A(float* A, int offset, int ah, int ud, int num_rows, int num_cols)
{
    Muller::update_A(A, offset, ah, ud, num_rows, num_cols);
    if (ctx != NULL)
        a_dirt = true;
}

void GPUMuller::update_B(float* B, int offset, int ud, int bw, int num_rows, int num_cols)
{
    Muller::update_B(B, offset, ud, bw, num_rows, num_cols);
    if (ctx != NULL)
        b_dirt = true;
}


void GPUMuller::setup_context()
{
    if (C.data == NULL)
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

void GPUMuller::update_buffers()
{
    // array rounding
    int pad_to = 64;

    int b_num_cols_round = round_up(B.num_cols, pad_to);
    int a_num_cols_round = round_up(A.num_cols, pad_to);
    int a_num_rows_round = round_up(A.num_rows, pad_to);

    int bw_round = round_up(B.w, pad_to);
    int ud_round = round_up(A.w, pad_to);
    int ah_round = round_up(A.h, pad_to);

    int a_h_bound = A.h;
    int a_w_bound = A.w;
    int b_w_bound = B.w;

    //cout << "Pre padding: " << endl;
    //print_A();
    //print_B();
    //print_C();

    if (A.data == B.data && A.data == C.data)
    {
        set_A(pad(A.data, A.num_rows, A.num_cols, pad_to), a_num_rows_round, a_num_cols_round);
        set_B(A.data, a_num_rows_round, a_num_cols_round);
        set_C(A.data, a_num_rows_round, a_num_cols_round);
    }
    else if (A.data == B.data)
    {
        set_A(pad(A.data, A.num_rows, A.num_cols, pad_to), a_num_rows_round, a_num_cols_round);
        set_B(A.data, a_num_rows_round, a_num_cols_round);
        set_C(pad(C.data, C.num_rows, C.num_cols, pad_to), a_num_rows_round, b_num_cols_round);
    }
    else if (B.data == C.data)
    {
        set_A(pad(A.data, A.num_rows, A.num_cols, pad_to), a_num_rows_round, a_num_cols_round);
        set_B(pad(B.data, B.num_rows, B.num_cols, pad_to), a_num_cols_round, b_num_cols_round);
        set_C(B.data, a_num_cols_round, b_num_cols_round);
    }
    else if (A.data == C.data)
    {
        set_A(pad(A.data, A.num_rows, A.num_cols, pad_to), a_num_rows_round, a_num_cols_round);
        set_B(pad(B.data, B.num_rows, B.num_cols, pad_to), a_num_cols_round, b_num_cols_round);
        set_C(A.data, a_num_rows_round, a_num_cols_round);
    }
    else
    {
        set_A(pad(A.data, A.num_rows, A.num_cols, pad_to), a_num_rows_round, a_num_cols_round);
        set_B(pad(B.data, B.num_rows, B.num_cols, pad_to), a_num_cols_round, b_num_cols_round);
        set_C(pad(C.data, C.num_rows, C.num_cols, pad_to), a_num_rows_round, b_num_cols_round);
    }

    //cout << "Post padding: " << endl;
    //print_A();
    //print_B();
    //print_C();

    // work dim setup
    global_work_size[0] = ah_round;
    global_work_size[1] = bw_round;
    local_work_size[0] = BLOCK_SIZE;
    local_work_size[1] = BLOCK_SIZE;

    if (d_A != NULL)
        err_num = clReleaseMemObject(d_A);
    d_A = clCreateBuffer(   ctx,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            a_num_rows_round * a_num_cols_round * sizeof(float),
                            A.data,
                            &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_B != NULL)
        err_num = clReleaseMemObject(d_B);
    else if (A.data == B.data)
        d_B = d_A;
    else
        d_B = clCreateBuffer(   ctx,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                b_num_cols_round * a_num_cols_round * sizeof(float),
                                B.data,
                                &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_C != NULL)
        err_num = clReleaseMemObject(d_C);
    else if (A.data == C.data)
        d_C = d_A;
    else if (B.data == C.data)
        d_C = d_B;
    else
        d_C = clCreateBuffer(   ctx,
                                //CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                                CL_MEM_READ_WRITE,
                                a_num_rows_round * b_num_cols_round * sizeof(float),
                                //C.data,
                                NULL,
                                &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }
    cleanBuff = true;
}


void GPUMuller::check_buffers()
{
    if (!cleanBuff)
        update_buffers();

    if (a_dirt)
        err_num = clEnqueueWriteBuffer( queue,
                                        d_A,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*A.num_rows*A.num_cols,
                                        A.data,
                                        0,
                                        NULL,
                                        NULL
                                        );
    if (b_dirt)
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_B,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*B.num_rows*B.num_cols,
                                        B.data,
                                        0,
                                        NULL,
                                        NULL
                                        );
    if (c_dirt)
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_C,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*C.num_rows*C.num_cols,
                                        C.data,
                                        0,
                                        NULL,
                                        NULL
                                        );
    if (err_num != CL_SUCCESS)
    {
        cout << "Error rewritting buffers" << endl;
        exit(err_num);
    }

}


void GPUMuller::multiply()
{
    cl_int temp_ah = A.h;
    cl_int temp_bw = B.w;
    cl_int temp_bw_round = B.num_cols;
    cl_int temp_ud = A.w;
    cl_int temp_ud_round = A.num_cols;
    cl_int temp_a_offset = A.offset;
    cl_int temp_b_offset = B.offset;
    cl_int temp_c_offset = C.offset;

    // set kernel args
    err_num  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &d_A);
    err_num |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &d_B);
    err_num |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &d_C);
    err_num |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *) &temp_ud);
    err_num |= clSetKernelArg(kernel, 4, sizeof(cl_int), (void *) &temp_ud_round);
    err_num |= clSetKernelArg(kernel, 5, sizeof(cl_int), (void *) &temp_bw_round);
    err_num |= clSetKernelArg(kernel, 6, sizeof(cl_int), (void *) &temp_bw);
    err_num |= clSetKernelArg(kernel, 7, sizeof(cl_int), (void *) &temp_ah);
    err_num |= clSetKernelArg(kernel, 8, sizeof(cl_int), (void *) &temp_a_offset);
    err_num |= clSetKernelArg(kernel, 9, sizeof(cl_int), (void *) &temp_b_offset);
    err_num |= clSetKernelArg(kernel, 10, sizeof(cl_int), (void *) &temp_c_offset);
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
                                    A.num_rows * B.num_cols * sizeof(float),
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

    C.offset = offset;
    C.w = width;
    C.h = height;

    int a_o = A.offset;
    int a_w = A.w;
    int a_h = A.h;

    int b_o = B.offset;
    int b_w = B.w;
    int b_h = B.h;

    //cout << "Before buffer creation: " << endl;
    //print_A();
    //print_B();

    if (ctx == NULL)
        setup_context();
    else
        check_buffers();

    bound_A(a_o, a_h, a_w);
    bound_B(b_o, b_h, b_w);

    C.offset = offset;
    C.w = width;
    C.h = height;

    //cout << "After rebounding: " << endl;
    //print_A();
    //print_B();

    multiply();
    cout << "C after multiply: " << endl;
    print_C(0, C.num_rows, C.num_cols);

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

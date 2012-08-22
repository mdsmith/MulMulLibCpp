// Author: Martin Smith
// Project: Flexible MULtiple MULtiplicaiton LIBrary in C Plus Plus.
// Created: Early August 2012
// Last Edited: 8/20/2012
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
#define PAD_SIZE 64
#define SCALAR 100

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
cl_mem d_As;
cl_mem d_B;
cl_mem d_Bs;
cl_mem d_C;
cl_mem d_Cs;

GPUMuller::GPUMuller()
{
    //cout << "naiveMuller constructed" << endl;
    //A = new Matrix();
    //B = new Matrix();
    //C = new Matrix();
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

void GPUMuller::bound_A(int offset, int ah, int ud)
{
    Muller::bound_A(offset, ah, ud);
}

void GPUMuller::bound_B(int offset, int ud, int bw)
{
    Muller::bound_B(offset, ud, bw);
}


void GPUMuller::setup_context()
{
    if (!C.is_set())
        set_C(  new float[A.get_bound_rows()*B.get_bound_cols()],
                A.get_bound_rows(),
                B.get_bound_cols()
                );

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

    A.update_scalings();
    B.update_scalings();
    C.update_scalings();
    update_buffers();
}

void GPUMuller::update_buffers()
{
    // array rounding
    if (A.get_scaled() == B.get_scaled() && A.get_scaled() == C.get_scaled())
    {
        A.pad_to(PAD_SIZE);
        B.set_data_and_scalings(A.get_scaled(),
                                A.get_scalings(),
                                A.get_total_rows(),
                                A.get_total_cols());
        C.set_data_and_scalings(A.get_scaled(),
                                A.get_scalings(),
                                A.get_total_rows(),
                                A.get_total_cols());
    }
    else if (B.get_scaled() == A.get_scaled())
    {
        A.pad_to(PAD_SIZE);
        B.set_data_and_scalings(A.get_scaled(),
                                A.get_scalings(),
                                A.get_total_rows(),
                                A.get_total_cols());
        C.pad_to(PAD_SIZE);
    }
    else if (C.get_scaled() == B.get_scaled())
    {
        A.pad_to(PAD_SIZE);
        B.pad_to(PAD_SIZE);
        C.set_data_and_scalings(B.get_scaled(),
                                B.get_scalings(),
                                B.get_total_rows(),
                                B.get_total_cols());
    }
    else if (C.get_scaled() == A.get_scaled())
    {
        A.pad_to(PAD_SIZE);
        B.pad_to(PAD_SIZE);
        C.set_data_and_scalings(A.get_scaled(),
                                A.get_scalings(),
                                A.get_total_rows(),
                                A.get_total_cols());
    }
    else
    {
        A.pad_to(PAD_SIZE);
        B.pad_to(PAD_SIZE);
        C.pad_to(PAD_SIZE);
    }

/*
    cout << "Post padding: " << endl;
    print_A();
    print_B();
    print_C();
*/

    // work dim setup
    global_work_size[0] = round_up(A.get_bound_rows(),PAD_SIZE);
    global_work_size[1] = round_up(B.get_bound_cols(),PAD_SIZE);
    local_work_size[0] = BLOCK_SIZE;
    local_work_size[1] = BLOCK_SIZE;

    if (d_A != NULL)
    {
        err_num = clReleaseMemObject(d_A);
        err_num |= clReleaseMemObject(d_As);
    }
    if (err_num != CL_SUCCESS)
    {
        cout << "release buffer fail" << endl;
        exit(err_num);
    }
    d_A = clCreateBuffer(   ctx,
                            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                            A.get_total_rows() * A.get_total_cols() * sizeof(float),
                            A.get_scaled(),
                            &err_num);
    d_As = clCreateBuffer(   ctx,
                            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                            A.get_total_rows() * A.get_total_cols() * sizeof(int),
                            A.get_scalings(),
                            &err_num);
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_B != NULL)
    {
        err_num = clReleaseMemObject(d_B);
        err_num |= clReleaseMemObject(d_Bs);
        if (err_num != CL_SUCCESS)
        {
            cout << "release buffer fail" << endl;
            exit(err_num);
        }
    }
    if (B.get_scaled() == A.get_scaled())
    {
        d_B = d_A;
        d_Bs = d_As;
    }
    else
    {
        d_B = clCreateBuffer(   ctx,
                                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                B.get_total_rows() * B.get_total_cols() *
                                sizeof(float),
                                B.get_scaled(),
                                &err_num);
        d_Bs = clCreateBuffer(  ctx,
                                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                B.get_total_rows() * B.get_total_cols() *
                                sizeof(int),
                                B.get_scalings(),
                                &err_num);
    }
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }

    if (d_C != NULL)
    {
        err_num = clReleaseMemObject(d_C);
        err_num |= clReleaseMemObject(d_Cs);
        if (err_num != CL_SUCCESS)
        {
            cout << "release buffer fail" << endl;
            exit(err_num);
        }
    }
    if (C.get_scaled() == A.get_scaled())
    {
        d_C = d_A;
        d_Cs = d_As;
    }
    else if (C.get_scaled() == B.get_scaled())
    {
        d_C = d_B;
        d_Cs = d_Bs;
    }
    else
    {
        d_C = clCreateBuffer(   ctx,
                                CL_MEM_READ_WRITE,
                                A.get_total_rows() * B.get_total_cols() *
                                sizeof(float),
                                NULL,
                                &err_num);
        d_C = clCreateBuffer(   ctx,
                                CL_MEM_READ_WRITE,
                                A.get_total_rows() * B.get_total_cols() *
                                sizeof(int),
                                NULL,
                                &err_num);
    }
    if (err_num != CL_SUCCESS)
    {
        cout << "make buffer fail" << endl;
        exit(err_num);
    }
    cleanBuff = true;
}


// XXX updating buffers only works if you're replacing the whole buffer
// (nothing on the GPU will be written back before being overwritten)
void GPUMuller::check_buffers()
{
    if (!cleanBuff)
        update_buffers();

    if (a_dirt)
    {
        err_num = clEnqueueWriteBuffer( queue,
                                        d_A,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*A.get_total_rows()*A.get_total_cols(),
                                        A.get_scaled(),
                                        0,
                                        NULL,
                                        NULL
                                        );
        err_num = clEnqueueWriteBuffer( queue,
                                        d_As,
                                        CL_FALSE,
                                        0,
                                        sizeof(int)*A.get_total_rows()*A.get_total_cols(),
                                        A.get_scalings(),
                                        0,
                                        NULL,
                                        NULL
                                        );
    }
    if (b_dirt)
    {
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_B,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*B.get_total_rows()*B.get_total_cols(),
                                        B.get_scaled(),
                                        0,
                                        NULL,
                                        NULL
                                        );
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_Bs,
                                        CL_FALSE,
                                        0,
                                        sizeof(int)*B.get_total_rows()*B.get_total_cols(),
                                        B.get_scalings(),
                                        0,
                                        NULL,
                                        NULL
                                        );
    }
    if (c_dirt)
    {
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_C,
                                        CL_FALSE,
                                        0,
                                        sizeof(cl_float)*C.get_total_rows()*C.get_total_cols(),
                                        C.get_scaled(),
                                        0,
                                        NULL,
                                        NULL
                                        );
        err_num |= clEnqueueWriteBuffer( queue,
                                        d_Cs,
                                        CL_FALSE,
                                        0,
                                        sizeof(int)*C.get_total_rows()*C.get_total_cols(),
                                        C.get_scalings(),
                                        0,
                                        NULL,
                                        NULL
                                        );
    }
    if (err_num != CL_SUCCESS)
    {
        cout << "Error rewritting buffers" << endl;
        exit(err_num);
    }
}


void GPUMuller::multiply()
{
    cl_int temp_ah = A.get_bound_rows();
    cl_int temp_bw = B.get_bound_cols();
    cl_int temp_bw_round = B.get_total_cols();
    cl_int temp_ud = A.get_bound_cols();
    cl_int temp_ud_round = A.get_total_cols();
    cl_int temp_a_offset = A.get_offset();
    cl_int temp_b_offset = B.get_offset();
    cl_int temp_c_offset = C.get_offset();

    // set kernel args
    err_num  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &d_A);
    err_num |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &d_As);
    err_num |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &d_B);
    err_num |= clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *) &d_Bs);
    err_num |= clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *) &d_C);
    err_num |= clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *) &d_Cs);
    err_num |= clSetKernelArg(kernel, 6, sizeof(cl_int), (void *) &temp_ud);
    err_num |= clSetKernelArg(kernel, 7, sizeof(cl_int), (void *) &temp_ud_round);
    err_num |= clSetKernelArg(kernel, 8, sizeof(cl_int), (void *) &temp_bw_round);
    err_num |= clSetKernelArg(kernel, 9, sizeof(cl_int), (void *) &temp_bw);
    err_num |= clSetKernelArg(kernel, 10, sizeof(cl_int), (void *) &temp_ah);
    err_num |= clSetKernelArg(kernel, 11, sizeof(cl_int), (void *) &temp_a_offset);
    err_num |= clSetKernelArg(kernel, 12, sizeof(cl_int), (void *) &temp_b_offset);
    err_num |= clSetKernelArg(kernel, 13, sizeof(cl_int), (void *) &temp_c_offset);
    if (err_num != CL_SUCCESS)
    {
        cout << "kernel arg set fail" << endl;
        exit(err_num);
    }

    // launch kernel
    timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

    cout << "Global work size: " << global_work_size[0];
    cout << ", " << global_work_size[1] << endl;
    cout << "Local work size: " << local_work_size[0];
    cout << ", " << local_work_size[1] << endl;
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
                                    C.get_total_rows() * C.get_total_cols()* sizeof(float),
                                    C.get_scaled(),
                                    0,
                                    NULL,
                                    NULL);
    err_num = clEnqueueReadBuffer(  queue,
                                    d_Cs,
                                    CL_FALSE,
                                    0,
                                    C.get_total_rows() * C.get_total_cols()*
                                    sizeof(int),
                                    C.get_scalings(),
                                    0,
                                    NULL,
                                    NULL);
    if (err_num != CL_SUCCESS)
    {
        cout << "read fail" << endl;
        exit(err_num);
    }
    clFinish(queue);
    C.set_set(true);
}


float* GPUMuller::get_C(int offset, int width, int height)
{

// XXX XXX HERE!!!!!! I think I need to slightly change my interface to be
// less ambiguous (set_A set_B bound_A bound_B [[set_C] bound_C] get_C)
    C.bound_data(offset, height, width);

// XXX this should no longer be needed
/*
    int a_o = A.offset;
    int a_w = A.w;
    int a_h = A.h;

    int b_o = B.offset;
    int b_w = B.w;
    int b_h = B.h;
*/

    //cout << "Before buffer creation: " << endl;
    //print_A();
    //print_B();

    if (ctx == NULL)
        setup_context();
    else
        check_buffers();

// XXX this should no longer be needed
/*
    bound_A(a_o, a_h, a_w);
    bound_B(b_o, b_h, b_w);

    C.offset = offset;
    C.w = width;
    C.h = height;
*/

    //cout << "After rebounding: " << endl;
    //print_A();
    //print_B();

    multiply();
    cout << "C after multiply: " << endl;
    print_C(0, C.get_total_rows(), C.get_total_cols());

    return C.get_slice(offset, width, height);
}


/*
void GPUMuller::scale(  float* input_data,
                        float* temp_scaled_data,
                        scaltype* scalings
                        )
{
    // XXX make this actually do something
    // XXX leaving this data laying around can cause some pretty serious
    // safety problems
    temp_scaled_data = input_data;
}

// Why am I scaling into a float here? Shouldn't this be a double? And if it
// is, aren't I kind of screwed?
float* GPUMuller::unscale(float* temp_scaled_data,
                        scaltype* scalings
                        )
{
    // XXX make this actually do something
    // XXX is this going to cause a memory leak?
    // XXX should this resolved by using set instead of a standard
    // assignment?
    // XXX leaving this data laying around can cause some pretty serious
    // safety problems
    float* tbr = new float[sizeof(temp_scaled_data)/sizeof(float)];
    memcpy((void*)tbr, (void*)temp_scaled_data, sizeof(temp_scaled_data));
    //return (float*)tbr;
    return (float*)tbr;
}
*/


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

#include "conv.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define SOURCE_KERNEL ("src/convolution/gpu/operation.cl")
#define COUNT_COLOR (3)  // RGB

char *read_source_kernel(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fpr_err("Error: opening kernel source failed\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    if (length < 0) {
        fclose(file);
        fpr_err("Error: unnatural file length\n");
        return NULL;
    }
    fseek(file, 0, SEEK_SET);

    char *source = (char *)malloc((size_t)length + 1);
    if (source == NULL) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(source, 1, length, file);
    source[bytes_read] = '\0';

    fclose(file);
    return source;
}

BMP *cleanup_and_return(BMP *bmp_conv, float *matrix, char *source_kernel,
                        unsigned char *data_source, unsigned char *data_conv,
                        cl_mem *buf_source, cl_mem *buf_conv,
                        cl_mem *buf_matrix_f, cl_kernel *kernel,
                        cl_program *program, cl_command_queue *queue,
                        cl_context *context, cl_device_id *device, int code) {
    free(matrix);
    free(source_kernel);
    free(data_source);
    free(data_conv);
    if (buf_source) clReleaseMemObject(*buf_source);
    if (buf_conv) clReleaseMemObject(*buf_conv);
    if (buf_matrix_f) clReleaseMemObject(*buf_matrix_f);
    if (kernel) clReleaseKernel(*kernel);
    if (program) clReleaseProgram(*program);
    if (queue) {
        clFinish(*queue);
        clReleaseCommandQueue(*queue);
    }
    if (context) clReleaseContext(*context);
    if (device) clReleaseDevice(*device);
    if (code) return bmp_conv;
    bclose(bmp_conv);
    return NULL;
}

// NOLINTBEGIN
BMP *conv_gpu_seq(BMP *bmp, Options opt) {
    BMP *bmp_conv = b_create(bmp);
    if (bmp_conv == NULL) {
        return NULL;
    }

    int size_matrix_f = opt.filter->height * opt.filter->width * sizeof(float);
    float *matrix = (float *)malloc(size_matrix_f);
    if (matrix == NULL) {
        fpr_err(ERROR_MALLOC);
        bclose(bmp_conv);
        return NULL;
    }
    for (int index = 0; index < opt.filter->height * opt.filter->width;
         index++) {
        matrix[index] = (float)(opt.filter->matrix[index]);
    }
    float factor = (float)opt.factor;
    float bias = (float)opt.bias;

    char *source_kernel = read_source_kernel(SOURCE_KERNEL);
    if (source_kernel == NULL) {
        bclose(bmp_conv);
        free(matrix);
        return NULL;
    }

    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_program program = clCreateProgramWithSource(
        context, 1, (const char **)&source_kernel, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "apply_filter_tiled", NULL);

    int height = get_height(bmp);
    int width = get_width(bmp);
    int size_buf = height * width * COUNT_COLOR * sizeof(unsigned char);

    unsigned char *data_source = (unsigned char *)malloc(size_buf);
    if (data_source == NULL) {
        fpr_err(ERROR_MALLOC);
        cleanup_and_return(bmp_conv, matrix, source_kernel, NULL, NULL, NULL,
                           NULL, NULL, &kernel, &program, &queue, &context,
                           &device, 0);
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char r, g, b;
            get_pixel_rgb(bmp, x, y, &r, &g, &b);
            int index = (y * width + x) * COUNT_COLOR;
            data_source[index] = r;
            data_source[index + 1] = g;
            data_source[index + 2] = b;
        }
    }

    cl_mem buf_source =
        clCreateBuffer(context, CL_MEM_READ_ONLY, size_buf, NULL, NULL);
    cl_mem buf_conv =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_buf, NULL, NULL);
    cl_mem buf_matrix_f =
        clCreateBuffer(context, CL_MEM_READ_ONLY, size_matrix_f, NULL, NULL);
    cl_mem buf_debug = clCreateBuffer(
        context, CL_MEM_WRITE_ONLY, height * width * sizeof(float), NULL, NULL);

    clEnqueueWriteBuffer(queue, buf_source, CL_TRUE, 0, size_buf, data_source,
                         0, NULL, NULL);
    clEnqueueWriteBuffer(queue, buf_matrix_f, CL_TRUE, 0, size_matrix_f, matrix,
                         0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buf_source);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buf_conv);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &buf_matrix_f);
    clSetKernelArg(kernel, 3, sizeof(int), &height);
    clSetKernelArg(kernel, 4, sizeof(int), &width);
    clSetKernelArg(kernel, 5, sizeof(int), &(opt.filter->height));
    clSetKernelArg(kernel, 6, sizeof(int), &(opt.filter->width));
    clSetKernelArg(kernel, 7, sizeof(float), &factor);
    clSetKernelArg(kernel, 8, sizeof(float), &bias);
    clSetKernelArg(kernel, 9, sizeof(cl_mem), &buf_debug);

    size_t size_global[2] = {height, width};
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, size_global, NULL, 0, NULL,
                           NULL);

    unsigned char *data_conv = (unsigned char *)malloc(size_buf);
    if (data_conv == NULL) {
        fpr_err(ERROR_MALLOC);
        return cleanup_and_return(bmp_conv, matrix, source_kernel, data_source,
                                  NULL, &buf_source, &buf_conv, &buf_matrix_f,
                                  &kernel, &program, &queue, &context, &device,
                                  0);
    }
    clEnqueueReadBuffer(queue, buf_conv, CL_TRUE, 0, size_buf, data_conv, 0,
                        NULL, NULL);

    float *debug = (float *)malloc(height * width * sizeof(float));
    clEnqueueReadBuffer(queue, buf_debug, CL_TRUE, 0,
                        height * width * sizeof(float), debug, 0, NULL, NULL);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x);
            set_pixel_rgb(bmp_conv, x, y, data_conv[index * COUNT_COLOR],
                          data_conv[index * COUNT_COLOR + 1],
                          data_conv[index * COUNT_COLOR + 2]);
        }
    }

    return cleanup_and_return(bmp_conv, matrix, source_kernel, data_source,
                              data_conv, &buf_source, &buf_conv, &buf_matrix_f,
                              &kernel, &program, &queue, &context, &device, 1);
}
// NOLINTEND

int conv_gpu_seq_mode(char **argv, Options opt) {
    double time_start = get_time();

    BMP *bmp = bopen(argv[1]);
    if (bmp == NULL) {
        fpr_err("Error: opening input file failed\n");
        return 1;
    }

    BMP *bmp_conv = conv_gpu_seq(bmp, opt);
    if (bmp_conv == NULL) {
        return 1;
    }

    bwrite(bmp_conv, argv[2]);
    double time_end = get_time();

    printf("Check %s\n", argv[2]);
    fpr_log("Log: %fs spent\n", time_end - time_start);

    bclose(bmp);
    bclose(bmp_conv);
    return 0;
}

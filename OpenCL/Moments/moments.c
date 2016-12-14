#define PROGRAM_FILE "moments.cl"
#define KERNEL_FUNC "moments"
#define VALUE 2

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

#include <time.h>

int main() {
   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int err, i;
   
   const int IMAGE_WIDTH  = 16;
   const int IMAGE_HEIGHT = 4;
   const int ARRAY_SIZE = IMAGE_HEIGHT * IMAGE_WIDTH;
   const int KERNEL_SIZE = 8;                                          //the number of pixels each kernel should process
   size_t NUM_WORK_ITEMS = IMAGE_WIDTH / KERNEL_SIZE;                  //the number of work items in each work group
   size_t GLOBAL_SIZE = (IMAGE_WIDTH * IMAGE_HEIGHT) / KERNEL_SIZE;    //total number of kernels
   size_t NUM_WORK_GROUPS = IMAGE_HEIGHT;
   printf("Following environment will be created:\n\
           Image size: %dx%d\n\
           Kernel size: %d\n\
           Total number of kernels: %d\n\
           Number of work-groups: %d\n\
           Number of work-items in each group: %d\n",
           IMAGE_WIDTH, IMAGE_HEIGHT, KERNEL_SIZE, GLOBAL_SIZE, NUM_WORK_GROUPS, NUM_WORK_ITEMS);
   
   /* Data and buffers */
   float data[ARRAY_SIZE];
   float sum[NUM_WORK_GROUPS];/// total, actual_sum;
   cl_mem input_buffer, sum_buffer;


    printf("Data vector:");
   /* Initialize data with random 0 and 1 */
    for(i=0; i<ARRAY_SIZE; i++) {
        if(i % IMAGE_WIDTH == 0)
            printf("\n");
        data[i] = i % 5 % 2;
        printf("%5.2f ", data[i]);
    }
    printf("\n");

   /* Create device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer */
   input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), data, &err);
   sum_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, NUM_WORK_GROUPS * sizeof(float), sum, &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 0, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

   /* Create a kernel */
   kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   };

    int tmp = NUM_WORK_ITEMS;
   /* Create kernel arguments */
   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
   err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &sum_buffer);
   err |= clSetKernelArg(kernel, 2, sizeof(tmp), &tmp);
   if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
   }

    clock_t begin = clock();

  
   /* Enqueue kernel */
   err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &GLOBAL_SIZE, 
         &NUM_WORK_ITEMS, 0, NULL, NULL); 
   if(err < 0) {
      perror("Couldn't enqueue the kernel");
      printf("Error: %d\n", err);
      exit(1);
   }

   /* Read the kernel's output */
   err = clEnqueueReadBuffer(queue, sum_buffer, CL_TRUE, 0, 
         sizeof(sum), sum, 0, NULL, NULL);
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);
   }
    //~ int moment11 = 0;
    //~ for(int i = 0; i < IMAGE_HEIGHT; i++)
    //~ {
        //~ moment11 += sum[i];
    //~ }   
    clock_t end = clock();
    double time_spent = 1000 * ((double)(end - begin) / CLOCKS_PER_SEC);
    printf("Elapsed time: %f [ms]\n", time_spent);
    //~ printf("M11 = %d\n", moment11);

   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(sum_buffer);
   clReleaseMemObject(input_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
      
   return 0;
}

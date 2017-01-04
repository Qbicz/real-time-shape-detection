#define PROGRAM_FILE "moments.cl"
#define KERNEL_FUNC "moments"

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
#include <math.h>

double* computeMomentsWithOpenCL(float** a, const int IMAGE_WIDTH,  const int IMAGE_HEIGHT, double* moments, const int NUM_CENTRAL_MOMENTS)
{
   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int err, i, j;
   
   //Full HD is 1920 x 1080
   //~ const int IMAGE_WIDTH  = 16; 
   //~ const int IMAGE_HEIGHT = 2;
   const int ARRAY_SIZE = IMAGE_HEIGHT * IMAGE_WIDTH;
   const int KERNEL_SIZE = 8;                                          //the number of pixels each kernel should process
   size_t NUM_WORK_ITEMS = IMAGE_WIDTH / KERNEL_SIZE;                  //the number of work items in each work group
   size_t GLOBAL_SIZE = (IMAGE_WIDTH * IMAGE_HEIGHT) / KERNEL_SIZE;    //total number of kernels
   size_t NUM_WORK_GROUPS = IMAGE_HEIGHT;
   const size_t NUM_MOMENTS = 4; //temporary buffer for MX0, MX1, MX2, MX3
   printf("The following environment will be created:\n\
           Image size: %dx%d\n\
           Kernel size: %d\n\
           Total number of kernels: %zu\n\
           Number of work-groups: %zu\n\
           Number of work-items in each group: %zu\n",
           IMAGE_WIDTH, IMAGE_HEIGHT, KERNEL_SIZE, GLOBAL_SIZE, NUM_WORK_GROUPS, NUM_WORK_ITEMS);
   
   /* Data and buffers */
   //calculate center of mass as x_ and y_
   

   float x_, y_, m00 = 0, m10 = 0, m01 = 0;
   
   float data2d[IMAGE_HEIGHT][IMAGE_WIDTH];
    
   for(i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(j = 0; j < IMAGE_WIDTH; j++)
       {
           data2d[i][j] = 255 *  ((i+j) % 5 % 3 % 2) ;
           printf("%5.2f ", data2d[i][j]);
           m00 += data2d[i][j];
           m01 += (j+1) * data2d[i][j];
           m10 += (i+1) * data2d[i][j];
       }
       printf("\n");
   }
   x_ = m10 / m00;
   y_ = m01 / m00;
   printf("M00 = %8.2f, M01 = %8.2f, M10 = %8.2f\n", m00, m01, m10);
   printf("Center of mass: [%f, %f]\n", x_, y_);

   
   float sum[NUM_MOMENTS * NUM_WORK_GROUPS];
   
   //~ double* moments = new double[NUM_CENTRAL_MOMENTS]; 
   //moments to be used in host application
   
   int workgroups_left = NUM_WORK_GROUPS; //decrement counter

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
   cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), data2d, &err);
   cl_mem sum_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, NUM_MOMENTS * NUM_WORK_GROUPS * sizeof(float), sum, &err);
   cl_mem moments_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, NUM_CENTRAL_MOMENTS * sizeof(double), moments, &err);
   cl_mem workgroups_left_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int), &workgroups_left, &err);
   
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
   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);          //input data
   err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &sum_buffer);           //buffer for each of the work groups
   err |= clSetKernelArg(kernel, 2, sizeof(tmp), &tmp);                     //pass number of work items as arg
   err |= clSetKernelArg(kernel, 3, 4 * NUM_WORK_ITEMS * sizeof(float), NULL);  //local memory for summing values inside work group for moments mx0, mx1, mx2, mx3
   err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &moments_buffer);       //global memory for summing values inside work group
   err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &workgroups_left_buffer);
   err |= clSetKernelArg(kernel, 6, sizeof(x_), &x_);   //pass the center of mass
   err |= clSetKernelArg(kernel, 7, sizeof(y_), &y_);   //pass the center of mass
   
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
   //~ err = clEnqueueReadBuffer(queue, sum_buffer, CL_TRUE, 0, 
         //~ sizeof(sum), sum, 0, NULL, NULL);
   err = clEnqueueReadBuffer(queue, moments_buffer, CL_TRUE, 0, NUM_CENTRAL_MOMENTS*sizeof(double), moments, 0, NULL, NULL);
   printf("SIZEOF MOMENTS %d\n", sizeof(moments));       
         
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);
   }
    
    clock_t end = clock();
    double time_spent = 1000 * ((double)(end - begin) / CLOCKS_PER_SEC);
    
    //Results 
    printf("Elapsed time: %f [ms]\n", time_spent);

    //Check the answers
    
    printf("Validiating the answers...\n");
    printf("[Moment]\t[Pure C]\t[OpenCL] \n");
   double m11 = 0, m30 =0, m03 =0, m12 = 0, m21 = 0, m20 =0, m02 =0;
   for(int i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(int j = 0; j < IMAGE_WIDTH; j++)
       {
           float cx = 1 + i - x_;
           float cy = 1 + j - y_;
           
           m02 += cy*cy*data2d[i][j];
           m03 += cy*cy*cy*data2d[i][j];
           m11 += cx*cy*data2d[i][j];
           m12 += cx*cy*cy*data2d[i][j];
           m20 += cx*cx*data2d[i][j];
           m21 += cx*cx*cy*data2d[i][j];
           m30 += cx*cx*cx*data2d[i][j];           
       }
   }    
    printf("Moment M02\t%8.2f\t%8.2f\n", m02,moments[0]);
    printf("Moment M03\t%8.2f\t%8.2f\n", m03,moments[1]);
    printf("Moment M11\t%8.2f\t%8.2f\n", m11,moments[2]);
    printf("Moment M12\t%8.2f\t%8.2f\n", m12,moments[3]);
    printf("Moment M20\t%8.2f\t%8.2f\n", m20,moments[4]);
    printf("Moment M21\t%8.2f\t%8.2f\n", m21,moments[5]);
    printf("Moment M30\t%8.2f\t%8.2f\n", m30,moments[6]);

    printf("Computing normalized central moments...\n");
    printf("Check usage of powf %f\n", powf(m00, 2.5));
    double n21 = m21 / powf(m00, 2.5),
           n03 = m03 / powf(m00, 2.5),
           n30 = m30 / powf(m00, 2.5),
           n12 = m12 / powf(m00, 2.5);
    
    printf("Normalized central moment n21 = %f\n", n21);
    printf("Normalized central moment n03 = %f\n", n03);
    printf("Normalized central moment n30 = %f\n", n30);
    printf("Normalized central moment n12 = %f\n", n12);


    printf("Computing Hu moments...\n");
    float Hu7 = (3*n21-n03)*(n30+n12)*( (n30+n12) * (n30+n12) - 3 * (n21+n03)*(n21+n03) ) - 
                 (n30-3*n12) * (n12+n03) * ( 3*(n30+n12)*(n30+n12) -(n21+n03)*(n21+n03) );
    printf("Hu7 = %f\n", Hu7);


   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(sum_buffer);
   clReleaseMemObject(input_buffer);
   clReleaseMemObject(workgroups_left_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
      
   return 0; 
}


int main()
{
    const int IMAGE_WIDTH  = 16; 
    const int IMAGE_HEIGHT = 2;
    float** image = new float*[IMAGE_WIDTH];
    
    for(int i = 0; i < IMAGE_WIDTH; i++)
    {
        image[i] = new float[IMAGE_HEIGHT];
    }
    
       //~ float data2d[IMAGE_HEIGHT][IMAGE_WIDTH]; 
   for(int i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(int j = 0; j < IMAGE_WIDTH; j++)
       {
           image[i][j] = 255 *  ((i+j) % 5 % 3 % 2) ;
       }
   }
    
    
    const size_t NUM_CENTRAL_MOMENTS = 7; //m11, m12, m20, m02, m30, m03 is enough for computing HU moments
    double moments[NUM_CENTRAL_MOMENTS];
    
    computeMomentsWithOpenCL(image, IMAGE_WIDTH, IMAGE_HEIGHT, moments, NUM_CENTRAL_MOMENTS);
}

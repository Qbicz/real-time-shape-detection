#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

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



using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 150;
int max_thresh = 255;
const float scale = 4e9;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );
void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale);
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);
void computeMomentsWithOpenCL(cv::Mat& frame, double* moments, const int NUM_CENTRAL_MOMENTS);


/** @function main */
int main( int argc, char** argv )
{
  // TODO: check for argc < 2

  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );

  /// Convert image to gray and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) );

  /// Create Window
  const char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src );

  createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );

  waitKey(0);
  return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
  clock_t begin = clock();  
  Mat canny_output;
  vector<vector<Point> > contours;
  
  vector<vector<double>> huMoments;  
  vector<Vec4i> hierarchy;

  /// Detect edges using canny
  Canny( src_gray, canny_output, thresh, thresh*2, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


	/// Leave only contours with appropriate length
	const int contour_thresh = 200;
	const bool closed = false;
	vector<vector<Point> > filteredContours;
	for(size_t i = 0; i < contours.size(); i++ )
	{
		  if(arcLength(contours[i], closed) > contour_thresh)
		  {
		      filteredContours.push_back(contours[i]);
		  }
  }
  contours.assign(filteredContours.begin(), filteredContours.end());
  

  /// Get the moments
  vector<Moments> mu(contours.size() );
  for(size_t i = 0; i < contours.size(); i++ )
     { mu[i] = moments( contours[i], false ); }
    
    printf("Found contours: %zu\n", contours.size());
    
    ///  Get the mass centers:
    vector<Point2f> mc( contours.size() );
    for(size_t i = 0; i < contours.size(); i++ )
    { 
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );    
    }

    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
       
    for(size_t i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );     
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }
     
  /// Compute Hu moments - use to tell the difference between mirrored objects/rotated 180 degrees
  for(size_t i = 0; i < contours.size(); i++)
  {
      Moments mom = mu[i];
      double hu[7];
      HuMoments(mom, hu);
      
      vector<double> huTmp = {hu[0], hu[1], hu[2], hu[3], hu[4], hu[5], hu[6]};
      huMoments.push_back(huTmp);
            
      //printf("Hu invariants for contour %zu:\n", i);
      //for( int i = 0; i < 7; i++ )
         //printf("[%d]=%.4e ", i+1, hu[i]);
      //printf("\n");

      /// Show 7th Hu moment as an arrow from the mass center
      // mass_center = mc[i];
      Point hu_orient = Point(static_cast<int>(mc[i].x) , static_cast<int>(mc[i].y+scale*(hu[6]))); // 7th Hu moment as a vertical arrow
      drawAxis(drawing, mc[i], hu_orient, Scalar(255, 255, 0), 5);
  }
  /*
   *  TODO: add PCA and draw orientation: major PCA direction multiplied by 7th Hu moment
   */ 
  clock_t end = clock();
  double time_spent = 1000 * ((double)(end - begin) / CLOCKS_PER_SEC);


  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing ); 

  /// Calculate the area with the moments 00 and compare with the result of the OpenCV function
  printf("\t Info: Area and Contour Length \n");
  for(size_t i = 0; i< contours.size(); i++ )
     {
       printf(" * Contour[%zu] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength( contours[i], true ) );
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }
     
  printf("Compute moments in OpenCL and compare them to other results...\n");   
  for(size_t i = 0; i < contours.size(); i++)
  {
    printf("Contour id: %zu\n", i);
    Mat contour =  Mat::zeros( canny_output.size(), CV_8UC1 );
    Scalar white = Scalar(255, 255, 255);
    drawContours( contour, contours, i, white, 1, 8, hierarchy, 0, Point() ); //to fill the contour use negative value (-1) for thickness (now 2)
    Moments im_mom = moments(contour, false);
  
  
  const size_t NUM_CENTRAL_MOMENTS = 7; //m11, m12, m20, m02, m30, m03 is enough for computing HU moments
  double moments[NUM_CENTRAL_MOMENTS];
        
  if(contour.cols %8 != 0)
  {
      printf("Unable to perform openCL computations - image width must be a multiplication of 8!\n");
      return;
  }     
  
    computeMomentsWithOpenCL(contour, moments, NUM_CENTRAL_MOMENTS);
    
     //Check the answers
    
   unsigned char* data_ptr = contour.data;
   const int IMAGE_WIDTH = contour.cols;
   const int IMAGE_HEIGHT = contour.rows;     
   float data2d[IMAGE_HEIGHT][IMAGE_WIDTH], x_, y_;

   double m00 = 0, m01 = 0, m10 = 0, m11 = 0, m30 =0, m03 =0, m12 = 0, m21 = 0, m20 =0, m02 =0;
   for(int i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(int j = 0; j < IMAGE_WIDTH; j++)
       {
           data2d[i][j] = data_ptr[i * contour.step + j];
           //moments
           m00 += data2d[i][j];
           m01 += (j+1) * data2d[i][j];
           m10 += (i+1) * data2d[i][j];
       }
   }    
   
   x_ = m10 / m00;
   y_ = m01 / m00;
   
   for(int i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(int j = 0; j < IMAGE_WIDTH; j++)
       {
           float cx = 1 + i - x_;
           float cy = 1 + j - y_;
           //central moments
           m20 += cy*cy*data2d[i][j];
           m30 += cy*cy*cy*data2d[i][j];
           m11 += cx*cy*data2d[i][j];
           m21 += cx*cy*cy*data2d[i][j];
           m02 += cx*cx*data2d[i][j];
           m12 += cx*cx*cy*data2d[i][j];
           m03 += cx*cx*cx*data2d[i][j];           
       }
   } 
      //Results 
    printf("Elapsed time in OpenCV: %f [ms]\n", time_spent);
    
    //printf("Moment M00\t%8.2f\n", im_mom.m00);
    //printf("Moment M01\t%8.2f\n", im_mom.m01);
    //printf("Moment M10\t%8.2f\n", im_mom.m10);


    //printf("Center of mass: [%f, %f]\n", x_, y_);
    //printf("Validiating the answers...\n");
    printf("[Moment]\t\t[Pure C]\t[OpenCV]\t[OpenCL] \n");
    printf("Moment M00\t\t%8e\t%8e\tN/A\n", m00, im_mom.m00);
    printf("Moment M01\t\t%8e\t%8e\tN/A\n", m10, im_mom.m01); //ugly hack - m01 instead of m10, values are not used again so its ok
    printf("Moment M10\t\t%8e\t%8e\tN/A\n", m01, im_mom.m10);   
    printf("Central Moment Mu02\t%8e\t%8e\t%8e\n", m02, im_mom.mu02, moments[0]);
    printf("Central Moment Mu03\t%8e\t%8e\t%8e\n", m03, im_mom.mu03, moments[1]);
    printf("Central Moment Mu11\t%8e\t%8e\t%8e\n", m11, im_mom.mu11, moments[2]);
    printf("Central Moment Mu12\t%8e\t%8e\t%8e\n", m12, im_mom.mu12, moments[3]);
    printf("Central Moment Mu20\t%8e\t%8e\t%8e\n", m20, im_mom.mu20, moments[4]);
    printf("Central Moment Mu21\t%8e\t%8e\t%8e\n", m21, im_mom.mu21, moments[5]);
    printf("Central Moment Mu30\t%8e\t%8e\t%8e\n", m30, im_mom.mu30, moments[6]);

    //printf("Computing normalized central moments...\n");
    double nu21 = m21 / powf(m00, 2.5),
           nu03 = m03 / powf(m00, 2.5),
           nu30 = m30 / powf(m00, 2.5),
           nu12 = m12 / powf(m00, 2.5),
           nu20 = m20 / powf(m00, 2),
           nu02 = m02 / powf(m00, 2),
           nu11 = m11 / powf(m00, 2);
    
    printf("Norm. centr. moment N21\tN/A \t\t%8e\t%8e\n", im_mom.nu21, nu21);
    printf("Norm. centr. moment N03\tN/A \t\t%8e\t%8e\n", im_mom.nu03, nu03);
    printf("Norm. centr. moment N30\tN/A \t\t%8e\t%8e\n", im_mom.nu30, nu30);
    printf("Norm. centr. moment N12\tN/A \t\t%8e\t%8e\n", im_mom.nu12, nu12);
    printf("Norm. centr. moment N20\tN/A \t\t%8e\t%8e\n", im_mom.nu20, nu20);
    printf("Norm. centr. moment N02\tN/A \t\t%8e\t%8e\n", im_mom.nu02, nu02);
    printf("Norm. centr. moment N11\tN/A \t\t%8e\t%8e\n", im_mom.nu11, nu11);


    //printf("Computing Hu invariants\n");
    //instead of computing hu moments by hand we can fall back to OpenCV implementation again
    Moments openClMoments;
    openClMoments.m00 = m00; openClMoments.m01 = m10; openClMoments.m10 = m01;
    
    openClMoments.mu11 = m11; openClMoments.mu03 = m03; openClMoments.mu02 = m02; openClMoments.mu12 = m12;
    openClMoments.mu21 = m21; openClMoments.mu20 = m20; openClMoments.mu30 = m30;
    
    openClMoments.nu11 = nu11; openClMoments.nu03 = nu03; openClMoments.nu02 = nu02; openClMoments.nu12 = nu12;
    openClMoments.nu21 = nu21; openClMoments.nu20 = nu20; openClMoments.nu30 = nu30;
        
    
    //and so on, remember to check m10 and m01
    double huOpenCl[7];
    HuMoments(openClMoments, huOpenCl);
    
    printf("Hu invariant Hu1\tN/A \t\t%8e\t%8e\n", huMoments[i][0], huOpenCl[0]);
    printf("Hu invariant Hu2\tN/A \t\t%8e\t%8e\n", huMoments[i][1], huOpenCl[1]);
    printf("Hu invariant Hu3\tN/A \t\t%8e\t%8e\n", huMoments[i][2], huOpenCl[2]);
    printf("Hu invariant Hu4\tN/A \t\t%8e\t%8e\n", huMoments[i][3], huOpenCl[3]);
    printf("Hu invariant Hu5\tN/A \t\t%8e\t%8e\n", huMoments[i][4], huOpenCl[4]);
    printf("Hu invariant Hu6\tN/A \t\t%8e\t%8e\n", huMoments[i][5], huOpenCl[5]);
    printf("Hu invariant Hu7\tN/A \t\t%8e\t%8e\n", huMoments[i][6], huOpenCl[6]);
  }
}


/**
 * @function drawAxis
 */
void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale)
{
//! [visualization1]
    double angle;
    double hypotenuse;
    angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
    hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range

    // Here we lengthen the arrow by a factor of scale
    q.x = (int) (p.x - scale * hypotenuse * cos(angle));
    q.y = (int) (p.y - scale * hypotenuse * sin(angle));
    line(img, p, q, colour, 1, CV_AA);

    // create the arrow hooks
    p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);

    p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
//! [visualization1]
}


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

void computeMomentsWithOpenCL(cv::Mat& frame, double* moments, const int NUM_CENTRAL_MOMENTS)
{

  if(frame.isContinuous())
  {
    //printf("Wymiary: %dx%d\n", frame.rows, frame.cols);
    //printf("Adres: %d\n", &frame); 
    //std::cout << frame << std::endl;   
  }
   const int IMAGE_WIDTH = frame.cols;
   const int IMAGE_HEIGHT = frame.rows;     
   unsigned char* data_ptr = frame.data;
   float x_, y_, m00 = 0, m10 = 0, m01 = 0;
   float data2d[IMAGE_HEIGHT][IMAGE_WIDTH];
    
   for(int i = 0; i < IMAGE_HEIGHT; i++)
   {
       for(int j = 0; j < IMAGE_WIDTH; j++)
       {
           data2d[i][j] = data_ptr[i * frame.step + j];
           //~ printf("%5.2f ", data2d[i][j]);
           m00 += data2d[i][j];
           m01 += (j+1) * data2d[i][j];
           m10 += (i+1) * data2d[i][j];
       }
       //~ printf("\n");
   }
   x_ = m10 / m00;
   y_ = m01 / m00;
   
   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int err;
   
   const int ARRAY_SIZE = IMAGE_HEIGHT * IMAGE_WIDTH;
   const int KERNEL_SIZE = 8;                                          //the number of pixels each kernel should process
   size_t NUM_WORK_ITEMS = IMAGE_WIDTH / KERNEL_SIZE;                  //the number of work items in each work group
   size_t GLOBAL_SIZE = (IMAGE_WIDTH * IMAGE_HEIGHT) / KERNEL_SIZE;    //total number of kernels
   size_t NUM_WORK_GROUPS = IMAGE_HEIGHT;
   const size_t NUM_MOMENTS = 4; //temporary buffer for MX0, MX1, MX2, MX3
   /*printf("The following environment will be created:\n\
           //Image size: %dx%d\n\
           //Kernel size: %d\n\
           //Total number of kernels: %zu\n\
           //Number of work-groups: %zu\n\
           //Number of work-items in each group: %zu\n",
           //IMAGE_WIDTH, IMAGE_HEIGHT, KERNEL_SIZE, GLOBAL_SIZE, NUM_WORK_GROUPS, NUM_WORK_ITEMS);
   */
   /* Data and buffers */
   //calculate center of mass as x_ and y_
     
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


   clock_t begin = clock();
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
         
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);
   }
    
    clock_t end = clock();
    double time_spent = 1000 * ((double)(end - begin) / CLOCKS_PER_SEC);
    
    //Results 
    printf("Elapsed time in OpenCL: %f [ms]\n", time_spent);

   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(sum_buffer);
   clReleaseMemObject(input_buffer);
   clReleaseMemObject(workgroups_left_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
}

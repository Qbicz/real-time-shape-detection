//~ /**********************************************************************
//~ Copyright ©2015 Advanced Micro Devices, Inc. All rights reserved.

//~ Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

//~ .   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//~ .   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 //~ other materials provided with the distribution.

//~ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 //~ WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 //~ DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 //~ OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 //~ NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//~ ********************************************************************/

//~ /**
//~ *******************************************************************************
//~ * @file <CartoonFilter.hpp>
//~ * @brief Show interops by using cartoon filter. Class declaration.
//~ *******************************************************************************
//~ */



//~ /**
//~ * CartoonFilter
//~ * To show the various interops between OpenCV, OpenCL and OpenCV-CL.
//~ */
//~ class CartoonFilter
//~ {
        //~ std::string imageFile;  /*!<Input image file.*/
        //~ double kernelTime;     /**< Kernel time taken by the Sample */
        //~ int timer;     /*!<Time taken for execution.*/

        //~ cv::Mat cvFrameCPU;  /*!<The input image file in cv::Mat format.*/
        //~ cv::Mat cvSrcMat;    /*!<The input cv::Mat for filtering.*/
        //~ cv::Mat cvDstMat;    /*!<The output cv::Mat obtained after filtering.*/
        //~ cv::Mat cvCannyDstMat;  /*!<The output canny matrix in cv::Mat.*/
        //~ cv::Mat cvDstCPU;    /*!<The output cv::Mat obtained after
                        //~ *  verification by running on CPU.*/

        //~ cv::ocl::oclMat oclSrcMat;  /*!<The input image in cv::ocl::oclMat format.*/
        //~ cv::ocl::oclMat oclCannyMat;/*!<The Canny input mat in
                               //~ *  cv::ocl::oclMat format*/
        //~ SDKTimer *sampleTimer;

    //~ public:
        //~ OpenCVCommandArgs* sampleArgs; /*!<OpenCVCommandArgs object.*/

        //~ /**
        //~ * Constructor
        //~ * Initialize OpenCVCommandArgs and SDKTimer objects, default image name
        //~ */
        //~ CartoonFilter() : imageFile("CartoonFilter_Input.jpg")
        //~ {
            //~ sampleArgs = new OpenCVCommandArgs();
            //~ sampleTimer = new SDKTimer();
            //~ sampleArgs->sampleVerStr = SAMPLE_VERSION;
        //~ }

        //~ /** Destructor
        //~ */
        //~ ~CartoonFilter() {}

        //~ // Verification
        //~ /**
        //~ ***************************************************************************
        //~ * @fn do_meanShiftOnCPU
        //~ * @brief Perform Mean Shift Filtering on CPU.
        //~ ***************************************************************************
        //~ */
        //~ void do_meanShiftOnCPU(int x0, int y0, uchar *sptr, uchar *dptr, int sstep,
                               //~ cv::Size size, int sp, int sr, int maxIter, float eps, int *tab);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn verifyMeanShiftFilterOnCPU
        //~ * @brief Call do_meanShiftOnCPU for every pixel of the image.
        //~ ***************************************************************************
        //~ */
        //~ void verifyMeanShiftFilterOnCPU(const cv::Mat src, cv::Mat &dst, int sp,
                                        //~ int sr, cv::TermCriteria crit = cv::TermCriteria(
                                                //~ cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 5, 1));

        //~ /**
        //~ ***************************************************************************
        //~ * @fn doPixelComparision_canny
        //~ * @brief Verify if src1 matches closely to src2.
        //~ *
        //~ * @return int Success or failure status.
        //~ ***************************************************************************
        //~ */
        //~ int doPixelComparision_canny(const cv::Mat src1, const cv::Mat src2);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn CPURefCartoonFilter
        //~ * @brief Run the cartoon filter on CPU for reference.
        //~ *
        //~ * @param cvSrcCPU The input cv::Mat on which the Cartoon Filter is run.
        //~ * @param cvDstCPU The output cv::Mat obtained.
        //~ *
        //~ * @return void
        //~ ***************************************************************************
        //~ */
        //~ void CPURefCartoonFilter(cv::Mat cvSrcCPU, cv::Mat &cvDstCPU);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn verifyResults
        //~ * @brief Compare the GPU Cartoon filtered image with the image
        //~ *        obtained from CPU run.
        //~ *
        //~ * @return int SDK_SUCCESS on success and nonzero on failure.
        //~ ***************************************************************************
        //~ */
        //~ int verifyResults();

        //~ // Color Conversions
        //~ /**
        //~ ***************************************************************************
        //~ * @fn RGB2Gray_caller
        //~ * @brief Convert RGB image to Gray by doing a OpenCL interop.
        //~ *
        //~ * @param oclSrc The source matrix in RGB format.
        //~ * @param oclDst The color converted destination matrix in Gray format.
        //~ * @param int The index.
        //~ ***************************************************************************
        //~ */
        //~ void RGB2Gray_caller(const oclMat &oclSrc, oclMat &oclDst, int bidx);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn Gray2RGB_caller
        //~ * @brief Convert Gray image to RGB by doing a OpenCL interop.
        //~ *
        //~ * @param oclSrc The source matrix in Gray format.
        //~ * @param oclDst The color converted destination matrix in RGB format.
        //~ ***************************************************************************
        //~ */
        //~ void Gray2RGB_caller(const oclMat &oclSrc, oclMat &oclDst);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn myCvtColor
        //~ * @brief My convert color chooses and calls the relevant color converter.
        //~ *
        //~ * @param oclSrc The source matrix that needs to be color converted.
        //~ * @param oclDst The color converted destination matrix.
        //~ * @param code The color conversion code.
        //~ ***************************************************************************
        //~ */
        //~ void myCvtColor(const oclMat &oclSrc, oclMat &oclDst, int code);

        //~ /**
        //~ ***************************************************************************
        //~ * @fn initialize
        //~ * @brief Initialize command line arguments.
        //~ *
        //~ * @return int SDK_SUCCESS on success and nonzero on failure.
        //~ ***************************************************************************
        //~ */
        //~ int initialize();

        //~ /**
        //~ ***************************************************************************
        //~ * @fn setUp
        //~ * @brief Setup device, input file.
        //~ *
        //~ * @return int SDK_SUCCESS on success and nonzero on failure.
        //~ ***************************************************************************
        //~ */
        //~ int setUp();

        //~ /**
        //~ ***************************************************************************
        //~ * @fn showInterops
        //~ * @brief Show interops by using Cartoon Filter.
        //~ *
        //~ * @return int SDK_SUCCESS on success and nonzero on failure.
        //~ ***************************************************************************
        //~ */
        //~ int showInterops();

        //~ /**
        //~ ***************************************************************************
        //~ * @fn run
        //~ * @brief Run the filter.
        //~ *
        //~ * @return int SDK_SUCCESS on success and nonzero on failure.
        //~ ***************************************************************************
        //~ */
        //~ int run();

        //~ /**
        //~ ***************************************************************************
        //~ * @fn displayCartoonImage
        //~ * @brief Display the original image and the cartoon.
        //~ ***************************************************************************
        //~ */
        //~ void displayCartoonImage()
        //~ {
            //~ // Write to disk.
            //~ imwrite("Output_Cartoon.jpg", cvDstMat);

            //~ if (sampleArgs->quiet)
            //~ {
                //~ return;
            //~ }

            //~ // display
            //~ cv::namedWindow("original");
            //~ cv::namedWindow("cartoon");
            //~ cv::imshow("original", cvFrameCPU);
            //~ cv::imshow("cartoon", cvDstMat);
            //~ waitKey(0);
        //~ }
//~ };





//~ /**
//~ *******************************************************************************
//~ * @fn convertToString
//~ * @brief Convert character array to std::string.
//~ *
//~ * @param filename The character array.
//~ * @param str std::string.
//~ *******************************************************************************
//~ */
//~ int convertToString(
        //~ const char *filename, 
        //~ std::string& str);

//~ /**
//~ *******************************************************************************
//~ * Implementation of convertToString                                           *
//~ ******************************************************************************/
//~ int convertToString(const char *filename, std::string& s)
//~ {
    //~ size_t size;
    //~ char*  str;

    //~ // create a file stream object by filename
    //~ std::fstream f(filename, (std::fstream::in | std::fstream::binary));


    //~ if(!f.is_open())
    //~ {
     	//~ return SDK_FAILURE;   
    //~ }
    //~ else
    //~ {
        //~ size_t fileSize;
        //~ f.seekg(0, std::fstream::end);
        //~ size = fileSize = (size_t)f.tellg();
        //~ f.seekg(0, std::fstream::beg);

        //~ str = new char[size+1];
        //~ if(!str)
        //~ {
            //~ f.close();
            //~ return SDK_FAILURE;
        //~ }

        //~ f.read(str, fileSize);
        //~ f.close();
        //~ str[size] = '\0';

        //~ s = str;
        //~ delete[] str;
        //~ return SDK_SUCCESS;
    //~ }
//~ }

//~ /**
//~ *******************************************************************************
//~ * Implementation of CartoonFilter::do_meanShiftOnCPU                          *
//~ ******************************************************************************/
//~ void CartoonFilter::do_meanShiftOnCPU(int x0, int y0, uchar *sptr, uchar *dptr,
                                      //~ int sstep, cv::Size size, int sp, int sr, int maxIter,
                                      //~ float eps, int *tab)
//~ {
    //~ int isr2 = sr * sr;
    //~ int c0, c1, c2, c3;
    //~ int iter;
    //~ uchar *ptr = NULL;
    //~ uchar *pstart = NULL;
    //~ int revx = 0, revy = 0;
    //~ c0 = sptr[0];
    //~ c1 = sptr[1];
    //~ c2 = sptr[2];
    //~ c3 = sptr[3];

    //~ /****************************************************************************
    //~ * Iterate meanshift procedure                                               *
    //~ ****************************************************************************/
    //~ for(iter = 0; iter < maxIter; iter++ )
    //~ {
        //~ int count = 0;
        //~ int s0 = 0, s1 = 0, s2 = 0, sx = 0, sy = 0;

        //~ /****************************************************************************
        //~ * mean shift: process pixels in window (p-sigmaSp)x(p+sigmaSp)              *
        //~ ****************************************************************************/
        //~ int minx = x0 - sp;
        //~ int miny = y0 - sp;
        //~ int maxx = x0 + sp;
        //~ int maxy = y0 + sp;

        //~ /****************************************************************************
        //~ * Deal with the image boundary.                                             *
        //~ ****************************************************************************/
        //~ if(minx < 0)
        //~ {
            //~ minx = 0;
        //~ }
        //~ if(miny < 0)
        //~ {
            //~ miny = 0;
        //~ }
        //~ if(maxx >= size.width)
        //~ {
            //~ maxx = size.width - 1;
        //~ }
        //~ if(maxy >= size.height)
        //~ {
            //~ maxy = size.height - 1;
        //~ }
        //~ if(iter == 0)
        //~ {
            //~ pstart = sptr;
        //~ }
        //~ else
        //~ {
            //~ pstart = pstart + revy * sstep + (revx << 2); //point to the new position
        //~ }
        //~ ptr = pstart;
        //~ //point to the start in the row
        //~ ptr = ptr + (miny - y0) * sstep + ((minx - x0) << 2);
        //~ for( int y = miny; y <= maxy; y++, ptr += sstep - ((maxx - minx + 1) << 2))
        //~ {
            //~ int rowCount = 0;
            //~ int x = minx;
            //~ for(; x <= maxx; x++, ptr += 4)
            //~ {
                //~ int t0 = ptr[0], t1 = ptr[1], t2 = ptr[2];
                //~ if(tab[t0 - c0 + 255] +tab[t1 - c1 + 255] + tab[t2 - c2 + 255] <= isr2)
                //~ {
                    //~ s0 += t0;
                    //~ s1 += t1;
                    //~ s2 += t2;
                    //~ sx += x;
                    //~ rowCount++;
                //~ }
            //~ }
            //~ if(rowCount == 0)
            //~ {
                //~ continue;
            //~ }
            //~ count += rowCount;
            //~ sy += y * rowCount;
        //~ }
        //~ if( count == 0 )
        //~ {
            //~ break;
        //~ }

        //~ int x1 = sx / count;
        //~ int y1 = sy / count;
        //~ s0 = s0 / count;
        //~ s1 = s1 / count;
        //~ s2 = s2 / count;

        //~ bool stopFlag = (x0 == x1 && y0 == y1) || (abs(x1 - x0) + abs(y1 - y0) +
                        //~ tab[s0 - c0 + 255] + tab[s1 - c1 + 255] + tab[s2 - c2 + 255] <= eps);

        //~ /****************************************************************************
        //~ * Revise the pointer corresponding to the new (y0,x0)                       *
        //~ ****************************************************************************/
        //~ //
        //~ revx = x1 - x0;
        //~ revy = y1 - y0;

        //~ x0 = x1;
        //~ y0 = y1;
        //~ c0 = s0;
        //~ c1 = s1;
        //~ c2 = s2;

        //~ if( stopFlag )
        //~ {
            //~ break;
        //~ }

    //~ }

    //~ dptr[0] = (uchar)c0;
    //~ dptr[1] = (uchar)c1;
    //~ dptr[2] = (uchar)c2;
    //~ dptr[3] = (uchar)c3;
//~ }

//~ /**
//~ *******************************************************************************
//~ * Implementation of CartoonFilter::verifyMeanShiftFilterOnCPU                 *
//~ ******************************************************************************/
//~ void CartoonFilter::verifyMeanShiftFilterOnCPU(const cv::Mat src, cv::Mat &dst,
        //~ int sp, int sr, cv::TermCriteria crit)
//~ {
    //~ if (src.empty())
    //~ {
        //~ cout << "Source is null" << endl;
    //~ }

    //~ if( !(crit.type & cv::TermCriteria::MAX_ITER) )
    //~ {
        //~ crit.maxCount = 5;
    //~ }
    //~ int maxIter = std::min(std::max(crit.maxCount, 1), 100);
    //~ float eps;
    //~ if( !(crit.type & cv::TermCriteria::EPS) )
    //~ {
        //~ eps = 1.f;
    //~ }
    //~ eps = (float)std::max(crit.epsilon, 0.0);

    //~ int tab[512];
    //~ for(int i = 0; i < 512; i++)
    //~ {
        //~ tab[i] = (i - 255) * (i - 255);
    //~ }
    //~ uchar *sptr = src.data;
    //~ uchar *dptr = dst.data;
    //~ int sstep = (int)src.step;
    //~ int dstep = (int)dst.step;
    //~ cv::Size size = src.size();

    //~ for(int i = 0; i < size.height; i++, sptr += sstep - (size.width << 2),
            //~ dptr += dstep - (size.width << 2))
    //~ {
        //~ for(int j = 0; j < size.width; j++, sptr += 4, dptr += 4)
        //~ {
            //~ do_meanShiftOnCPU(j, i, sptr, dptr, sstep, size, sp,
                              //~ sr, maxIter, eps, tab);
        //~ }
    //~ }
//~ }

//~ /**
//~ *******************************************************************************
//~ * Implementation of CartoonFilter::doPixelComparision_canny                   *
//~ ******************************************************************************/
//~ int CartoonFilter::doPixelComparision_canny(const cv::Mat src1,
        //~ const cv::Mat src2)
//~ {
    //~ /****************************************************************************
    //~ * Assert on the following.                                                  *
    //~ ****************************************************************************/
    //~ if (src1.size() != src2.size() || src1.type() != src2.type())
    //~ {
        //~ cout << "The sources are not equal to compare!" << endl;
        //~ return SDK_FAILURE;
    //~ }

    //~ if (src1.channels() != 3 && src2.channels() != 3)
    //~ {
        //~ cout << "Not a 3 channel image" << endl;
        //~ return SDK_FAILURE;
    //~ }

    //~ /****************************************************************************
    //~ * Loop through every pixel of both images and compare.                      *
    //~ ****************************************************************************/
    //~ int total_failures = 0;
    //~ cv::Vec3b black(0, 0, 0);
    //~ for (int i = 0; i < src1.rows; i++)
    //~ {
        //~ for (int j = 0; j < src1.cols; j++)
        //~ {
            //~ cv::Vec3b c1 = src1.at<cv::Vec3b>(i, j);
            //~ cv::Vec3b c2 = src2.at<cv::Vec3b>(i, j);
            //~ // int x;  x = c[0];  cout << c << " ";

            //~ if (c1 != c2)
            //~ {
                //~ // if src2 pixel is black, then no problem.
                //~ if (c2 == black)
                //~ {
                    //~ continue;
                //~ }
                //~ else
                //~ {
                    //~ // src2 pixel is non-black, so check for surrounding
                    //~ // 8 pixels in 3x3 matrix around the pixel to be black.
                    //~ int rowLow = i - 1, rowHigh = i + 1, colLow = j - 1, colHigh = j + 1;
                    //~ if (rowLow < 0)
                    //~ {
                        //~ rowLow = 0;
                    //~ }
                    //~ if (rowHigh >= src1.rows)
                    //~ {
                        //~ rowHigh = src1.rows - 1;
                    //~ }
                    //~ if (colLow < 0)
                    //~ {
                        //~ colLow = 0;
                    //~ }
                    //~ if (colHigh >= src1.cols)
                    //~ {
                        //~ colHigh = src1.cols - 1;
                    //~ }
                    //~ bool blackPxFound = false;
                    //~ for (int k = rowLow; k <= rowHigh; k++)
                    //~ {
                        //~ for (int l = colLow; l <= colHigh; l++)
                        //~ {
                            //~ cv::Vec3b surroundPx = src2.at<cv::Vec3b>(k, l);
                            //~ if (surroundPx == black)
                            //~ {
                                //~ k = rowHigh;
                                //~ blackPxFound = true;
                                //~ break;
                            //~ }
                        //~ }
                    //~ }// end for-k
                    //~ /********************************************************************
                    //~ * The match did not happen.                                         *
                    //~ ********************************************************************/
                    //~ if (!blackPxFound)
                    //~ {
                        //~ total_failures++;
                    //~ }
                //~ }// end if-else
            //~ }// end if
        //~ }
    //~ }// end for-i,j
    //~ if (total_failures > ((src1.rows * src1.cols) / 100) )
    //~ {
        //~ cout << total_failures << " pixel mismatches." << endl;
        //~ cout << "Failed!" << endl;
        //~ return SDK_FAILURE;
    //~ }
    //~ cout << endl << "Passed!" << endl;
    //~ return SDK_SUCCESS;
//~ }


//~ /**
//~ *******************************************************************************
//~ * Implementation of CartoonFilter::CPURefCartoonFilter()                      *
//~ ******************************************************************************/
//~ void CartoonFilter::CPURefCartoonFilter(cv::Mat cvSrcCPU, cv::Mat &cvDstCPU)
//~ {
    //~ cv::Mat cvSaveSrcCPU, cvGray, cvCannyMat;
    //~ cvSaveSrcCPU = cvSrcCPU;

    //~ /****************************************************************************
    //~ * Mean Shift Filter.                                                        *
    //~ ****************************************************************************/
    //~ cv::cvtColor(cvSrcCPU.clone(), cvSrcCPU, CV_BGR2BGRA);

    //~ verifyMeanShiftFilterOnCPU(cvSrcCPU.clone(), cvSrcCPU, 10, 30);

    //~ cv::cvtColor(cvSrcCPU.clone(), cvSrcCPU, CV_BGRA2BGR);

    //~ /****************************************************************************
    //~ * Canny.                                                                    *
    //~ ****************************************************************************/
    //~ cv::cvtColor(cvSaveSrcCPU, cvGray, CV_BGR2GRAY);
    //~ cv::Canny(cvGray, cvCannyMat, 150, 150);
    //~ cv::cvtColor(cvCannyMat, cvDstCPU, CV_GRAY2BGR);

    //~ cvDstCPU = cvSrcCPU - cvDstCPU;
//~ }

//~ /**
//~ *******************************************************************************
//~ * Implementation of CartoonFilter::verifyResults()                            *
//~ ******************************************************************************/
//~ int CartoonFilter::verifyResults()
//~ {
    //~ if (!sampleArgs->verify)
    //~ {
        //~ return SDK_SUCCESS;
    //~ }
    //~ cout << "Verifying...";

    //~ /****************************************************************************
    //~ * cvDstMat - the result obtained from run() on GPU.                         *
    //~ ****************************************************************************/
    //~ cv::cvtColor(cvDstMat.clone(), cvDstMat, CV_BGRA2BGR);

    //~ /****************************************************************************
    //~ * get CPU reference cartoon filtered image.                                 *
    //~ ****************************************************************************/
    //~ CPURefCartoonFilter(cvFrameCPU, cvDstCPU);
    //~ cv::cvtColor(cvDstCPU.clone(), cvDstCPU, CV_BGRA2BGR);
    //~ if (!sampleArgs->quiet)
    //~ {
        //~ // display CPU Reference Cartoon image
        //~ cv::namedWindow("CPURefCartoon");
        //~ cv::imshow("CPURefCartoon", cvDstCPU);
    //~ }
    //~ // Write to disk.
    //~ imwrite("Output_CPURefCartoon.jpg", cvDstCPU);

    //~ /****************************************************************************
    //~ * cv::ocl::canny and cv::canny results vary; so the following function      *
    //~ * gives a relaxation between cvDstMat and cvDstCPU during verification      *
    //~ * by doing pixel comparision.
    //~ ****************************************************************************/
    //~ return doPixelComparision_canny(cvDstMat, cvDstCPU);
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::RGB2Gray_caller()                          *
//~ ******************************************************************************/
//~ void CartoonFilter::RGB2Gray_caller(const oclMat &oclSrc, oclMat &oclDst,
                                    //~ int bidx)
//~ {
    //~ int channels = oclSrc.oclchannels();
    //~ size_t globalThreads[3] = {oclSrc.cols, oclSrc.rows, 1};
    //~ size_t localThreads[3] = {16, 16, 1};
    //~ char build_options[50];
    //~ sprintf (build_options, "-DDEPTH_%d", oclSrc.depth());

    //~ // args
    //~ vector<pair<size_t, const void *> > args;
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&oclSrc.cols));
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&oclSrc.rows));
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&oclSrc.step));
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&oclDst.step));
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&channels));
    //~ args.push_back(make_pair(sizeof(cl_int), (void *)&bidx));
    //~ args.push_back(make_pair(sizeof(cl_mem), (void *)&oclSrc.data));
    //~ args.push_back(make_pair(sizeof(cl_mem), (void *)&oclDst.data));

   
	//~ // convert kernel file into string

	//~ string sourceStr;
	//~ int status = convertToString(kernelFile, sourceStr);
	//~ const char *source = sourceStr.c_str();

	//~ program_src1.programStr = source;

	//~ openCLExecuteKernelInterop(oclSrc.clCxt, program_src1, "RGB2Gray",
                               //~ globalThreads, localThreads, args, -1, -1, build_options);
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::Gray2RGB_caller()                          *
//~ ******************************************************************************/
//~ void CartoonFilter::Gray2RGB_caller(const oclMat &oclSrc, oclMat &oclDst)
//~ {
    //~ size_t globalThreads[3] = {oclSrc.cols, oclSrc.rows, 1};
    //~ size_t localThreads[3] = {16, 16, 1};
    //~ char build_options[50];
    //~ sprintf (build_options, "-DDEPTH_%d", oclSrc.depth());

    //~ // args
    //~ vector<pair<size_t, const void *> > args;
    //~ args.push_back( make_pair( sizeof(cl_int) , (void *)&oclSrc.cols));
    //~ args.push_back( make_pair( sizeof(cl_int) , (void *)&oclSrc.rows));
    //~ args.push_back( make_pair( sizeof(cl_int) , (void *)&oclSrc.step));
    //~ args.push_back( make_pair( sizeof(cl_int) , (void *)&oclDst.step));
    //~ args.push_back( make_pair( sizeof(cl_mem) , (void *)&oclSrc.data));
    //~ args.push_back( make_pair( sizeof(cl_mem) , (void *)&oclDst.data));
  
	//~ // convert kernel file into string
	//~ string sourceStr;
	//~ int status = convertToString(kernelFile, sourceStr);
	//~ const char *source = sourceStr.c_str();

	//~ program_src2.programStr = source; 


	//~ openCLExecuteKernelInterop(oclSrc.clCxt, program_src2,  "Gray2RGB",
                               //~ globalThreads, localThreads, args, -1, -1, build_options);
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::myCvtColor()                               *
//~ ******************************************************************************/
//~ void CartoonFilter::myCvtColor(const oclMat &oclSrc, oclMat &oclDst, int code)
//~ {
    //~ Size size = oclSrc.size();
    //~ int depth = oclSrc.depth();
    //~ int channels = oclSrc.oclchannels();

    //~ CV_Assert(depth == CV_8U || depth == CV_16U || depth == CV_32F);

    //~ switch(code)
    //~ {
    //~ case CV_BGR2GRAY:
    //~ case CV_BGRA2GRAY:
    //~ {
        //~ CV_Assert(channels == 3 || channels == 4);
        //~ int bidx = ((code == CV_BGR2GRAY || CV_BGRA2GRAY) ? 0 : 2);
        //~ oclDst.create(size, CV_MAKETYPE(depth, 1));
        //~ RGB2Gray_caller(oclSrc, oclDst, bidx);
        //~ break;
    //~ }
    //~ case CV_GRAY2BGR:
    //~ case CV_GRAY2BGRA:
    //~ {
        //~ int dcn = (code == CV_GRAY2BGRA ? 4 : 3);
        //~ oclDst.create(size, CV_MAKETYPE(depth, dcn));
        //~ Gray2RGB_caller(oclSrc, oclDst);
        //~ break;
    //~ }
    //~ default:
        //~ CV_Error(SDK_FAILURE, "No convert rule");
        //~ return;
    //~ }
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::initialize()                               *
//~ ******************************************************************************/
//~ int CartoonFilter::initialize()
//~ {
    //~ if (sampleArgs->initialize())
    //~ {
        //~ return SDK_FAILURE;
    //~ }

    //~ Option* imageFileName = new Option;
    //~ CHECK_ALLOCATION(imageFileName, "Memory allocation error.\n");

    //~ imageFileName->_sVersion = "f";
    //~ imageFileName->_lVersion = "imageFile";
    //~ imageFileName->_description = "Input image file name.";
    //~ imageFileName->_type = CA_ARG_STRING;
    //~ imageFileName->_value = &imageFile;

    //~ sampleArgs->AddOption(imageFileName);
    //~ delete imageFileName;

    //~ Option* iteration_option = new Option;
    //~ CHECK_ALLOCATION(iteration_option, "Memory allocation error.\n");

    //~ iteration_option->_sVersion = "i";
    //~ iteration_option->_lVersion = "iterations";
    //~ iteration_option->_description =
        //~ "Number of iterations for kernel execution.";
    //~ iteration_option->_type = CA_ARG_INT;
    //~ iteration_option->_value = &sampleArgs->iterations;

    //~ sampleArgs->AddOption(iteration_option);
    //~ delete iteration_option;

    //~ return SDK_SUCCESS;
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::setUp()                                    *
//~ ******************************************************************************/
//~ int CartoonFilter::setUp()
//~ {
  	//~ int devnums = cv::ocl::getOpenCLDevices(oclinfo, CVCL_DEVICE_TYPE_CPU); 

    //~ if(devnums < 1)
    //~ {
        //~ std::cout << "Device not found!" << std::endl;
        //~ return SDK_FAILURE;
    //~ }

    //~ IplImage* iplImg = cvLoadImage(imageFile.c_str());
    //~ cvFrameCPU = iplImg;
    //~ if( cvFrameCPU.empty() )
    //~ {
        //~ std::cout << "Unable to open file: " << imageFile << std::endl;
        //~ return SDK_FAILURE;
    //~ }

    //~ if (sampleArgs->iterations < 1)
    //~ {
        //~ std::cout << "Iterations cannot be 0 or negative!" << std::endl;
        //~ return SDK_FAILURE;
    //~ }
    //~ return SDK_SUCCESS;
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::showInterops()                             *
//~ ******************************************************************************/
//~ int CartoonFilter::showInterops()
//~ {
    //~ cv::ocl::oclMat oclCloneMat;

    //~ /****************************************************************************
    //~ * Mean shift filter.                                                        *
    //~ ****************************************************************************/
    //~ cv::cvtColor(cvFrameCPU, cvSrcMat, CV_BGRA2BGR);
    //~ oclSrcMat = cvSrcMat;  // Interop: OpenCV to OpenCV-CL.

    //~ oclCloneMat = oclSrcMat.clone();

    //~ cv::ocl::meanShiftFiltering(oclCloneMat, oclSrcMat, 10, 30);
    //~ cvDstMat = oclSrcMat;  // Interop: OpenCV-CL to OpenCV.

    //~ /****************************************************************************
    //~ * Canny.                                                                    *
    //~ ****************************************************************************/
    //~ oclCannyMat = cvFrameCPU;
    //~ // Interop: OpenCV to Raw OpenCL kernel.
    //~ myCvtColor(oclCannyMat.clone(), oclCannyMat, CV_BGRA2GRAY);

    //~ oclCloneMat = oclCannyMat.clone();// Interop: Raw OpenCL kernel to OpenCV-CL.
    //~ cv::ocl::Canny(oclCloneMat, oclCannyMat, 150, 150);

    //~ // Interop: OpenCV-CL to Raw OpenCL kernel.
    //~ myCvtColor(oclCannyMat.clone(), oclCannyMat, CV_GRAY2BGRA);
    //~ cvCannyDstMat = oclCannyMat;  // Interop: Raw OpenCL kernel to OpenCV.

    //~ // diff
    //~ cvDstMat = cvDstMat - cvCannyDstMat;

    //~ return SDK_SUCCESS;
//~ }

//~ /******************************************************************************
//~ * Implementation of CartoonFilter::run()                                      *
//~ ******************************************************************************/
//~ int CartoonFilter::run()
//~ {
    //~ if (!sampleArgs->quiet)
    //~ {
        //~ std::cout << "========================================" << std::endl;
        //~ std::cout << "Introducing Interop using Cartoon Filter" << std::endl;
        //~ std::cout << "========================================" << std::endl;
    //~ }

    //~ /****************************************************************************
    //~ * Warm-up runs.                                                             *
    //~ ****************************************************************************/
    //~ if (sampleArgs->timing)
    //~ {
        //~ for (int i = 0; i < 2 && sampleArgs->iterations != 1; i++)
        //~ {
            //~ if (showInterops() != SDK_SUCCESS)
            //~ {
                //~ return SDK_FAILURE;
            //~ }
        //~ }
    //~ }

    //~ /****************************************************************************
    //~ * Execute filter for `iterations' times.                                    *
    //~ ****************************************************************************/
    //~ if (!sampleArgs->quiet)
    //~ {
        //~ std::cout << std::endl << "Executing kernel for " << sampleArgs->iterations
                  //~ << " iteration(s):" << std::endl;
        //~ std::cout << "------------------------------------" << std::endl;
    //~ }

    //~ if(sampleArgs->timing)
    //~ {
        //~ timer = sampleTimer->createTimer();
        //~ sampleTimer->resetTimer(timer);
        //~ sampleTimer->startTimer(timer);
    //~ }

    //~ for (int i = 0; i < sampleArgs->iterations; i++)
    //~ {
        //~ if (showInterops() != SDK_SUCCESS)
        //~ {
            //~ return SDK_FAILURE;
        //~ }
    //~ }

    //~ if (sampleArgs->timing)
    //~ {
        //~ sampleTimer->stopTimer(timer);
        //~ kernelTime = sampleTimer->readTimer(timer) / sampleArgs->iterations;

        //~ std::cout << std::endl;
        //~ std::cout << "Width x Height\t" << "Avg. time / kernel execution\t";
        //~ std::cout << "Pixels / sec" << std::endl;
        //~ std::cout << "-----\t" << "------\t" << "----------------------------\t";
        //~ std::cout << "------------" << std::endl;

        //~ std::cout << "  " << cvFrameCPU.cols << " x " << cvFrameCPU.rows << "\t";
        //~ std::cout << "       " << kernelTime << "             " << "\t";
        //~ std::cout << "  " << (cvFrameCPU.cols * cvFrameCPU.rows) / kernelTime;
        //~ std::cout << std::endl;
    //~ }

    //~ return SDK_SUCCESS;
//~ }

#include "opencv2/imgproc/imgproc.hpp"

#define HU_MOMENTS_NUM 7

using namespace cv;

// TODO: move to separate file included by all modules working with Hu moments
typedef std::vector<float> hu_moments_t;

const int threshold = 150;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
      printf("Usage: ./moments input_png [init_canny_threshold]\n");
      exit(1);
    }

    if(argc == 3)
    {
      thresh = atoi(argv[2]);
    }

    Mat src_image;
   
    src_image = imread( argv[1], 1 );
    hu_moments_t hu_moments = hu_moments_compute(src_image, threshold);

    std::cout << "Hu moments: " << print_vector(hu_moments) << std::endl;

    return 0;
}

hu_moments_t hu_moments_compute(Mat& src, const int thresh)
{
    Mat gray_image;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    // Detect edges using Canny
    Mat edges_image;
    // TODO: do not use magic values
    Canny( gray, edges_image, thresh, thresh*2, 3 );
    
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(edges_image, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); ++i)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);
        // Ignore contours that are too small or too large
        if (area < 1e3 || area > 1e6) continue;

        printf("area = %f for contour %lu\n", area, i);

        // Compute Hu moments of a single contour
        Moments mu = moments( roi, false );
        double hu[HU_MOMENTS_NUM];
        HuMoments(mu, hu);
        printf("Hu invariants for contour %zu:\n", i);
        for( int j = 0; j < HU_MOMENTS_NUM; j++ )
            printf("[%d]=%.4e ", j+1, hu[j]);
        printf("\n");

    }
    hu_moments_t hu_vector;
    hu_vector.assign(hu, hu + HU_MOMENTS_NUM);
    return hu_vector;
}


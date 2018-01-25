#ifndef CENTER_OF_MASS_H_
#define CENTER_OF_MASS_H_

#include <opencv2/core/core.hpp>

float center_of_mass_position_compute(const cv::Mat& src,
                                      const int canny_threshold,
                                      const int upper_threshold,
                                      int& label,
                                      const int sobel_kernel_size,
                                      const bool label_images,
                                      const bool show_images,
                                      bool& acorn_detected);

#endif /* CENTER_OF_MASS_H_ */


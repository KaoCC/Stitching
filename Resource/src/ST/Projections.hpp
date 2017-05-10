#ifndef _ST_PROJECTIONS_HPP_
#define _ST_PROJECTIONS_HPP_


#include <opencv2/opencv.hpp>
#include <cmath>

namespace ST {


	cv::Mat cylinderProjection(const cv::Mat& image, double focalLength, bool grayScaleFlag); 
	



}






#endif


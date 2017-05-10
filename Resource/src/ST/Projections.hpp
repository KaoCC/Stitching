#ifndef _ST_PROJECTIONS_HPP_
#define _ST_PROJECTIONS_HPP_


#include <opencv2/opencv.hpp>
#include <cmath>

namespace ST {


	cv::Mat cylinderProjection(const cv::Mat& image, double focalLength) {

		double hWidth = (double)image.size().width / 2;
		double hHeight = (double)image.size().height / 2;


		size_t imgWidth = focalLength * std::atan(hWidth / focalLength);
		size_t imgHeight = hHeight;

		cv::Mat projectionImage (cv::Size(imgWidth * 2, imgHeight * 2), CV_8UC3);

		
		for (size_t j = 0; j < image.size().height; ++j) {
			for (size_t i = 0; i < image.size().width; ++i) {


				// yet to be done 



			}
		}





		return projectionImage;

	}


}






#endif


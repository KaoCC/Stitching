#include "Projections.hpp"

namespace ST {

	cv::Mat cylinderProjection(const cv::Mat & image, double focalLength, bool grayScaleFlag) {

		double hWidth = static_cast<double>(image.size().width / 2);
		double hHeight = static_cast<double>(image.size().height / 2);

		std::cerr << hWidth << " " << hHeight << '\n';

		// x, y
		double imgWidth = focalLength * std::atan(hWidth / focalLength);
		double imgHeight = hHeight;

		std::cerr << imgWidth << " " << imgHeight << '\n';

		// width might be wrong !
		cv::Mat projectionImage(cv::Size((int)((imgWidth + 1) * 2), (int)(imgHeight * 2)), CV_8UC3);
		//cv::Mat projectionImage(cv::Size(image.size()), CV_8UC3);

		for (int j = 0; j < image.size().height; ++j) {
			for (int i = 0; i < image.size().width; ++i) {



				int xIndex = static_cast<int>(focalLength * std::atan((i - hWidth) / focalLength) + imgWidth);
				int yIndex = static_cast<int>(focalLength * (j - hHeight) / std::sqrt((i - hWidth) * (i - hWidth) + focalLength * focalLength) + imgHeight);

				//std::cerr << yIndex << " " << xIndex << '\n';

				// move pixels
				projectionImage.at<cv::Vec3b>(yIndex, xIndex) = image.at<cv::Vec3b>(j, i);
				//projectionImage.at<cv::Vec3b>(yIndex, xIndex) = 0;

			}
		}



		if (grayScaleFlag) {
			cv::cvtColor(projectionImage, projectionImage, cv::COLOR_BGR2GRAY);
		}

		cv::imshow("proj", projectionImage);
		//cv::imshow("orig", image);
		cv::waitKey(0);

		return projectionImage;

	}

}
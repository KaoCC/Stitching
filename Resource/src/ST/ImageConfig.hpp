#ifndef _ST_IMAGECONFIG_HPP_
#define _ST_IMAGECONFIG_HPP_


#include <string>
#include <vector>
#include <array>

#include <opencv2/opencv.hpp>

namespace ST {

	class KeyPoint;

	class ImageConfig {

	public:

		static const size_t kScaleSize = 5;

		void load(const std::string fileName, double focalLength);
		std::vector<KeyPoint> computeKeyPoints();

		const cv::Mat& getOriginalImage() const;



		// test
		//void testKeyPoints(const std::vector<KeyPoint>& keyPoints);

	private:
		cv::Mat mOriginalImage;
		std::array<cv::Mat, kScaleSize> mScaledImages;
		std::array<cv::Mat, kScaleSize> mFHM;

	};



	void loadImageConfigs(const std::string& basePath, const std::string& fileName, std::vector<ImageConfig>& images);

}

















#endif

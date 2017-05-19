#ifndef _ST_IMAGECONFIG_HPP_
#define _ST_IMAGECONFIG_HPP_


#include <string>
#include <vector>
#include <array>

#include <opencv2/opencv.hpp>

#include "AffineData.hpp"

namespace ST {

	class KeyPoint;

	class ImageConfig {

	public:

		static const size_t kScaleSize = 5;

		void load(const std::string fileName, double focalLength);
		std::vector<KeyPoint> computeKeyPoints();

		const cv::Mat& getOriginalImage() const;

		const cv::Mat& getScaledImages(std::size_t scaleIndex) const {
			return mScaledImages[scaleIndex];
		}

		const cv::Mat& getFHM(std::size_t scaleIndex) const {
			return mFHM[scaleIndex];
		}

	private:
		cv::Mat mOriginalImage;
		std::array<cv::Mat, kScaleSize> mScaledImages;
		std::array<cv::Mat, kScaleSize> mFHM;

	};



	void loadImageConfigs(const std::string& basePath, const std::string& fileName, std::vector<ImageConfig>& images);
	void testKeyPoints(const ST::ImageConfig& image, const std::vector<ST::KeyPoint>& keyPoints, std::string name);

	cv::Mat stitch(const std::vector<ST::ImageConfig>& imageConfigs, const std::vector<ST::AffineData>& affines);

}

















#endif

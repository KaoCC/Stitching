#include "ImageConfig.hpp"


#include <sstream>
#include <fstream> 

#include <iostream>

#include "Projections.hpp"

namespace ST {

	void loadImageConfigs(const std::string & basePath, const std::string & fileName, std::vector<ImageConfig>& images) {

		const std::string fullPath = basePath + fileName;

		std::ifstream inputStream(fullPath);

		if (!inputStream) {
			throw std::runtime_error("Could not open file: " + fullPath);
		}

		std::string lineString;


		bool numFlag = false;

		std::size_t numOfImages = 0;
		std::size_t currentImageIndex = 0;

		while (std::getline(inputStream, lineString)) {

			std::stringstream lineSplitter(lineString);
			char token;

			lineSplitter >> token;
			if ((!lineSplitter) || token == '#') {
				continue;
			} else {
				lineSplitter.putback(token);
			}



			if (!numFlag) {

				std::size_t num = 0;
				lineSplitter >> num;

				// first number
				numOfImages = num;
				std::cout << "Number of image: " << numOfImages << '\n';
				numFlag = true;

				images.resize(numOfImages);


			} else {

				std::string imageFileName;


				lineSplitter >> imageFileName; // >> invShutterSpeed;

				std::cout << "file: " << imageFileName << std::endl;


				// read image
				// KAOCC: focal length is wrong !
				images[currentImageIndex++].load(basePath + imageFileName, 1000);

				if (currentImageIndex == numOfImages) {
					break;
				}

			}


		}


	}

	void ImageConfig::load(const std::string fileName, double focalLength) {

		// original
		mOriginalImage = cv::imread(fileName);

		// scaled images
		mScaledImages[0] = cylinderProjection(mOriginalImage, focalLength, true);
		for (size_t i = 1; i < kScaleSize; ++i) {

			//mScaledImages[i] = cv::Mat(mScaledImages[i - 1].size().width / 2, mScaledImages[i - 1].size().height/ 2, CV_8U);
			cv::resize(mScaledImages[i - 1], mScaledImages[i], cv::Size(mScaledImages[i - 1].size().width / 2, mScaledImages[i - 1].size().height / 2));
			//cv::imshow("proj", mScaledImages[i - 1]);
			//cv::imshow("orig", mScaledImages[i]);
			//cv::waitKey(0);
		}

	}

	std::vector<KeyPoint> ImageConfig::computeKeyPoints() {

		// yet to be done

		return std::vector<KeyPoint>();
	}

	cv::Mat & ImageConfig::getOriginalImage() {
		return mOriginalImage;
	}


}
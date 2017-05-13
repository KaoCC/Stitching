

#include <vector>
#include <string>

#include "ImageConfig.hpp"
#include "KeyPoint.hpp"
#include "Projections.hpp"

const std::string kDefaultBasePath = "../Resource/input_image/";
const std::string kDefaultFileList = "list.txt";


// tmp
static std::vector<ST::KeyPoint> nonMaxSuppression(const std::vector<ST::KeyPoint>& originalFeatures) {

	std::vector<ST::KeyPoint> finalResult;

	double radius = 10000;

	// sort it !

	while (finalResult.size() < 500) {

		std::vector<bool> suppressionFlags(originalFeatures.size(), false);

		// loop over the KeyPoints
		for (int i = 0; i < originalFeatures.size(); ++i) {

			if (suppressionFlags[i] != true) {

				// Mark the rest of it
				for (int j = i + 1; j < originalFeatures.size(); ++j) {
					if (originalFeatures[i].getValue() == originalFeatures[j].getValue()) {
						suppressionFlags[j] = true;
					}

				}

				// Add KeyPoint to the final result
				finalResult.push_back(originalFeatures[i]);

				if (finalResult.size() >= 500) {
					break;
				}

			}

		}


		radius *= 0.5;

	}



	return finalResult;
}


static void testKeyPoints(const ST::ImageConfig& image, const std::vector<ST::KeyPoint>& keyPoints) {
	std::cerr << keyPoints.size() << std::endl;

	//cv::Mat result(cv::Size(mScaledImages[0].size()), CV_8UC3);

	cv::Mat result = ST::cylinderProjection(image.getOriginalImage(), 1000, false);

	//cv::cvtColor(mScaledImages[0], result, cv::COLOR_GRAY2BGR);

	for (int i = 0; i < keyPoints.size(); ++i) {
		int radius = 16 >> (4 - keyPoints[i].getScale());
		cv::circle(result, cvPoint(keyPoints[i].getX() * radius, keyPoints[i].getY() * radius), radius * 4, CV_RGB(255, 0, 0));
	}


	cv::imshow("result", result);
	cv::waitKey(0);
}


int main(int argc, char* argv[]) {

	std::string basePath;
	if (argc > 1) {
		basePath = argv[1];
	} else {
		basePath = kDefaultBasePath;
	}

	std::string fileList;
	if (argc > 2) {
		fileList = argv[2];
	} else {
		fileList = kDefaultFileList;
	}

	std::vector<ST::ImageConfig> images;
	ST::loadImageConfigs(basePath, fileList, images);



	// compute features
	std::vector<std::vector<ST::KeyPoint>> features;
	for (auto& image : images) {
		features.push_back(image.computeKeyPoints());
	}


	std::cerr << "computeKeyPoints\n";

	// test
	for (int i = 0; i < images.size(); ++i) {
		testKeyPoints(images[i], features[i]);
	}

	// non max suppression
	//std::vector<std::vector<ST::KeyPoint>> FinalResults;
	//for (auto& feature : features) {
	//	nonMaxSuppression(feature);
	//}


	//for (int i = 0; i < images.size(); ++i) {
	//	testKeyPoints(images[i], FinalResults[i]);
	//}

	// SubPixel Refinement

	// Matching




	return 0;
}



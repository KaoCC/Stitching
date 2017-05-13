

#include <vector>
#include <string>

#include "ImageConfig.hpp"
#include "KeyPoint.hpp"
#include "Projections.hpp"

const std::string kDefaultBasePath = "../Resource/input_image/";
const std::string kDefaultFileList = "list.txt";


// tmp

const std::size_t kMaxNumOfPoints = 500;

// tmp
static std::vector<ST::KeyPoint> nonMaxSuppression(std::vector<ST::KeyPoint>& originalFeatures) {

	std::vector<ST::KeyPoint> finalResult;

	double radius = 20;

	// sort it ! (?)
	std::sort(originalFeatures.begin(), originalFeatures.end(), ST::KeyPoint::compare);

	std::size_t currentNumOfValid = originalFeatures.size();
	std::vector<bool> validFlags(originalFeatures.size(), true);

	while (currentNumOfValid > kMaxNumOfPoints) {

		double radiusSquare = radius * radius;

		for (int i = 0; i < originalFeatures.size(); ++i) {

			if (!validFlags[i]) {
				continue;
			}


			for (int j = i + 1; j < originalFeatures.size(); ++j) {

				if (ST::KeyPoint::computeDistanceSquare(originalFeatures[i], originalFeatures[j]) < radiusSquare) {
					validFlags[j] = false;
				}

				// always true since it is sorted
				//if (originalFeatures[i].getValue() > originalFeatures[j].getValue()) {
				//	validFlags[j] = false;
				//} else {
				//	validFlags[i] = false;
				//	break;
				//}

			}


		}

		currentNumOfValid = std::count(validFlags.begin(), validFlags.end(), true);
		std::cerr << "Valid: " << currentNumOfValid << " Rad: " << radius << '\n';

		radius += 10;

	}

	std::cerr << "Add\n";

	for (size_t i = 0; i < originalFeatures.size(); ++i) {
		if (validFlags[i]) {
			finalResult.push_back(originalFeatures[i]);
		}
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
	//for (int i = 0; i < images.size(); ++i) {
	//	testKeyPoints(images[i], features[i]);
	//}

	// non max suppression
	std::vector<std::vector<ST::KeyPoint>> FinalResults;
	for (auto& feature : features) {
		FinalResults.push_back(nonMaxSuppression(feature));
	}

	// test
	for (int i = 0; i < images.size(); ++i) {
		testKeyPoints(images[i], FinalResults[i]);
	}

	// SubPixel Refinement

	// Matching




	return 0;
}



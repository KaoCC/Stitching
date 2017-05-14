

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

				// KAOCC: do we need to consider the same scale value ? (if a.s == b.s ?)
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

// tmp
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

// tmp
static void subPixelRefinement(const ST::ImageConfig& image, std::vector<ST::KeyPoint>& features) {

	for (size_t i = 0; i < features.size(); ++i) {

		const auto& fHM = image.getFHM(features[i].getScale());

		std::array<std::array<double , 3>, 3> fValue;

		int mX = features[i].getX();
		int mY = features[i].getY();

		// skip !
		if (mX - 3 < 0 || mY - 3 < 0) {
			continue;
		}

		// (1, 1) is the original
		for (int ix = 0; ix < 3; ++ix) {
			for (int iy = 0; iy < 3; ++iy) {
				mX += (ix - 1);
				mY += (iy - 1);
				fValue[ix][iy] = fHM.at<double>(mY, mX);
			}

		}

		//X = (x, y)
		cv::Mat dfdX(2, 1, CV_64F);
		dfdX.at<double>(0, 0) = (fValue[2][1] - fValue[0][1]) / 2;
		dfdX.at<double>(1, 0) = (fValue[1][2] - fValue[1][0]) / 2;
		cv::Mat dfdX2(2, 2, CV_64F);
		dfdX2.at<double>(0, 0) = fValue[2][1] - 2 * fValue[1][1] + fValue[0][1];
		dfdX2.at<double>(0, 1) = (fValue[0][0] + fValue[2][2] - fValue[0][2] - fValue[2][0]) / 4;
		dfdX2.at<double>(1, 0) = (fValue[0][0] + fValue[2][2] - fValue[0][2] - fValue[2][0]) / 4;
		dfdX2.at<double>(1, 1) = fValue[1][2] - 2 * fValue[1][1] + fValue[1][0];

		cv::Mat delta = -dfdX2.inv() * dfdX;

		//std::cerr << delta.at <double>(0, 0) << std::endl;
		//std::cerr << delta.at <double>(1, 0) << std::endl;

		features[i].adjustX(delta.at <double>(0, 0));
		features[i].adjustY(delta.at <double>(1, 0));

	}

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
	std::vector<std::vector<ST::KeyPoint>> finalResults;
	for (auto& feature : features) {
		finalResults.push_back(nonMaxSuppression(feature));
	}

	// test
	for (int i = 0; i < images.size(); ++i) {
		testKeyPoints(images[i], finalResults[i]);
	}

	// SubPixel Refinement

	std::cerr << "SubPixel Refinement\n";
	for (int i = 0; i < images.size(); ++i) {
		subPixelRefinement(images[i], finalResults[i]);
	}

	// test
	for (int i = 0; i < images.size(); ++i) {
		testKeyPoints(images[i], finalResults[i]);
	}

	// Create Descriptor


	// Matching




	return 0;
}



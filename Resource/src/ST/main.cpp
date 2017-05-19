

#include <vector>
#include <string>

#include "ImageConfig.hpp"
#include "KeyPoint.hpp"
#include "Projections.hpp"

#include "DescriptorMSOP.hpp"
#include "SimpleMatcher.hpp"

const std::string kDefaultBasePath = "../Resource/input_image/";
const std::string kDefaultFileList = "list.txt";

const bool kWithTripod = true;

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
				if (originalFeatures[i].getScale() == originalFeatures[j].getScale() && ST::KeyPoint::computeDistanceSquare(originalFeatures[i], originalFeatures[j]) < radiusSquare) {
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
//static void testKeyPoints(const ST::ImageConfig& image, const std::vector<ST::KeyPoint>& keyPoints) {
//	std::cerr << keyPoints.size() << std::endl;
//
//	//cv::Mat result(cv::Size(mScaledImages[0].size()), CV_8UC3);
//
//	cv::Mat result = ST::cylinderProjection(image.getOriginalImage(), 10000, false);
//
//	//cv::cvtColor(mScaledImages[0], result, cv::COLOR_GRAY2BGR);
//
//	for (int i = 0; i < keyPoints.size(); ++i) {
//		int radius = 16 >> (4 - keyPoints[i].getScale());
//		cv::circle(result, cvPoint(keyPoints[i].getX() * radius, keyPoints[i].getY() * radius), radius * 4, CV_RGB(255, 0, 0));
//	}
//
//
//	cv::imshow("result", result);
//	cv::waitKey(0);
//}

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
		dfdX2.at<double>(1, 0) = dfdX2.at<double>(0, 1);
		dfdX2.at<double>(1, 1) = fValue[1][2] - 2 * fValue[1][1] + fValue[1][0];

		cv::Mat delta = -dfdX2.inv() * dfdX;

		//std::cerr << delta.at <double>(0, 0) << std::endl;
		//std::cerr << delta.at <double>(1, 0) << std::endl;

		features[i].adjustX(delta.at <double>(0, 0));
		features[i].adjustY(delta.at <double>(1, 0));

	}

}

//tmp
static void computeDescriptor(const ST::ImageConfig& image, std::vector<ST::KeyPoint>& features) {
	for (auto& feature : features) {
		feature.setDescriptor(ST::DescriptorMSOP::createDescriptorMSOP(image, feature));
	}
}


//test code 
static cv::Mat drift(int width, int height, const cv::Mat& affine, const cv::Mat& img) {

	double rawXY[3] = {
		width,
		0,
		1
	};

	cv::Mat originalXY (3, 1, CV_64F, rawXY);
	cv::Mat newXY = affine * originalXY;

	std::cerr << "test\n";

	double eleX = newXY.at<double>(0, 0);
	double eleY = newXY.at<double>(1, 0);

	int newWidth = std::sqrt(eleX * eleX + eleY * eleY);


	double rawXYbar[3] = {
		width,
		height,
		1
	};

	cv::Mat originalXYbar(3, 1, CV_64F, rawXYbar);
	cv::Mat newXYbar = affine * originalXYbar;

	std::cerr << "test2\n";

	double eleXbar = newXYbar.at<double>(0, 0);
	double eleYbar = newXYbar.at<double>(1, 0);

	cv::Point2f src[4] = {
		cv::Point2d(0, 0),
		cv::Point2d(0, height),
		cv::Point2d(eleX, eleY),
		cv::Point2d(eleXbar, eleYbar),
	};


	cv::Point2f dst[4] = {
		cv::Point2d(0, 0),
		cv::Point2d(0, height),
		cv::Point2d(newWidth, 0),
		cv::Point2d(newWidth, height),
	};

	cv::Mat perspective = cv::getPerspectiveTransform(src, dst);

	std::cerr << "perspective:" << perspective << std::endl;

	//cv::Mat newImage(cv::Size(newWidth, height), CV_8U);

	cv::Mat newImage;
	cv::warpPerspective(img, newImage, perspective, cv::Size(newWidth, height), CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	return newImage;
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
	//for (int i = 0; i < images.size(); ++i) {
	//	testKeyPoints(images[i], finalResults[i]);
	//}

	// SubPixel Refinement

	std::cerr << "SubPixel Refinement\n";
	for (int i = 0; i < images.size(); ++i) {
		subPixelRefinement(images[i], finalResults[i]);
	}

	//test
	//for (int i = 0; i < images.size(); ++i) {
	//	testKeyPoints(images[i], finalResults[i], std::to_string(i));
	//}

	// Create Descriptor
	for (int i = 0; i < images.size(); ++i) {
		computeDescriptor(images[i], finalResults[i]);
	}

	// Matching
	ST::SimpleMatcher matcher;

	std::vector<ST::MatchPairs> matchResults;
	for (int i = 1; i < images.size(); ++i) {
		matchResults.push_back(matcher.match(finalResults[i - 1], finalResults[i]));
	}

	// test
	for (const auto& result : matchResults) {
		std::cerr << result.size() << std::endl;
	}

	//tmp;

	//int ai = 0; int bi = ai + 1;
	//for (const auto& match : matchResults) {

	//	std::vector<ST::KeyPoint> aa;
	//	std::vector<ST::KeyPoint> bb;

	//	for (const auto& pair : match) {
	//		aa.push_back(*pair.first);
	//		bb.push_back(*pair.second);
	//	}

	//	testKeyPoints(images[ai], aa, "a");
	//	testKeyPoints(images[bi], bb, "b");
	//	cv::waitKey(0);

	//	++ai;
	//	bi = ai + 1;
	//}




	std::cerr << "Comptue Affine Mat\n";

	std::vector<ST::AffineData> affines;

	for (const auto& match : matchResults) {
		affines.push_back(matcher.computeAffine(match, kWithTripod));
	}

	std::cerr << "Affine SZ: " << affines.size() << std::endl;

	for (const auto& aff : affines) {
		std::cerr << "delta x: " <<aff.getDeltaX() << std::endl;
		std::cerr << "delta y: " << aff.getDeltaY() << std::endl;
		std::cerr << aff.getAffineMat() << std::endl;

	}

	// KAOCC: todo : load imag in color mode


	auto mappedImg = ST::stitch(images, affines);



	//std::cerr << "Drift" << std::endl;
	//cv::Mat driftImg = drift(mappedImgA.size().width, images[0].getScaledImages(0).size().height, Affines[images.size() - 2].getAffineMat() , mappedImgA);
	//cv::imshow("driftImg", driftImg);
	//cv::imwrite("driftImg.jpg", driftImg);

	cv::imwrite("mappedImg_Merged.jpg", mappedImg);
	cv::waitKey(0);

	return 0;
}



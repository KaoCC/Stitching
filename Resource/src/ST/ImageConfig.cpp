#include "ImageConfig.hpp"


#include <sstream>
#include <fstream> 

#include <iostream>

#include "Projections.hpp"

#include "KeyPoint.hpp"

// tmp
static const double kGSigP = 1.0;
static const double kGSigD = 1.0;
static const double kGSigI = 1.5;
static const double kGSigO = 4.5;

static const double kLength = 20000;

namespace ST {


	// 0: x, 1: y
	static void computeGradient(const cv::Mat& inputImage, std::array<cv::Mat, 2>& gradients) {

		cv::Mat& Dx = gradients[0];
		cv::Sobel(inputImage, Dx, CV_64F, 1, 0, 3);

		cv::Mat& Dy = gradients[1];
		cv::Sobel(inputImage, Dy, CV_64F, 0, 1, 3);

	}


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
				images[currentImageIndex++].load(basePath + imageFileName, kLength);

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

			cv::GaussianBlur(mScaledImages[i - 1], mScaledImages[i - 1], cv::Size(0, 0), kGSigP);

			//cv::imshow("proj", mScaledImages[i - 1]);
			//cv::imshow("orig", mScaledImages[i]);
			//cv::waitKey(0);
		}

	}

	std::vector<KeyPoint> ImageConfig::computeKeyPoints() {

		// yet to be done

		std::vector<KeyPoint> keyPoints;

		for (size_t index = 0; index < kScaleSize; ++index) {



			// .... lots of stuff here

			// Compute x and y derivatives of image

			//Gradient : Gx, Gy
			std::array<cv::Mat, 2> G;
			computeGradient(mScaledImages[index], G);

			//cv::imshow("G", G[1]);
			//cv::waitKey(0);

			// Gaussian: x, y
			std::array<cv::Mat, 2> Gg;
			for (size_t idx = 0; idx < G.size(); ++idx) {
				cv::GaussianBlur(G[idx], Gg[idx], cv::Size(0, 0), kGSigD);
			}

			// Ixx, Ixy, Iyy
			std::array<cv::Mat, 3> I { cv::Mat(mScaledImages[index].size(), CV_64F), cv::Mat(mScaledImages[index].size(), CV_64F), cv::Mat(mScaledImages[index].size(), CV_64F) };
			auto& Ixx = I[0];
			auto& Ixy = I[1];
			auto& Iyy = I[2];

			for (int y = 0; y < mScaledImages[index].size().height; ++y) {
				for (int x = 0; x < mScaledImages[index].size().width; ++x) {

					auto& ix = Gg[0].at<double>(y, x);
					auto& iy = Gg[1].at<double>(y, x);

					Ixx.at<double>(y, x) = ix * ix;
					Ixy.at<double>(y, x) = ix * iy;
					Iyy.at<double>(y, x) = iy * iy;
				}
			}

			std::array<cv::Mat, 3> H { cv::Mat(mScaledImages[index].size(), CV_64F),  cv::Mat(mScaledImages[index].size(), CV_64F), cv::Mat(mScaledImages[index].size(), CV_64F) };

			// compute Gaussian
			for (size_t idx = 0; idx < H.size(); ++idx) {
				cv::GaussianBlur(I[idx], H[idx], cv::Size(0, 0), kGSigI);
			}


			//cv::imshow("H", H[0]);
			//cv::waitKey(0);

			mFHM[index] = cv::Mat(mScaledImages[index].size(), CV_64F);

			for (int y = 0; y < mScaledImages[index].size().height; ++y) {
				for (int x = 0; x < mScaledImages[index].size().width; ++x) {

					double det = H[0].at<double>(y, x) * H[2].at<double>(y, x) - H[1].at<double>(y, x) * H[1].at<double>(y, x);
					double trace = H[0].at<double>(y, x) + H[2].at<double>(y, x);

					mFHM[index].at<double>(y, x) = det / (trace + 0.0001);		// tmp

				}
			}

			//cv::imshow("mFHM", mFHM[index]);
			//cv::waitKey(0);

			// Ori: x, y
			std::array<cv::Mat, 2> Go;
			for (size_t idx = 0; idx < G.size(); ++idx) {
				cv::GaussianBlur(G[idx], Go[idx], cv::Size(0, 0), kGSigO);
			}

			// Pick local maxima of 3x3 and larger than 10
			for (int y = 0; y < mFHM[index].size().height; ++y) {
				for (int x = 0; x < mFHM[index].size().width; ++x) {

					if (mFHM[index].at<double>(y, x) < 10) {
						continue;
					}

					// find local max
					bool maxFlag = true;
					for (int dy = -1; dy < 1; ++dy) {
						for (int dx = -1; dx < 1;  ++dx) {

							// check the boundary
							if ((dy == 0 && dx == 0) || (x + dx == mFHM[index].size().width) || (x + dx < 0)  || (y + dy == mFHM[index].size().height) || (y + dy < 0) ) {
								continue;
							}

							// not the local max ...
							if (mFHM[index].at<double>(y  + dy, x + dx) > mFHM[index].at<double>(y, x)) {
								maxFlag = false;
								break;
							}
						}
					}

					if (maxFlag) {
						keyPoints.push_back(KeyPoint(x, y, std::atan2(Go[1].at<double>(y, x), Go[0].at<double>(y, x)), mFHM[index].at<double>(y, x), index));
					}

				}
			}


		}

		// test
		//testKeyPoints(keyPoints);

		std::cerr << keyPoints.size() << std::endl;

		return keyPoints;
	}

	const cv::Mat & ImageConfig::getOriginalImage() const {
		return mOriginalImage;
	}




	void testKeyPoints(const ST::ImageConfig& image, const std::vector<ST::KeyPoint>& keyPoints, std::string name) {
		std::cerr << keyPoints.size() << std::endl;

		//cv::Mat result(cv::Size(mScaledImages[0].size()), CV_8UC3);

		cv::Mat result = ST::cylinderProjection(image.getOriginalImage(), kLength, false);

		//cv::cvtColor(mScaledImages[0], result, cv::COLOR_GRAY2BGR);

		for (int i = 0; i < keyPoints.size(); ++i) {
			int radius = 16 >> (4 - keyPoints[i].getScale());
			cv::circle(result, cvPoint(keyPoints[i].getX() * radius, keyPoints[i].getY() * radius), radius * 4, CV_RGB(255, 0, 0));
		}


		cv::imshow(name, result);
		//cv::waitKey(0);
	}

}
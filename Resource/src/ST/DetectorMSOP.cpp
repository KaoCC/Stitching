#include "DetectorMSOP.hpp"

#include <opencv2/opencv.hpp>

#include "ImageConfig.hpp"
#include "KeyPoint.hpp"


namespace ST {


	static const double kGSigD = 1.0;
	static const double kGSigI = 1.5;
	static const double kGSigO = 4.5;


	std::vector<KeyPoint> DetectorMSOP::detect(ImageConfig& imageConf) {

		std::vector<KeyPoint> keyPoints;

		for (size_t index = 0; index < imageConf.kScaleSize; ++index) {

			const auto& scaledImgRef = imageConf.getScaledImages(index);

			// Compute x and y derivatives of image

			//Gradient : Gx, Gy
			std::array<cv::Mat, 2> G;
			computeGradient(scaledImgRef, G);

			

			//cv::imshow("G", G[1]);
			//cv::waitKey(0);

			// Gaussian: x, y
			std::array<cv::Mat, 2> Gg;
			for (size_t idx = 0; idx < G.size(); ++idx) {
				cv::GaussianBlur(G[idx], Gg[idx], cv::Size(0, 0), kGSigD);
			}

			// Ixx, Ixy, Iyy
			std::array<cv::Mat, 3> I{ cv::Mat(scaledImgRef.size(), CV_64F), cv::Mat(scaledImgRef.size(), CV_64F), cv::Mat(scaledImgRef.size(), CV_64F) };
			auto& Ixx = I[0];
			auto& Ixy = I[1];
			auto& Iyy = I[2];

			for (int y = 0; y < scaledImgRef.size().height; ++y) {
				for (int x = 0; x < scaledImgRef.size().width; ++x) {

					auto& ix = Gg[0].at<double>(y, x);
					auto& iy = Gg[1].at<double>(y, x);

					Ixx.at<double>(y, x) = ix * ix;
					Ixy.at<double>(y, x) = ix * iy;
					Iyy.at<double>(y, x) = iy * iy;
				}
			}

			std::array<cv::Mat, 3> H{ cv::Mat(scaledImgRef.size(), CV_64F),  cv::Mat(scaledImgRef.size(), CV_64F), cv::Mat(scaledImgRef.size(), CV_64F) };

			// compute Gaussian
			for (size_t idx = 0; idx < H.size(); ++idx) {
				cv::GaussianBlur(I[idx], H[idx], cv::Size(0, 0), kGSigI);
			}


			//cv::imshow("H", H[0]);
			//cv::waitKey(0);

			auto& fhmRef = imageConf.getFHM(index);

			fhmRef = cv::Mat(scaledImgRef.size(), CV_64F);

			for (int y = 0; y < scaledImgRef.size().height; ++y) {
				for (int x = 0; x < scaledImgRef.size().width; ++x) {

					double det = H[0].at<double>(y, x) * H[2].at<double>(y, x) - H[1].at<double>(y, x) * H[1].at<double>(y, x);
					double trace = H[0].at<double>(y, x) + H[2].at<double>(y, x);

					fhmRef.at<double>(y, x) = det / (trace + 0.0001);		// tmp

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
			for (int y = 0; y < fhmRef.size().height; ++y) {
				for (int x = 0; x < fhmRef.size().width; ++x) {

					if (fhmRef.at<double>(y, x) < 10) {
						continue;
					}

					// find local max
					bool maxFlag = true;
					for (int dy = -1; dy < 1; ++dy) {
						for (int dx = -1; dx < 1; ++dx) {

							// check the boundary
							if ((dy == 0 && dx == 0) || (x + dx == fhmRef.size().width) || (x + dx < 0) || (y + dy == fhmRef.size().height) || (y + dy < 0)) {
								continue;
							}

							// not the local max ...
							if (fhmRef.at<double>(y + dy, x + dx) > fhmRef.at<double>(y, x)) {
								maxFlag = false;
								break;
							}
						}
					}

					if (maxFlag) {
						keyPoints.push_back(KeyPoint(x, y, std::atan2(Go[1].at<double>(y, x), Go[0].at<double>(y, x)), fhmRef.at<double>(y, x), index));
					}

				}
			}


		}

		// test
		//testKeyPoints(keyPoints);

		std::cerr << keyPoints.size() << std::endl;

		return keyPoints;

	}

}

#include "DescriptorMSOP.hpp"


namespace ST {




	std::shared_ptr<Descriptor> DescriptorMSOP::createDescriptorMSOP(const ImageConfig & imageConf, const KeyPoint & keypoint) {

		std::shared_ptr<DescriptorMSOP> descriptor{ new DescriptorMSOP() };

		//std::vector<KeyPoint> points;
		//points.push_back(keypoint);
		//testKeyPoints(imageConf, points, "tmp");

		double angle = keypoint.getAngle();

		//angle = 0;
		//angle = 3.14/16;
		//std::cerr << "Angle: " << angle << '\n';

		cv::Mat trans(cv::Size(3, 2), CV_64F);
		trans.at <double>(0, 0) = std::cos(angle);
		trans.at <double>(0, 1) = -std::sin(angle);

		trans.at <double>(1, 0) = std::sin(angle);
		trans.at <double>(1, 1) = std::cos(angle);

		trans.at <double>(0, 2) = keypoint.getX();
		trans.at <double>(1, 2) = keypoint.getY();

		cv::Mat sample(cv::Size(40, 40), CV_8U);
		cv::Mat patch(cv::Size(8, 8), CV_8U);

		//std::cerr << keypoint.getX() << ',' <<keypoint.getY() <<'\n';

		//img ref
		const auto& imgRef = imageConf.getScaledImages(keypoint.getScale());

		// test
		//cv::Mat test(imgRef.size(), CV_8U);

		// KAOCC: check !
		cv::warpAffine(imgRef, sample, trans, cv::Size(40, 40), cv::WARP_INVERSE_MAP, cv::BORDER_TRANSPARENT);

		//cv::resize(test, sample, cv::Size(40, 40));

		//cv::getRectSubPix(imgRef, cv::Size(40, 40), cv::Point2f(keypoint.getX(), keypoint.getY()), sample);
		
		//int scale = std::pow(2, keypoint.getScale());

		//for (int j = 0; j < sample.size().height; ++j) {
		//	for (int i = 0; i < sample.size().width; ++i) {
		//		sample.at<uchar>(j, i) = test.at<uchar>(j, i);

		//	}
		//}

		cv::resize(sample, patch, cv::Size(8, 8), cv::INTER_AREA);

		//cv::imshow("test", test);
		//cv::imshow("sample", sample);

		//cv::imshow("img", imgRef);
		//cv::imshow("result", descriptor->mSample);
		//cv::waitKey(0);

		// save to des

		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				descriptor->mDes.at<double>(j, i) = patch.at<uchar>(j, i);

			}
		}

		// normal
		double sum = 0;

		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				sum += patch.at<uchar>(j, i);
			}

		}

		double avg = sum / 64;

		//std::cerr << "avg:" << avg << '\n';

		double sq = 0;
		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				descriptor->mDes.at<double>(j, i) -= avg;

				sq += (descriptor->mDes.at<double>(j, i) * descriptor->mDes.at<double>(j, i));
			}

		}

		//std::cerr << "SQ:" << sq << '\n';
		double sigma = std::sqrt(sq / 64);

		if ((int)sigma == 0) {
			sigma += 0.0001;
		}

		//std::cerr << "Sig:" << sigma << '\n';

		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				descriptor->mDes.at<double>(j, i) /= (sigma);

			}

		}


		cv::waitKey(0);

		// calculate wavelet Tr.

		descriptor->mWaveletArray[0] = 0;
		descriptor->mWaveletArray[1] = 0;
		descriptor->mWaveletArray[2] = 0;

		for (int j = 0; j < descriptor->mDes.size().height; ++j) {
			for (int i = 0; i < descriptor->mDes.size().width / 2; ++i) {
				descriptor->mWaveletArray[0] += descriptor->mDes.at<double>(j, i);
			}

		}

		for (int j = 0; j < descriptor->mDes.size().height / 2; ++j) {
			for (int i = 0; i < descriptor->mDes.size().width; ++i) {
				descriptor->mWaveletArray[1] += descriptor->mDes.at<double>(j, i);
			}

		}


		for (int j = 0; j < descriptor->mDes.size().height / 2; ++j) {
			for (int i = 0; i < descriptor->mDes.size().width / 2; ++i) {
				descriptor->mWaveletArray[2] += descriptor->mDes.at<double>(j, i);
			}

		}

		for (int j = descriptor->mDes.size().height / 2; j < descriptor->mDes.size().height; ++j) {
			for (int i = descriptor->mDes.size().width / 2; i < descriptor->mDes.size().width; ++i) {
				descriptor->mWaveletArray[2] += descriptor->mDes.at<double>(j, i);
			}

		}


		return descriptor;

	}

	double DescriptorMSOP::diff(const KeyPoint & keypointA, const KeyPoint & keypointB) {

		auto& desA = keypointA.getDescriptor();
		auto& desB = keypointB.getDescriptor();

		DescriptorMSOP* desAMSOP = dynamic_cast<DescriptorMSOP*>(desA.get());
		DescriptorMSOP* desBMSOP = dynamic_cast<DescriptorMSOP*>(desB.get());

		double result = 0;

		for (int j = 0; j < desAMSOP->mDes.size().height; ++j) {
			for (int i = 0; i < desAMSOP->mDes.size().width; ++i) {
				result += std::abs((desAMSOP->mDes.at<double>(j, i) - desBMSOP->mDes.at<double>(j, i)));

				//std::cerr << "a:" << desAMSOP->mDes.at<double>(j, i) << " b: " << desBMSOP->mDes.at<double>(j, i) << std::endl;
			}

		}

		return result;


	}

	bool DescriptorMSOP::waveletCompare(const KeyPoint & keypointA, const KeyPoint & keypointB) {

		auto& desA = keypointA.getDescriptor();
		auto& desB = keypointB.getDescriptor();

		DescriptorMSOP* desAMSOP = dynamic_cast<DescriptorMSOP*>(desA.get());
		DescriptorMSOP* desBMSOP = dynamic_cast<DescriptorMSOP*>(desB.get());

		bool result = true;

		for (int i = 0; i < desAMSOP->mWaveletArray.size(); ++i) {
			if (std::abs(desAMSOP->mWaveletArray[i] - desBMSOP->mWaveletArray[i] )> 16) {
				result = false;
				break;
			}
		}


		return result;

	}





	DescriptorMSOP::DescriptorMSOP() :  mDes(cv::Size(8, 8), CV_64F) {
	}

	DescriptorMSOP::~DescriptorMSOP() {
	}

}

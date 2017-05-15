#include "DescriptorMSOP.hpp"


namespace ST {




	std::shared_ptr<Descriptor> DescriptorMSOP::createDescriptorMSOP(const ImageConfig & imageConf, const KeyPoint & keypoint) {

		std::shared_ptr<DescriptorMSOP> descriptor{ new DescriptorMSOP() };


		double angle = keypoint.getAngle();

		cv::Mat trans(cv::Size(3, 2), CV_64F);
		trans.at <double>(0, 0) = std::cos(angle);
		trans.at <double>(0, 1) = -std::sin(angle);

		trans.at <double>(1, 0) = std::sin(angle);
		trans.at <double>(1, 1) = std::cos(angle);

		trans.at <double>(0, 2) = keypoint.getX();
		trans.at <double>(1, 2) = keypoint.getY();

		cv::Mat sample(cv::Size(40, 40), CV_8U);
		cv::Mat patch(cv::Size(8, 8), CV_8U);


		cv::warpAffine(imageConf.getScaledImages(keypoint.getScale()), sample, trans, cv::Size(40, 40));
		cv::resize(sample, patch, cv::Size(8, 8));

		//cv::imshow("img", imageConf.getScaledImages(keypoint.getScale()));
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

		double sq = 0;
		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				descriptor->mDes.at<double>(j, i) -= avg;
				sq = descriptor->mDes.at<double>(j, i) * descriptor->mDes.at<double>(j, i);
			}

		}

		double sigma = std::sqrt(sq / 64);

		for (int j = 0; j < patch.size().height; ++j) {
			for (int i = 0; i < patch.size().width; ++i) {
				descriptor->mDes.at<double>(j, i) /= sigma;
			}

		}

		//cv::imshow("result", descriptor->mDes);
		//cv::waitKey(0);

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





	DescriptorMSOP::DescriptorMSOP() :  mDes(cv::Size(8, 8), CV_64F) {
	}

	DescriptorMSOP::~DescriptorMSOP() {
	}

}

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


		cv::warpAffine(imageConf.getScaledImages(keypoint.getScale()), descriptor->mSample, trans, cv::Size(40, 40));
		cv::resize(descriptor->mSample, descriptor->mPatch, cv::Size(8, 8));

		//cv::imshow("img", imageConf.getScaledImages(keypoint.getScale()));
		//cv::imshow("result", descriptor->mSample);
		//cv::waitKey(0);

		// normal
		double sum = 0;

		for (int j = 0; j < descriptor->mPatch.size().height; ++j) {
			for (int i = 0; i < descriptor->mPatch.size().width; ++i) {
				sum += descriptor->mPatch.at<double>(j, i);
			}

		}

		double avg = sum / 64;

		double sq = 0;
		for (int j = 0; j < descriptor->mPatch.size().height; ++j) {
			for (int i = 0; i < descriptor->mPatch.size().width; ++i) {
				descriptor->mPatch.at<double>(j, i) -= avg;
				sq = descriptor->mPatch.at<double>(j, i) * descriptor->mPatch.at<double>(j, i);
			}

		}

		double sigma = std::sqrt(sq / 64);

		for (int j = 0; j < descriptor->mPatch.size().height; ++j) {
			for (int i = 0; i < descriptor->mPatch.size().width; ++i) {
				descriptor->mPatch.at<double>(j, i) /= sigma;
			}

		}

		cv::imshow("result", descriptor->mSample);
		cv::waitKey(0);

		return descriptor;

	}





	DescriptorMSOP::DescriptorMSOP() : mSample(cv::Size(40, 40), CV_64F), mPatch(cv::Size(8, 8), CV_64F) {
	}

	DescriptorMSOP::~DescriptorMSOP() {
	}

}

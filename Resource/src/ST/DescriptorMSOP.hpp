
#ifndef _ST_DESCRIPTORMSOP_HPP_
#define _ST_DESCRIPTORMSOP_HPP_

#include "Descriptor.hpp"
#include "ImageConfig.hpp"
#include "KeyPoint.hpp"

#include <opencv2/opencv.hpp>

namespace ST {

	class DescriptorMSOP : public Descriptor {
	public:

		static std::shared_ptr<Descriptor> createDescriptorMSOP(const ImageConfig& imageConf, const KeyPoint& keypoint);

		virtual ~DescriptorMSOP();

	private:

		DescriptorMSOP();
		
		//cv::Mat mSample;
		//cv::Mat mPatch;
		cv::Mat mDes;
		std::array<double, 3> mWaveletArray;

	};

}


#endif

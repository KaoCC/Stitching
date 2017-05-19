#ifndef _ST_DETECTORMSOP_HPP_
#define _ST_DETECTORMSOP_HPP_


#include "Detector.hpp"

namespace ST {

	class DetectorMSOP : public Detector {
	public:
		virtual std::vector<KeyPoint> detect(ImageConfig& imageConf) override;
	};


}









#endif


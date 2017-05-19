
#ifndef _ST_DETECTOR_HPP_
#define _ST_DETECTOR_HPP_

#include <vector>


namespace ST {

	class KeyPoint;
	class ImageConfig;

	class Detector {
	public:
		virtual std::vector<KeyPoint> detect(ImageConfig& imageConf) = 0;
	};



}


#endif

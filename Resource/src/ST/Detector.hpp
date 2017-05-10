
#ifndef _ST_DETECTOR_HPP_
#define _ST_DETECTOR_HPP_

#include <vector>

namespace ST {

	class KeyPoint;

	class Detector {
	public:
		virtual void detect(std::vector<KeyPoint>& keypoints) = 0;
	};



}


#endif

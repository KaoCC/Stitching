#ifndef _ST_KEYPOINT_HPP_
#define _ST_KEYPOINT_HPP_

#include <utility>
#include <array>

namespace ST {

	class Descriptor;

	class KeyPoint {

	public:

		KeyPoint(double x, double y, double angle, double val, std::size_t s);

		double getX() const {
			return mPt.first;
		}

		double getY() const {
			return mPt.second;
		}


		std::size_t getScale() const {
			return mS;
		}

	private:

		// coordinates: (x, y)
		std::pair<double, double> mPt;

		// angle
		double mAngle;

		double mValue;

		std::size_t mS;

		Descriptor* mDescriptor;
		std::array<double, 3> mWaveletArray;


	};

}





#endif







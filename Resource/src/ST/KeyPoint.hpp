#ifndef _ST_KEYPOINT_HPP_
#define _ST_KEYPOINT_HPP_

#include <utility>
#include <array>

#include <memory>

namespace ST {

	class Descriptor;

	class KeyPoint {

	public:

		KeyPoint(double x, double y, double angle, double val, std::size_t s);

		static double computeDistanceSquare(const KeyPoint& a, const KeyPoint& b);

		static bool compare(const KeyPoint& a, const KeyPoint& b);

		double getX() const {
			return mPt.first;
		}

		double getY() const {
			return mPt.second;
		}


		double getAngle() const {
			return mAngle;
		}

		std::size_t getScale() const {
			return mScale;
		}

		double getValue() const {
			return mValue;
		}

		void adjustX(double deltaX) {
			mPt.first += deltaX;
		}

		void adjustY(double deltaY) {
			mPt.second += deltaY;
		}

		void setDescriptor(std::shared_ptr<Descriptor> des) {
			mDescriptor = des;
		}

		//void waveletTransform();

	private:

		// coordinates: (x, y)
		std::pair<double, double> mPt;

		// angle
		double mAngle;

		double mValue;

		std::size_t mScale;

		std::shared_ptr<Descriptor>  mDescriptor;



	};

}





#endif







#include "KeyPoint.hpp"



namespace ST {

	KeyPoint::KeyPoint(double x, double y, double angle, double val, std::size_t s) : mPt(x, y) , mAngle(angle), mValue(val), mScale(s), mDescriptor(nullptr) {


	}

	double KeyPoint::computeDistanceSquare(const KeyPoint & a, const KeyPoint & b) {

		const auto& x1 = a.getX();
		const auto& x2 = b.getX();
		const auto& y1 = a.getY();
		const auto& y2 = b.getY();

		return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	}

	bool KeyPoint::compare(const KeyPoint & a, const KeyPoint & b) {
		return a.getValue() > b.getValue();
	}


}



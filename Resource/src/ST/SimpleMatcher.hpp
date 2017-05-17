#ifndef _ST_SIMPLEMATCHER_HPP_
#define _ST_SIMPLEMATCHER_HPP_

#include "Matcher.hpp"
#include <opencv2/opencv.hpp>

namespace ST {


	class AffineData {

	public:
		AffineData();

		void setDeltaX(double deltaX) {
			mDeltaX = deltaX;
		}

		void setDeltaY(double deltaY) {
			mDeltaY = deltaY;
		}

		void setAffineMat(const cv::Mat& mat) {
			mAffine = mat;
		}

		double getDeltaX() const {
			return mDeltaX;
		}

		double getDeltaY() const {
			return mDeltaY;
		}
		

		cv::Mat& getAffineMat() {
			return mAffine;
		}

		const cv::Mat& getAffineMat() const {
			return mAffine;
		}



	private:

		cv::Mat mAffine;
		double mDeltaX;
		double mDeltaY;

	};

	class SimpleMatcher : public Matcher {
	public:


		virtual MatchPairs match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) override;

		AffineData computeAffine(const MatchPairs& matchData);

	};





}








#endif


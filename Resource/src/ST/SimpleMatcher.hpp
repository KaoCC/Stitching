#ifndef _ST_SIMPLEMATCHER_HPP_
#define _ST_SIMPLEMATCHER_HPP_

#include "Matcher.hpp"

namespace ST {


	class AffineData {

	public:
		AffineData();

	private:

		cv::Mat mAffine;
		double deltaX;
		double deltaY;

	};

	class SimpleMatcher : public Matcher {
	public:


		virtual MatchPairs match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) override;

		AffineData computeAffine(const MatchPairs& matchData);

	};





}








#endif


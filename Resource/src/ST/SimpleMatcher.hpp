#ifndef _ST_SIMPLEMATCHER_HPP_
#define _ST_SIMPLEMATCHER_HPP_

#include "Matcher.hpp"
#include "AffineData.hpp"

namespace ST {




	class SimpleMatcher : public Matcher {
	public:


		virtual MatchPairs match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) override;

		AffineData computeAffine(const MatchPairs& matchData, bool withTripod);

	};





}








#endif


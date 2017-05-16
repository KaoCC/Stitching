#ifndef _ST_SIMPLEMATCHER_HPP_
#define _ST_SIMPLEMATCHER_HPP_

#include "Matcher.hpp"

namespace ST {


	class SimpleMatcher : public Matcher {
	public:


		virtual MatchPair match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) override;

	};

}








#endif


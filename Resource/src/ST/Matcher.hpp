#ifndef _ST_MATCHER_HPP_
#define _ST_MATCHER_HPP_

#include <vector>
#include "KeyPoint.hpp"

namespace ST {

	using MatchPair = std::vector<std::pair<const KeyPoint*,const KeyPoint*>>;


	class Matcher {

	public:
		virtual MatchPair match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) = 0;

		virtual ~Matcher() = default;

	};
}








#endif


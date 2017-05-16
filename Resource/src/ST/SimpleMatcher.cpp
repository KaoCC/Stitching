#include "SimpleMatcher.hpp"

#include "DescriptorMSOP.hpp"

namespace ST {

	MatchPair SimpleMatcher::match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) {

		MatchPair matchpair;

		double threshold = 30;
		double tmp = 0;

		std::vector<int> indexOfA(featuresA.size());
		std::vector<int> indexOfB(featuresB.size());

		// match A with B
		for (int i = 0; i < featuresA.size(); ++i) {
			indexOfA[i] = -1;
			double minimum = threshold;

			// loop over B
			for (int j = 0; j < featuresB.size(); ++j) {
				if (featuresA[i].getScale() != featuresB[j].getScale()) {
					continue;
				}


				tmp = 0;

				// wavelet compare
				if (!DescriptorMSOP::waveletCompare(featuresA[i], featuresB[j])) {
					tmp = threshold;
				}
	
				// point to point
				if (tmp < threshold) {
					tmp = DescriptorMSOP::diff(featuresA[i], featuresB[j]);
					//std::cerr << "diff" << tmp << std::endl;
				}

				// smaller than min?
				if (tmp < minimum) {
					minimum = tmp;
					//matchpair.push_back(std::make_pair(&featuresA[i], &featuresB[j]));
					indexOfA[i] = j;
				}

			}

		}

		tmp = 0;

		// match B with A
		for (int i = 0; i < featuresB.size(); ++i) {
			indexOfB[i] = -1;
			double minimum = threshold;

			// loop over A
			for (int j = 0; j < featuresA.size(); ++j) {
				if (featuresB[i].getScale() != featuresA[j].getScale()) {
					continue;
				}


				tmp = 0;

				// wavelet compare
				if (!DescriptorMSOP::waveletCompare(featuresB[i], featuresA[j])) {
					tmp = threshold;
				}

				// point to point
				if (tmp < threshold) {
					tmp = DescriptorMSOP::diff(featuresB[i], featuresA[j]);
					//std::cerr << "diff" << tmp << std::endl;
				}

				// smaller than min?
				if (tmp < minimum) {
					minimum = tmp;
					//matchpair.push_back(std::make_pair(&featuresA[i], &featuresB[j]));
					indexOfB[i] = j;
				}

			}
		}


		// if they match ... create a pair
		for (int i = 0; i < featuresA.size(); ++i) {
			if (indexOfA[i] != -1 && indexOfB[indexOfA[i]] == i) {
				int j = indexOfA[i];
				matchpair.push_back(std::make_pair(&featuresA[i], &featuresB[j]));
			}
		}

		return matchpair;
	}

}

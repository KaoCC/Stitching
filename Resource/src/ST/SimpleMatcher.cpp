#include "SimpleMatcher.hpp"

#include "DescriptorMSOP.hpp"

#include <opencv2/opencv.hpp>

namespace ST {

	MatchPairs SimpleMatcher::match(const std::vector<KeyPoint>& featuresA, const std::vector<KeyPoint>& featuresB) {

		MatchPairs matchpair;

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

	AffineData SimpleMatcher::computeAffine(const MatchPairs& matchData) {

		AffineData affine;

		// x , y
		std::vector<std::array<double, 2>> affMatrixA(matchData.size());
		std::vector<std::array<double, 2>> affMatrixB(matchData.size());

		for (int i = 0; i < matchData.size(); ++i) {

			// get match KeyPoints
			const auto& keyPointA = *matchData[i].first;
			const auto& keyPointB = *matchData[i].second;

			affMatrixA[i][0] = keyPointA.getX();
			affMatrixA[i][1] = keyPointA.getY();

			affMatrixB[i][0] = keyPointB.getX();
			affMatrixB[i][1] = keyPointB.getY();			

		}


		// k ?

		std::array<int, 3> indexArray;

		do {

			for (int i = 0; i < indexArray.size(); ++i) {
				// tmp
				indexArray[i] = std::rand() % matchData.size();
			}

		} while (indexArray[0] == indexArray[1] || indexArray[0] == indexArray[2] || indexArray[1] == indexArray[2]);





		return affine;


	}



	AffineData::AffineData() : mAffine(cv::Size(3, 2), CV_64F) {
	}

}

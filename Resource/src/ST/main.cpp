

#include <vector>
#include <string>

#include "ImageConfig.hpp"
#include "KeyPoint.hpp"

const std::string kDefaultBasePath = "../Resource/input_image/";
const std::string kDefaultFileList = "list.txt";

int main(int argc, char* argv[]) {

	std::string basePath;
	if (argc > 1) {
		basePath = argv[1];
	} else {
		basePath = kDefaultBasePath;
	}

	std::string fileList;
	if (argc > 2) {
		fileList = argv[2];
	} else {
		fileList = kDefaultFileList;
	}

	std::vector<ST::ImageConfig> images;
	ST::loadImageConfigs(basePath, fileList, images);



	// compute features
	std::vector<std::vector<ST::KeyPoint>> features;
	for (auto& image : images) {
		features.push_back(image.computeKeyPoints());
	}

	// SubPixel Refinement

	// Matching




	return 0;
}



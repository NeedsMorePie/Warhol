#pragma once

#include "stdafx.h"

class Monroe
{
public:
	Monroe(const cv::Mat &image);
	~Monroe();

private:
	void process();
	void getColors(std::vector<std::vector<float>> &randColors, unsigned numClusters, const cv::Mat &originalColors);
	void setPixel(int i, int x, int y, int cluster_id,
		const std::vector<std::vector<std::vector<float>>> &randColors, cv::Mat &image);
	cv::Mat stitch(const std::vector<cv::Mat> &quadrants);

	cv::Mat image;
	cv::Mat grayscale;
};


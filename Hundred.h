#pragma once

#include "stdafx.h"

class Hundred
{
public:
	Hundred(const cv::Mat &image);
	~Hundred();

private:
	void process();

	cv::Mat convolve(const cv::Mat &x, const cv::Mat &h);
	cv::Mat stitch(const std::vector<std::vector<cv::Mat>> &grid);

	cv::Mat image;
	cv::Mat grayscale;
};


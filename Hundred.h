#pragma once

#include "stdafx.h"

class Hundred
{
public:
	Hundred(const cv::Mat &image);
	~Hundred();

private:
	void process();

	cv::Mat image;
	cv::Mat grayscale;
};


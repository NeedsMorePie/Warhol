#pragma once

#include "stdafx.h"

class Monroe
{
public:
	Monroe(const cv::Mat &image);
	~Monroe();

private:
	cv::Mat image;
};


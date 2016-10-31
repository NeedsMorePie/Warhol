#include "stdafx.h"
#include "Hundred.h"


Hundred::Hundred(const cv::Mat &image)
	: image(image)
{
	cv::cvtColor(image, grayscale, CV_RGB2GRAY);
	process();
}


Hundred::~Hundred()
{
}


void Hundred::process()
{
	cv::imshow("Hundred", grayscale);
	cv::imwrite("hundred.jpg", grayscale);
}

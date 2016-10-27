// Warhol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat image;
	image = cv::imread("C:\\Users\\davis\\OneDrive\\Pictures\\timetable.png", CV_LOAD_IMAGE_COLOR);  

	if (!image.data)        
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	cv::imshow("Display window", image);

	cv::waitKey(0);

	return 0;
}


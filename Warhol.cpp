// Warhol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void printCredits();

int main(int argc, char* argv[])
{
	printCredits();

	if (argc != 3)
	{
		std::cout << "Incorrect arguments. Aborting..." << std::endl;
		return 0;
	}
	
	std::string command = argv[1];
	std::string imagePath = argv[2];

	cv::Mat image;
	image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);

	if (!image.data)        
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	if (command == "-m")
	{

	}

	return 0;
}

void printCredits()
{
	std::cout << "------------------WARHOL------------------" << std::endl;
	std::cout << "-----------------Davis Wu-----------------" << std::endl;
	std::cout << std::endl;
}
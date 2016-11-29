// Warhol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Monroe.h"
#include "Hundred.h"

void printCredits();
void resizeAndCrop(cv::Mat &image);
void startContinuousCapture();

int main(int argc, char* argv[])
{
	srand(time(NULL));
	printCredits();

	if (argc < 2)
	{
		std::cout << "Incorrect arguments. Aborting..." << std::endl;
		return 0;
	}
	
	std::string command = argv[1];
	std::string imagePath = argc >= 3 ? argv[2] : "";

	if (command == "-video")
	{
		startContinuousCapture();
		return 0;
	}

	cv::Mat image;
	image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);

	if (!image.data)        
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	resizeAndCrop(image);

	if (command == "-m")
	{
		Monroe processor(image);
	}
	else if (command == "-h")
	{
		Hundred processor(image);
	}

	cv::waitKey(0);

	return 0;
}

void printCredits()
{
	std::cout << "------------------WARHOL------------------" << std::endl;
	std::cout << "-----------------Davis Wu-----------------" << std::endl;
	std::cout << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "    -m <image path>     Monroe generator" << std::endl;
	std::cout << "    -h <image path>     Hundred generator" << std::endl;
	std::cout << "    -video              Continuous video feed" << std::endl;
	std::cout << std::endl;
}

void resizeAndCrop(cv::Mat &image)
{
	int rows = image.rows;
	int cols = image.cols;
	int minDimensions = rows < cols ? rows : cols;

	int x = (cols - minDimensions) / 2;
	int y = (rows - minDimensions) / 2;

	if (x < 0) x = 0;
	if (y < 0) y = 0;

	// Do crop
	cv::Rect myROI(x, y,
		minDimensions, minDimensions);
	image = image(myROI);

	// Do resize
	cv::resize(image, image, cv::Size(500, 500));
}

void startContinuousCapture()
{
	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		std::cout << "Failed to capture video" << std::endl;
		return;
	}

	std::cout << "Press ESC on the video feed to exit" << std::endl;

	cv::namedWindow("Warhol", 1);
	while (true)
	{
		cv::Mat frame;
		cap >> frame; 
		cv::imshow("Warhol", frame);

		int key = cv::waitKey(30);

		//if (key >= 0)
		//std::cout << key << std::endl;

		if (key == 27) // ESC
		{
			break;
		}
		else if (key == 109) // m
		{
			resizeAndCrop(frame);
			Monroe processor(frame);
		}
		else if (key == 104) // h
		{
			resizeAndCrop(frame);
			Hundred processor(frame);
		}
	}
}
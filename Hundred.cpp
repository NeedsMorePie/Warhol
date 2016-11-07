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
	const int gridSize = 100;
	const int numCells = 10;
	const int gamma = 30;
	const double gain = 1.5;
	cv::resize(grayscale, grayscale, cv::Size(gridSize, gridSize));

	// Choose a random row and column to be the max convolution
	int randRow = rand() % numCells;
	int randCol = rand() % numCells;

	// Get noise matrix
	cv::Mat noise = cv::Mat(grayscale.size(), grayscale.type());
	cv::randn(noise, 0, 15);

	// Initialize a grid of images
	std::vector<std::vector<cv::Mat>> grid;
	for (int i = 0; i < numCells; ++i)
	{
		grid.push_back(std::vector<cv::Mat>());
		for (int j = 0; j < numCells; ++j)
		{
			double xDiff = randCol - j;
			double yDiff = randRow - i;
			int blurSize = 10 - (int)std::sqrt(xDiff * xDiff + yDiff * yDiff);
			if (blurSize > 4 && rand() % 2 == 1) blurSize -= 1; // Randomness
			if (blurSize <= 2) blurSize = 3; // Minimum blur size is 3
			if ((blurSize & 0x1) == 0) blurSize += 1; // Ensure odd

			// Get matrix h for convolution. Values are from 0 to 255 as a uchar
			cv::Mat h;
			cv::resize(grayscale, h, cv::Size(blurSize, blurSize));
			cv::Mat convolved = convolve(grayscale, h);

			// Random noise factor
			double randNoiseFactor = (double)(rand() % 1001) / 1000 * 0.4 + blurSize / 10.0 * 0.3;

			// Random gain factor
			double randGainFactor = (double)(rand() % 700) / 1000 + 1.0 + (blurSize / 20.0);

			grid[i].push_back(convolved * gain * randGainFactor + gamma + noise * randNoiseFactor);
		}
	}

	cv::Mat stitched = stitch(grid);
	cv::normalize(stitched, stitched, 0, 230, cv::NORM_MINMAX);
	cv::imshow("Hundred", stitched);
	cv::imwrite("hundred.jpg", stitched);
}

/*
 * Returns matrix y that is the same size as x after convolution with h
 */
cv::Mat Hundred::convolve(const cv::Mat &x, const cv::Mat &h)
{
	cv::Mat y(x.rows, x.cols, x.type()); 
	int pivot = (int)h.cols / 2; // Assume h is a square

	// (col, row) = (x, y) of matrix x
	for (int row = 0; row < x.rows; ++row)
	{
		for (int col = 0; col < x.cols; ++col)
		{
			// Get value for a single pixel
			int sum = 0;

			// (i, j) = (x, y) of matrix h
			for (int i = 0; i < h.cols; ++i) // horizontal
			{
				for (int j = 0; j < h.rows; ++j) // vertical
				{
					int xIdx = (i - pivot) + col;
					int yIdx = (j - pivot) + row;
					bool withinBounds = xIdx >= 0 && xIdx < x.cols &&
										yIdx >= 0 && yIdx < x.rows;

					int pixelValue = withinBounds ? x.at<uchar>(yIdx, xIdx) : 0;
					sum += pixelValue * (double)h.at<uchar>(j, i)/255.0;
				}
			}

			// Average it out
			y.at<uchar>(row, col) = sum / (h.cols * h.rows);
		}
	}

	return y;
}

cv::Mat Hundred::stitch(const std::vector<std::vector<cv::Mat>> &grid)
{
	// Assume grid has at least one item in it
	int cellSize = grid[0][0].rows;
	int imageSize = cellSize * grid.size();
	cv::Mat stitched(imageSize, imageSize, grid[0][0].type());

	for (int i = 0; i < grid.size(); ++i)
	{
		for (int j = 0; j < grid[0].size(); ++j)
		{
			// Copying the ith row and jth column
			grid[i][j].copyTo(stitched(cv::Rect(j * cellSize, i * cellSize, cellSize, cellSize)));
		}
	}

	return stitched;
}
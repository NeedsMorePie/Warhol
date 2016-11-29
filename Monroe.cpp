#include "stdafx.h"
#include "Monroe.h"


Monroe::Monroe(const cv::Mat &image) 
	: image(image)
{
	cv::cvtColor(image, grayscale, CV_RGB2GRAY);
	process();
}


Monroe::~Monroe()
{
}


void Monroe::process()
{
	// Convert to lab
	cv::cvtColor(image, image, cv::COLOR_BGR2Lab);

	// Blur image
	cv::blur(image, image, cv::Size(10, 10));

	// Channels to use
	std::unordered_set<int> channelsToUse;
	channelsToUse.insert(0);
	channelsToUse.insert(1);
	channelsToUse.insert(2);

	// Flatten
	int channels = image.channels();
	cv::Mat samples(image.rows * image.cols, channels, CV_32F);
	for (int y = 0; y < image.rows; ++y)
	{
		for (int x = 0; x < image.cols; ++x)
		{
			for (int z = 0; z < channels; ++z)
			{
				if (channelsToUse.find(z) != channelsToUse.end())
				{
					if (z == 0)
					{
						samples.at<float>(y + x*image.rows, z) = image.at<cv::Vec3b>(y, x)[z]*0.3;
					}
					else
					{
						samples.at<float>(y + x*image.rows, z) = image.at<cv::Vec3b>(y, x)[z];
					}
				}
				else
				{
					samples.at<float>(y + x*image.rows, z) = 0;
				}
			}
		}
	}

	// Kmeans parameters 
	int numClusters = rand() % 2 + 4;
	cv::Mat labels;
	const int attempts = 6;
	cv::Mat centers;

	// Run kmeans
	cv::kmeans(samples, numClusters, labels,
		cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001),
		attempts, cv::KMEANS_PP_CENTERS, centers);

	// Create quadrants (randomly choose either 2 or 3 rows)
	int numQuadrants = rand() % 2 + 2;
	numQuadrants *= numQuadrants;
	std::vector<cv::Mat> quadrants;
	for (int i = 0; i < numQuadrants; ++i)
	{
		quadrants.push_back(cv::Mat(image.size(), image.type()));
	}

	// Get colors
	std::vector<std::vector<std::vector<float>>> randColors;
	for (unsigned i = 0; i < quadrants.size(); ++i)
	{
		std::vector<std::vector<float>> quadrantColors;
		getColors(quadrantColors, numClusters, centers);
		randColors.push_back(quadrantColors);
	}

	// Make quadrants
	for (int y = 0; y < image.rows; ++y)
	{
		for (int x = 0; x < image.cols; ++x)
		{
			int cluster_id = labels.at<int>(y + x*image.rows, 0);

			for (unsigned i = 0; i < quadrants.size(); ++i)
			{
				setPixel(i, x, y, cluster_id, randColors, quadrants[i]);
			}
		}
	}

	cv::Mat stitchedImage = stitch(quadrants);

	cv::imshow("Monroe", stitchedImage);
	cv::imwrite("monroe.jpg", stitchedImage);
}

/* 
 * Sets the new pixel but takes into account the grayscale image
 */
void Monroe::setPixel(int i, int x, int y, int cluster_id,
	const std::vector<std::vector<std::vector<float>>> &randColors, cv::Mat &image)
{
	float intensity = ((int)grayscale.at<uchar>(y, x)) / 255.0;
	float intensityScale = 2.0;
	intensity = intensity * intensityScale;
	intensity += 0.20; // (0.4); // Increase brightness

	double r = randColors[i][cluster_id][0] * intensity;
	double g = randColors[i][cluster_id][1] * intensity;
	double b = randColors[i][cluster_id][2] * intensity;

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	image.at<cv::Vec3b>(y, x)[0] = r;
	image.at<cv::Vec3b>(y, x)[1] = g;
	image.at<cv::Vec3b>(y, x)[2] = b;
}

void Monroe::getColors(std::vector<std::vector<float>> &randColors, unsigned numClusters, const cv::Mat &originalColors)
{
	// For each cluster (color)
	for (int i = 0; i < numClusters; ++i)
	{
		float originalR = originalColors.at<float>(i, 0);
		float originalG = originalColors.at<float>(i, 1);
		float originalB = originalColors.at<float>(i, 2);

		// Generate random color as a float from 0.2 to 0.8
		float r = ((double)(rand() % 600) / 1000.0 + 0.2) * 255.0;
		float g = ((double)(rand() % 600) / 1000.0 + 0.2) * 255.0;
		float b = ((double)(rand() % 600) / 1000.0 + 0.2) * 255.0;

		float originalWeight = 0.3;
		float newWeight = 1.0 - originalWeight;

		// Average them with the original colors (weight the original colors more)
		std::vector<float> randColor;
		randColor.push_back(r * newWeight + originalR * originalWeight);
		randColor.push_back(g * newWeight + originalG * originalWeight);
		randColor.push_back(b * newWeight + originalB * originalWeight);

		randColors.push_back(randColor);
	}
}

cv::Mat Monroe::stitch(const std::vector<cv::Mat> &quadrants)
{
	int sqrtSize = std::sqrt(quadrants.size());
	int rows = quadrants[0].rows;
	int cols = quadrants[0].cols;
	cv::Mat newMat(cv::Size(rows * sqrtSize, cols * sqrtSize), quadrants[0].type());

	for (unsigned row = 0; row < sqrtSize; ++row)
	{
		for (unsigned col = 0; col < sqrtSize; ++col)
		{
			int quadrantIndex = row * sqrtSize + col;

			quadrants[quadrantIndex].copyTo(newMat(cv::Rect(col * cols, row * rows, cols, rows)));
		}
	}

	return newMat;
}
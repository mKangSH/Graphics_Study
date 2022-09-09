#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

class HistogramMonitor
{
	public:
		HistogramMonitor(std::vector<cv::Mat> planes, int histSize, int histWidth, int histHeight,
			int binWidth, bool uniform, bool accumulate, const float** histRange);

		int getHistSize(); 
		int getHistWidth(); 
		int getHistHeight();
		int getBinWidth(); 
		bool getUniform(); 
		bool getAccumulate();

		virtual ~HistogramMonitor() = default;
	private:
		std::vector<cv::Mat> mPlanes;
		int mHistSize = 256;
		int mHistWidth = 512;
		int mHistHeight = 400;
		int mBinWidth;

		bool mUniform = true;
		bool mAccumulate = true;
		
		const float** histRange;
};
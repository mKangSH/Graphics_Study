#include "Histogram.h"

HistogramMonitor::HistogramMonitor(std::vector<cv::Mat> planes, int histSize, int histWidth, int histHeight,
	int binWidth, bool uniform, bool accumulate, const float** histRange)
{
	this->mPlanes = planes;
	this->mHistSize = histSize;
	this->mHistWidth = histSize;
	this->mHistHeight = histSize;
	this->mBinWidth = histSize;
	this->mUniform = histSize;
	this->mAccumulate = histSize;
	this->mHistSize = histSize;
}

int HistogramMonitor::getHistSize()
{
	return mHistSize; 
}

int HistogramMonitor::getHistWidth() 
{ 
	return mHistWidth; 
}

int HistogramMonitor::getHistHeight() 
{
	return mHistHeight; 
}

int HistogramMonitor::getBinWidth()
{
	return mBinWidth; 
}

bool HistogramMonitor::getUniform()
{ 
	return mUniform;
}

bool HistogramMonitor::getAccumulate() 
{
	return mAccumulate;
}
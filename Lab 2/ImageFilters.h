#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function


void max_filter(cv::Mat &img , int k_size);
void min_filter(cv::Mat &img , int k_size);
void median_filter(cv::Mat &img , int k_size);

#endif // IMAGEFILTERS_H
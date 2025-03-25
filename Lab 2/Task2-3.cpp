#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>
#include "ImageFilters.h"

int main(int argc, char **argv){


    cv::Mat img = cv::imread(argv[1], cv::IMREAD_GRAYSCALE); // To force the conversion in 1 channel image
    std::cout<<img.channels()<<std::endl;

    cv::namedWindow("InputImage");
    cv::imshow("InputImage", img);
    cv::waitKey(0);
    std::cout<<img.channels();

    //cv::Mat grey_image;
    //cv::cvtColor(img, grey_image, cv::COLOR_RGB2GRAY); // If you use cvt Color it only split in 3 channels the grey but not convert in 1 channel !
    //std::cout<<grey_image.channels();

    max_filter(img, 5); //3 best setting for the Astronaut_salt_pepper image
    
    cv::namedWindow("MaxFilter");
    cv::imshow("MaxFilter", img);
    cv::waitKey(0);


    min_filter(img, 5);
    cv::namedWindow("MinFilter");
    cv::imshow("MinFilter", img);
    cv::waitKey(0);

    median_filter(img, 7);
    
    cv::namedWindow("MedianFilter");
    cv::imshow("MedianFilter", img);
    cv::waitKey(0);
    


    return 0;
}
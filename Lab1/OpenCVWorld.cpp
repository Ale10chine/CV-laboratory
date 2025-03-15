// OpenImg.cpp
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    if(argc < 2){
        std::cout<<"You must to provide an image!\n";
        return -1;
    }

    cv::Mat img = cv::imread(argv[1]);
    if(img.empty()){ // Std way to check if a Mat object return an empty matrix
       std::cout<<"You dont'provide any valid image!\n"; 
       return -2; 
    }

    cv::namedWindow("Example 1");
    cv::imshow("Example 1", img);
    cv::waitKey(0);
    return 0;
}

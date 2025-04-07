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
    if(img.empty()){
       std::cout<<"You dont'provide any valid image!\n"; 
       return -2; 
    }

    // Function member of Mat class to print the channels
    std::cout<<"Print the channels : "<<img.channels()<< std::endl; // NOTE: to be sure of printing use std::endl

    cv::namedWindow("Example 1");
    cv::imshow("Example 1", img); 

    // Save the waitKey return value in a char var and print it before ending the run    
    char var = cv::waitKey(0);
    std::cout<<"Variable that contains the content of waitKey: \t"<<var<<std::endl;

    return 0;
}

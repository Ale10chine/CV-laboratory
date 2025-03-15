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

    cv::Mat img2 = img.clone();
    cv::Mat img3 = img.clone();

    // Inspecting the image using it like a matrix with the mat object and finding setting the 
    // first channel to 0, since the first channel is RGB --> BGR, the Blue color is set to 0, the image apears 
    // with an high tonality of yellow
    if(img.channels() == 3){
        for (int i = 0; i < img.rows; ++i)
        {
            for (int j = 0; j < img.cols; ++j)
            {
                img.at<cv::Vec3b>(i, j)[1] = 0;
                img.at<cv::Vec3b>(i, j)[2] = 0;
                img2.at<cv::Vec3b> (i,j)[0] = 0;
                img2.at<cv::Vec3b> (i,j)[2] = 0; 
                img3.at<cv::Vec3b> (i,j)[0] = 0;
                img3.at<cv::Vec3b> (i,j)[1] = 0;
            }
        }   
    }

    // Function member of Mat class to print the channels
    std::cout<<"Print the channels : "<<img.channels()<< std::endl; // NOTE: to be sure of printing use std::endl


    cv::namedWindow("Example 1");
    cv::namedWindow("Example 2");
    cv::namedWindow("Example 3");
    cv::imshow("Example 1", img); 
    cv::imshow("Example 2", img2);
    cv::imshow("Example 3", img3);  

    // Save the waitKey return value in a char var and print it before ending the run    
    char var = cv::waitKey(0);
    std::cout<<"Variable that contains the content of waitKey: \t"<<var<<std::endl;

    return 0;
}

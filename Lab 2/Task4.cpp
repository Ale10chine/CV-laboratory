#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>


// Note : Must Be odd!
int KERNEL_SIZE = 25;

int main(int argc, char **argv){
    /*
    const char* filename = argc >=2 ? argv[1] : "Astronaut_original.jpg";
    cv::Mat input_img = cv::imread(samples::findFile( filename );
    */

    cv::Mat input_img = cv::imread(argv[1]);
    cv::Mat output_img;

    cv::namedWindow("InputImage");
    cv::imshow("InputImage",input_img);
    cv::waitKey(0);
    
    // Gaussian filter
    cv::GaussianBlur(input_img, output_img, cv::Size(KERNEL_SIZE,KERNEL_SIZE), 0 , 0); // input, output, Ksize, sigmaX, sigmaY

    
    cv::namedWindow("GaussianFilter");
    cv::imshow("GaussianFilter", output_img);
    cv::waitKey(0);
    

}


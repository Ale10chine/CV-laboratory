#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // Necessary if you want to use cvtColor() func
#include <iostream>

int main(int argc, char **argv){

    if(argc < 2){
        std::cout<<"You must to provide an image!\n";
        return -1;
    }

    cv::Mat img = cv::imread(argv[1]);

    if(img.empty()){
       std::cout<<"You dont'provide any valid image!\n"; 
       return -2; 
    }

    cv::Mat grey_img;
    // Function that convert the input := 'img' from RBG to GREY in output := 'grey_img'
    cv::cvtColor(img, grey_img, cv::COLOR_BGR2GRAY);

    cv::imshow("(RGB --> GREY)",grey_img);
    cv::waitKey(0);


    cv::Mat filtered_img;

    // Creation of filters
    // LINEAR FILTERS:

    // Average filter (fast solution: use cv::blur or cv::boxfilter instead of define the kernels manually)
    cv::Mat_<float> average_kernel = (cv::Mat_<float>(3, 3) <<
    1.0/9, 1.0/9, 1.0/9,
    1.0/9, 1.0/9, 1.0/9,
    1.0/9, 1.0/9, 1.0/9);
    
    // Function that applyies the (NB : only LINEAR filter) filter on the input image := 'grey_img' and returns the output on := 'filtered_Img'
    cv::filter2D(grey_img, filtered_img, -1, average_kernel); // ( -1 could be also that is the depth of the output (-1, CV_8U, CV_32F, ecc.))

    cv::imshow("Average filter",filtered_img);
    cv::waitKey(0);

    // Sobel filter  (is composed by 2 kernels, that are matrices for the gradient on X direction and gradient on Y direction)
    cv::Mat_<float> sobelX_kernel = (cv::Mat_<float>(3, 3) <<
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);

    cv::Mat_<float> sobelY_kernel = (cv::Mat_<float>(3, 3) <<
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1);

    cv::Mat sobelX, sobelY;

    // Note : change the format beacuse magnitude function requires CV_32F or CV_64F images
    cv::filter2D(grey_img, sobelX, CV_32F, sobelX_kernel);
    cv::filter2D(grey_img, sobelY, CV_32F, sobelY_kernel);

    // Function that combine the 2 intensity of the gradient
    cv::magnitude(sobelX, sobelY, filtered_img); 
    // cv::normalize scales the values ​​in the array so that they fall within a specified range.
    cv::normalize(filtered_img, filtered_img, 0, 255, cv::NORM_MINMAX, CV_8U); // cv::NORM_MINMAX (type of normalization)

    cv::imshow("Sobel filter",filtered_img);
    cv::waitKey(0);

    // NON-LINEAR FILTERS:
    // Max filter

    // Define a kernel 3 x 3
    cv::Mat max_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    //
    cv::dilate(grey_img, filtered_img, max_kernel);


    cv::imshow("Max Filter", filtered_img);
    cv::waitKey(0);

    // Min filter

    cv::Mat min_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    //
    cv::erode(grey_img, filtered_img, min_kernel);
    
    
    cv::imshow("Min filter",filtered_img);
    cv::waitKey(0);




    

    return 0;
}


/*
General notes:

    -   Function nameWinow must be used in the case we need to define some viusalization parameters on the
        window in wich we are sharing our image, but if not we can ommit that function since the function
        imshow handle automatically this situation generating a speicfic default window if this doesn't exists

*/
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>

int main(int argc, char **argv){

    // Read the prompted image from the directory and show it
    cv::Mat img = cv::imread(argv[1]);    

    cv::namedWindow("Window1");
    cv::imshow("Window1", img);
    cv::waitKey(0);

    // Convert it into greyscale
    cv::Mat grey_image;

    cv::cvtColor(img, grey_image, cv::COLOR_RGB2GRAY);

    std::cout<<grey_image.channels()<<std::endl;

    cv::namedWindow("Window2");
    cv::imshow("Window2", grey_image);
    cv::waitKey(0);

    // Save it
    cv::imwrite("Garden_grey_scale.png", grey_image);


    return 0;
}
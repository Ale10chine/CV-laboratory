// #include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>

// Struct needed in the means_values function
struct means_values{
    int B;
    int G;
    int R;
};

// The signature is a std way according to OpenCv library
void MouseCallback_T(int event, int x, int y, int flags, void *userdata);

means_values mean_calc(const std::vector<uchar>& kernel_B, const std::vector<uchar>& kernel_G, const std::vector<uchar>& kernel_R);
cv::Mat mask(cv::Mat *ptr, int B, int G, int R);


int main(int argc, char **argv){

    cv::Mat img = cv::imread(argv[1]);
// to comnplete in order to move the window as estabilished
    int screen_width = 1440;
    int window_width = img.cols;

    cv::namedWindow("Window1");
    cv::imshow("Window1", img);
    cv::waitKey(0);
    cv::destroyWindow("Window1");

    // Task3
    cv::namedWindow("Segment T-shirt");
    cv::moveWindow("Segment T-shirt", 0, 0);

    cv::namedWindow("Applying Threshold");
    cv::moveWindow("Applying Threshold", screen_width - window_width,0);

    cv::setMouseCallback("Segment T-shirt",MouseCallback_T, &img);
    
    cv::imshow("Segment T-shirt", img);
    cv::waitKey(0);
    cv::destroyWindow("Segment T-shirt");

    //Task4
    cv::Mat hsv_img;
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

    cv::namedWindow("Segment T-shirt HSV");

    cv::setMouseCallback("Segment T-shirt HSV",MouseCallback_T, &hsv_img);
    
    cv::imshow("Segment T-shirt HSV", hsv_img);
    cv::waitKey(0);


    return 0;
}



// Task3
void MouseCallback_T(int event, int x, int y, int flags, void *userdata){
    if(event == cv::EVENT_LBUTTONDOWN){

        cv::Mat *ptr_img = (cv::Mat *)(userdata); // could hind errors but is more coincice cast
        std::vector<uchar> kernel_B, kernel_G, kernel_R;
        int off = 9 / 2;

        for (int i = y - off; i < y - off + 9; i++){ // For the Robocup.png we have 780 x 1387 (row x cols)
            for (int j = x - off; j < x - off + 9 ; j++){
                if((i >= 0 && i <= ptr_img->rows - 1) && (j >= 0 && j <= ptr_img->cols-1)){
                    kernel_B.push_back((int)ptr_img->at<cv::Vec3b>(i,j) [0]);
                    kernel_G.push_back((int)ptr_img->at<cv::Vec3b>(i,j) [1]);
                    kernel_R.push_back((int)ptr_img->at<cv::Vec3b>(i,j) [2]);
                }
            }
        }

        means_values means = mean_calc(kernel_B, kernel_G, kernel_R);
        std::cout << " Mean_B: " << means.B
                  << " Mean_G: " << means.G
                  << " Mean_R: " << means.R << "\n\n";

        cv::imshow("Applying Threshold", mask(ptr_img,means.B, means.G, means.R));
    }
}

// Mask function
cv::Mat mask(cv::Mat *ptr, int B, int G, int R){

    // Cration of an object that is the clone of the object pointed by the pointer
    cv::Mat m = ptr->clone();

        int T = 25;

        for (int i = 0; i < ptr->rows; i++){
            for (int j = 0; j < ptr->cols; j++){
                // | pixel(B\G\R) - mean (B\G\R) | <= T
                if(std::abs(m.at<cv::Vec3b>(i,j)[0] - B) <= T && std::abs(m.at<cv::Vec3b>(i,j)[1] - G) <= T
                     && std::abs(m.at<cv::Vec3b>(i,j)[2] - R) <= T ){
                    // Set to white 
                        m.at<cv::Vec3b>(i,j)[0] = 255;
                        m.at<cv::Vec3b>(i,j)[1] = 255;
                        m.at<cv::Vec3b>(i,j)[2] = 255;       
                }else{ 
                    // Set to black
                        m.at<cv::Vec3b>(i,j)[0] = 0;
                        m.at<cv::Vec3b>(i,j)[1] = 0;
                        m.at<cv::Vec3b>(i,j)[2] = 0; 
                }
            }
        }
    return m;
}

// Mean calculator
means_values mean_calc(const std::vector<uchar>& kernel_B, const std::vector<uchar>& kernel_G, const std::vector<uchar>& kernel_R){

    int sum_B = 0, sum_G = 0, sum_R = 0;

    for (int i = 0; i < kernel_B.size(); i++){
        sum_B += kernel_B[i];
        sum_G += kernel_G[i];
        sum_R += kernel_R[i];
    }
    means_values means;
    means.B = sum_B / kernel_B.size();
    means.G = sum_G / kernel_G.size();
    means.R = sum_R / kernel_R.size();

    return means;
}
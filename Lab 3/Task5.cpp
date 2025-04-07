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

// NOTE that this function is implemented differently from the other tasks program
means_values mean_calc(cv::Mat *ptr_img, int x , int y); // The function entirely do the calculation of the mean in the neighborhood
// Note also this one is modified in order to manage the threshold on the different HSV channel
cv::Mat mask(cv::Mat *ptr, int B, int G, int R); // Treshold that set 0 or 255 to the input image
cv::Mat mask2(cv::Mat img, const cv::Mat &init_img); // Mask that set to the original value of the inp. img if 255 or (92, 37, 201) otherwise


int main(int argc, char **argv){

    cv::Mat img = cv::imread(argv[1]);

    //Task4-5 (since we are basically exapanding the previous one)
    cv::Mat hsv_img;
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

    cv::namedWindow("Segment T-shirt HSV");

    cv::namedWindow("Applying Threshold");
    cv::namedWindow("Applying Threshold 2");

    cv::setMouseCallback("Segment T-shirt HSV",MouseCallback_T, &hsv_img);
    
    cv::imshow("Segment T-shirt HSV", hsv_img);
    cv::waitKey(0);


    return 0;
}



void MouseCallback_T(int event, int x, int y, int flags, void *userdata){
    if(event == cv::EVENT_LBUTTONDOWN){

        cv::Mat *ptr_img = (cv::Mat *)(userdata); // could hind errors but is more coincice cast

        means_values means = mean_calc(ptr_img, x, y);
        std::cout << " Mean_B: " << means.B
                  << " Mean_G: " << means.G
                  << " Mean_R: " << means.R << "\n\n";

        cv::Mat t_img = mask(ptr_img,means.B, means.G, means.R);
        cv::imshow("Applying Threshold",t_img);

        cv::Mat t2_img =  mask2(t_img, *ptr_img);
        cv::Mat t2_img_rev;
        cv::cvtColor(t2_img, t2_img_rev, cv::COLOR_HSV2BGR);
        cv::imshow("Applying Threshold 2",t2_img_rev);
        
    }
}



// Mask function (Task 3)
cv::Mat mask(cv::Mat *ptr, int B, int G, int R){

    // Cration of an object that is the clone of the object pointed by the pointer
    cv::Mat m = ptr->clone();
    int T = 8;

    for (int i = 0; i < ptr->rows; i++){
        for (int j = 0; j < ptr->cols; j++){
            // | pixel(B\G\R) - mean (B\G\R) | <= T
            if (std::abs(m.at<cv::Vec3b>(i, j)[0] - B) <= T && std::abs(m.at<cv::Vec3b>(i, j)[1] - G) <= T*10 
                && std::abs(m.at<cv::Vec3b>(i, j)[2] - R) <= T*15){
                // Set to white
                m.at<cv::Vec3b>(i, j)[0] = 255;
                m.at<cv::Vec3b>(i, j)[1] = 255;
                m.at<cv::Vec3b>(i, j)[2] = 255;
            }else{
                // Set to black
                m.at<cv::Vec3b>(i, j)[0] = 0;
                m.at<cv::Vec3b>(i, j)[1] = 0;
                m.at<cv::Vec3b>(i, j)[2] = 0;
            }
        }
    }
    return m;
}

// Mask2 (Task5)
cv::Mat mask2(cv::Mat img, const cv::Mat &init_img){
    // Here we don't need to clone nothing sice in the signature of the function we have already
    // done a copy of the image in which we have applied the first threshold
    
        for (int i = 0; i < img.rows; i++){
            for (int j = 0; j< img.cols; j++){
                // If the pixel image is white set this fixed value assigned as request from task5a (92,37,201)
                if(img.at<cv::Vec3b>(i,j)[0] == 255 && img.at<cv::Vec3b>(i,j)[1] == 255 && img.at<cv::Vec3b>(i,j)[2] == 255){

                    img.at<cv::Vec3b>(i,j)[0] = 92;
                    img.at<cv::Vec3b>(i,j)[1] = 37;
                    img.at<cv::Vec3b>(i,j)[2] = 201;
                }else{
                    img.at<cv::Vec3b>(i,j)[0] = init_img.at<cv::Vec3b>(i,j)[0];
                    img.at<cv::Vec3b>(i,j)[1] = init_img.at<cv::Vec3b>(i,j)[1];
                    img.at<cv::Vec3b>(i,j)[2] = init_img.at<cv::Vec3b>(i,j)[2];
                }
            }
            
        }
    return img;
}

// Mean calculator (Task 2)
means_values mean_calc(cv::Mat *ptr_img, int x , int y){

    // Memorizing the neighboorhood values
    std::vector<uchar> kernel_B, kernel_G, kernel_R;
    int off = 9 / 2;

    for (int i = y - off; i < y - off + 9; i++){ // For the Robocup.png we have 780 x 1387 (row x cols)
        for (int j = x - off; j < x - off + 9; j++){
            if ((i >= 0 && i <= ptr_img->rows - 1) && (j >= 0 && j <= ptr_img->cols - 1)){
                kernel_B.push_back((int)ptr_img->at<cv::Vec3b>(i, j)[0]);
                kernel_G.push_back((int)ptr_img->at<cv::Vec3b>(i, j)[1]);
                kernel_R.push_back((int)ptr_img->at<cv::Vec3b>(i, j)[2]);
            }
        }
    }

    // Computing the mean
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
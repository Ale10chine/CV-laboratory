// #include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>

// The signature is a std way according to OpenCv library
void MouseCallback(int event, int x, int y, int flags, void *userdata);
void MouseCallback_2(int event, int x, int y, int flags, void *userdata);

int main(int argc, char **argv){

    cv::Mat img = cv::imread(argv[1]);

    // Task1
    cv::namedWindow("Window1");

    // Sets mouse handler for the specified window. Is a loop function, remains open until the window remain open and  
    // we can basically give some imput, in this case click of the mouse
    
    //cv::setMouseCallback("Window1", MouseCallback, nullptr);

    // NOTE: we call this function before showing the image since this function as said before remains in "listening", lopping until
    // the closure of the window    
    cv::setMouseCallback("Window1", MouseCallback, &img);  
    cv::imshow("Window1", img);
    cv::waitKey(0);

    // Task2
    cv::namedWindow("Window2");

    cv::setMouseCallback("Window2", MouseCallback_2, &img);
    
    cv::imshow("Window2", img);
    cv::waitKey(0);

    return 0;
}

// Task1
void MouseCallback(int event, int x, int y, int flags, void *userdata){
    if (event == cv::EVENT_LBUTTONDOWN){
        // Convert the userdata pointer in a image pointer, in practice i create a pointer that points on my input image

        //cv::Mat *ptr_img = static_cast<cv::Mat *>(userdata);// Equal cast of the below but done at compile time, is a safe cast 
        cv::Mat *ptr_img = (cv::Mat *)(userdata); // could hind errors but is more coincice cast 

        std::cout << " x (col) =  " << x << " y (row) = " << y << std::endl;
        /* Equal code of below but with two problems explained in the NOTEs
        std::cout<<" B : "<<(int)ptr_img->at<cv::Vec3b>(x,y)[0] 
                 <<" G : "<<(int)ptr_img->at<cv::Vec3b>(x,y)[1]
                 <<" R : "<<(int)ptr_img->at<cv::Vec3b>(x,y)[2]<<std::endl;
        */

        // NOTE: The GUI is implemented in a way s.t we have to invert x,y axes so is more useful to use cv::Point
        // that returns us directly the correct coordinates
        // NOTE: you have to do the cast because cout treat Vec3b as a ASCII character
        std::cout<<" B : "<<(int)ptr_img->at<cv::Vec3b>(cv::Point(x,y)) [0] 
                 <<" G : "<<(int)ptr_img->at<cv::Vec3b>(cv::Point(x,y)) [1]
                 <<" R : "<<(int)ptr_img->at<cv::Vec3b>(cv::Point(x,y)) [2]<<"\n\n";
    

    }
}

// Task2
void MouseCallback_2(int event, int x, int y, int flags, void *userdata){
    if(event == cv::EVENT_LBUTTONDOWN){

        cv::Mat *ptr_img = (cv::Mat *)(userdata); // could hind errors but is more coincice cast
        int off = 9 / 2;
        std::vector<uchar> kernel_B;
        std::vector<uchar> kernel_G;
        std::vector<uchar> kernel_R;

        // NOTE that the GUI inverts x,y so bascially x corresponds to the column, while y corresponds to the  rows
        // Common sense : (x := rows, y := cols), while for the GUI we have : (y := rows, x := cols)
        std::cout << " x (col) =  " << x << " y (row) = " << y << std::endl;
        // Print of the dimension of the image (780 x 1387)
        std::cout << " rows i: "<<ptr_img->rows<<" cols j: "<<ptr_img->cols<<std::endl;

        for (int i = y - off; i < y - off + 9; i++){ // For the Robocup.png we have 780 x 1387 (row x cols)
            for (int j = x - off; j < x - off + 9 ; j++){
                if((i >= 0 && i <= ptr_img->rows - 1) && (j >= 0 && j <= ptr_img->cols-1)){
                    /* 
                    std::cout << " B : " << (int)ptr_img->at<cv::Vec3b>(cv::Point(j,i))[0]
                              << " G : " << (int)ptr_img->at<cv::Vec3b>(cv::Point(j,i))[1]
                              << " R : " << (int)ptr_img->at<cv::Vec3b>(cv::Point(j,i))[2] << std::endl;
                    */
                    kernel_B.push_back((int)ptr_img->at<cv::Vec3b>(cv::Point(j,i)) [0]);
                    kernel_G.push_back((int)ptr_img->at<cv::Vec3b>(cv::Point(j,i)) [1]);
                    kernel_R.push_back((int)ptr_img->at<cv::Vec3b>(cv::Point(j,i)) [2]);
                }
            }
        }
        int sum_B = 0, sum_G = 0, sum_R = 0;
        int mean_B = 0, mean_G = 0, mean_R = 0;
        for(int i = 0; i < kernel_B.size(); i++){
            sum_B += kernel_B[i];
            sum_G += kernel_G[i];
            sum_R += kernel_R[i];
            }
        mean_B = sum_B / kernel_B.size();  
        mean_G = sum_G / kernel_G.size();
        mean_R = sum_R / kernel_R.size();

        std::cout<<" Mean_B: "<<mean_B
                 <<" Mean_G: "<<mean_G
                 <<" Mean_R: "<<mean_R<<"\n\n";        
    }
}


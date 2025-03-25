#include "ImageFilters.h"
#include <iostream>


// MAX FILTER
void max_filter(cv::Mat &img , int k_size){
    if(k_size % 2 == 0){
        std::cout<<"The Kernel size must be odd"<<std::endl;
        return;
    }

    int off = k_size/2;
    cv::Mat img_clone = img.clone();
    // Row major scanning the image
    for (int i = 0; i < img.rows; i++){
        for (int j = 0; j < img.cols; j++){
            // In practice we reason vectorizing the kernel
            std::vector<uchar> kernel;
            //Scanning row major the kernel centered in pos (i,j)
            for (int x = i - off; x < i - off + k_size; x++){
                for (int y = j - off; y < j - off + k_size; y++){
                
                    if(x >= 0 && x < img.rows && y >= 0  && y < img.cols){
                        kernel.push_back(img.at<uchar>(x, y));
                    }
                }                
            }
            //Sort the entry of the kernel in decreasing order (Max to min)
            std::sort(kernel.begin(), kernel.end(), std::greater<uchar>());

            //Applying the filter to each channel in the clone image
            //If we don't do this we compromise the current image
            img_clone.at<uchar>(i,j) = kernel[0];

        }
        
    }
    //At the end we apply the modfifies at the original image
    img = img_clone;
}

// MIN FILTER
void min_filter(cv::Mat &img , int k_size){
    if(k_size % 2 == 0){
            std::cout<<"The Kernel size must be odd"<<std::endl;
            return;
        }

        int off = k_size/2;
        cv::Mat img_clone = img.clone();

        for (int i = 0; i < img.rows; i++){
            for (int j = 0; j < img.cols; j++){
            
                std::vector<uchar> kernel;

                for (int x = i - off; x < i - off + k_size; x++){
                    for (int y = j - off; y < j - off + k_size; y++){
                    
                        if(x >= 0 && x < img.rows && y >= 0  && y < img.cols){
                            kernel.push_back(img.at<uchar>(x, y));
                        }
                    }                
                }
                //Sort the entry of the kernel in increasing order (Min to max)
                std::sort(kernel.begin(), kernel.end()); // default = crescent order

                img_clone.at<uchar>(i,j) = kernel[0];

            }
            
        }
        //At the end we apply the modfifies at the original image
        img = img_clone;


}

// MEDIAN FILTER
void median_filter(cv::Mat &img , int k_size){
    if(k_size % 2 == 0){
            std::cout<<"The Kernel size must be odd"<<std::endl;
            return;
        }

        int off = k_size/2;
        cv::Mat img_clone = img.clone();

        for (int i = 0; i < img.rows; i++){
            for (int j = 0; j < img.cols; j++){
                
                std::vector<uchar> kernel;

                for (int x = i - off; x < i - off + k_size; x++){
                    for (int y = j - off; y < j - off + k_size; y++){
                    
                        if(x >= 0 && x < img.rows && y >= 0  && y < img.cols){
                            kernel.push_back(img.at<uchar>(x, y));
                        }
                    }                
                }
                // Note: median filter is implemented taking the media value of a crescent sorted vector
                std::sort(kernel.begin(), kernel.end());

                // We take the median value in ordered vectorize kernel
                img_clone.at<uchar>(i,j) = kernel[kernel.size()/2];

            }
            
        }
        //At the end we apply the modfifies at the original image
        img = img_clone;
}



//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


void chess_designer(cv::Mat &img, int pix);

int main(int argc, char **argv)
{

// TASK 5-(a) --------------------------------------------------------------------------------------
    //Create two images 256x256 8 bit depth
    cv::Mat img_a(256,256, CV_8U);   
    cv::Mat img_b(256,256, CV_8U);

    for (int i = 0; i < img_a.rows; ++i)
    {
        for (int j = 0; j < img_a.cols; ++j)
        {
            img_a.at<unsigned char> (i,j) = j;
        }
    }

    for (int i = 0; i < img_b.rows; ++i)
    {
        for (int j = 0; j < img_b.cols; ++j)
        {
            img_b.at<unsigned char> (i,j) = i;
        }
    }

    cv::namedWindow("Example 1");
    cv::imshow("Example 1", img_a); 

    cv::namedWindow("Example 2");
    cv::imshow("Example 2", img_b); 

    // Save the waitKey return value in a char var and print it before ending the run    
    char var = cv::waitKey(0);
    std::cout<<"Variable that contains the content of waitKey: \t"<<var<<std::endl;

// TASK 5-(b) --------------------------------------------------------------------------------------
    //Create two images 300x300 8 bit depth
    cv::Mat img_c(300,300, CV_8U);   
    cv::Mat img_d(300,300, CV_8U);


    chess_designer(img_c, 20);
    chess_designer(img_d, 50);
    

    cv::namedWindow("Example 3");
    cv::imshow("Example 3", img_c);
    cv::namedWindow("Example 4");
    cv::imshow("Example 4", img_d);


    cv::waitKey(0);

    return 0;
}

// Function that draws a chessboard (chosing the wished pixels in input)
void chess_designer(cv::Mat &img, int pix)
{
    int m = 0;             // counter that increments each 20 units going right to the matrix (20 columns)
    bool alternate = true; // in this way we can invert the pattern every 20 units going down to the matrix (20 rows)

    // Row major scan
    for (int i = 0; i < img.rows; ++i)
    {
        // Every #pix rows invert the pattern changing the 'alternate' bool value
        if (i % pix == 0 && alternate == true)
        {
            alternate = false;
        }
        else if (i % pix == 0 && alternate == false)
        {
            alternate = true;
        }

        for (int j = 0; j < img.cols; ++j)
        {
            if (alternate)
            {
                if (m % 2 == 0)
                {
                    if (j < pix * (m + 1))
                        img.at<unsigned char>(i, j) = 0; // setting all pixels that satisfy that condition to black
                }
                else
                {
                    img.at<unsigned char>(i, j) = 255; // white pixels
                }
            }
            else // doing the exact opposite of above
            {
                if (m % 2 != 0)
                {
                    if (j < pix * (m + 1))
                        img.at<unsigned char>(i, j) = 0;
                }
                else
                {
                    img.at<unsigned char>(i, j) = 255;
                }
            }

            if (j == (pix * (m + 1)))
                m++;
        }
        m = 0;
    }
}

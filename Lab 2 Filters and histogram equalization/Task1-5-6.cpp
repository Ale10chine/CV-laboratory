//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>


void plotHistogram(const cv::Mat& hist, const std::string& windowName);

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


    cv::Mat hist;

    // Parameters for calcHist
    int channels[] = {0};          // Canale 0 (grayscale)
    int histSize[] = {256};         // 256 bin
    float range[] = {0, 256};       // Range per immagini 8-bit
    const float* ranges[] = {range};

    // Calculate the Histogram of a given image
    calcHist(&img, 1, channels, cv::Mat(), hist, 1, histSize, ranges);  // Input images, # of images, mask (Mat() to put a void mask, )

    cv::namedWindow("Histogram");
    plotHistogram(hist, "Histogram");

    // Apply the histogram equalization function
    cv::Mat equalized_img;
    cv::equalizeHist(grey_image,equalized_img);

    cv::namedWindow("Equalization");
    cv::imshow("Equalization",equalized_img);
    cv::waitKey(0);



    return 0;
}

/*

void calcHist(
    const Mat* images,      // Array di immagini (input)
    int nimages,           // Numero di immagini 
    const int* channels,   // Canali da considerare (es. [0] per grayscale)
    InputArray mask,       // Maschera opzionale (regione di interesse)
    OutputArray hist,      // Istogramma calcolato (output)
    int dims,             // Dimensionalità dell'istogramma (es. 1D, 2D)
    const int* histSize,  // Numero di bin per canale (es. [256])
    const float** ranges, // Intervalli dei valori (es. [0, 256])
    bool uniform = true,  // Se i bin sono uniformi
    bool accumulate = false // Se accumulare risultati multipli
);

*/


void plotHistogram(const cv::Mat& hist, const std::string& windowName = "Histogram") {
    // Create a void (blanck) image for the plot
    int hist_w = 512, hist_h = 400;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Histogram normalization in order to adapt at the high of the plot
    cv::Mat normalizedHist;
    cv::normalize(hist, normalizedHist, 0, histImage.rows, cv::NORM_MINMAX);

    // Draw the bins of thehistogram
    int bin_w = cvRound((double)hist_w / hist.rows);
    for (int i = 1; i < hist.rows; i++) {
        cv::line(
            histImage,
            cv::Point(bin_w * (i-1), hist_h - cvRound(normalizedHist.at<float>(i-1))),
            cv::Point(bin_w * i, hist_h - cvRound(normalizedHist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0  // Red color, width = 2
        );
    }

    // Plot the histogram
    cv::imshow(windowName, histImage);
    cv::waitKey(0);
}
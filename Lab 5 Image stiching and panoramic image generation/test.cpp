// Test script to understand on how compute the task1 requirements only for 2 consecutive images
// of the dataset


#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>
#include <unistd.h> // for using getopt()
#include <dirent.h> // for DIR, c library
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp> // for using SIFT
#include <opencv2/calib3d.hpp> // To use findHomography
#include "PanoramicUtils/header/panoramic_utils.h" // to include the cylindric distorsion function
#include <algorithm> // to use the sorting algoritm for std structures

// Input : number of input in command line argc, content of the command line argv, path dir
// Write in the string dir passed for reference the path of the folder passed from command line
void parse_command_line(int argc, char* argv[], std::string& dir_path) {
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
               dir_path= optarg;
                break;
            case '?':
                std::cerr << "Usage: " << argv[0] << " -p <path> " << std::endl
                          << "  Where:" << std::endl
                          << "  -p is ... dir path" << std::endl;
                break;
        }
    }
}

// Input : path string as const reference, a vector of string in which would be contained the names of the files
// Open a directory, read the file name and memorize it (pushing back) into vector filenames
void get_all_filenames(const std::string& dir_path, std::vector<std::string>& filenames) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dir_path.c_str())) != NULL) {
        // process all the files insider the directory
        while ((ent = readdir (dir)) != NULL) {
            std::string file_name = ent->d_name;
            // Don't consider the current directory '.' and the parent ".."
            if (file_name == "." || file_name == "..") {
                continue;
            }
            if (*(dir_path.end() - 1) == '/') {
                filenames.push_back(dir_path + file_name);
            } else {
                filenames.push_back(dir_path + "/" + file_name);
            }
        }
        closedir(dir); // Close the directory.
    }
}

void printMatrix(const cv::Mat& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            std::cout << matrix.at<double>(i, j) << " ";
        }
        std::cout << std::endl;
    }
}


int main(int argc, char** argv){

    //Check if the command line input has the correct number of input 
    if(argc != 3){
        std::cout<<"Too short input ..."<<std::endl;
        return -1;
    }

    // String to store the dir_path from the commandline
    std::string dir_path;
    // Vector to store in a string vector all the name of the images
    std::vector<std::string> filenames;

    parse_command_line(argc, argv, dir_path);
    std::cout<<"Dir name: "<< dir_path<<std::endl;

    get_all_filenames(dir_path, filenames);

    // Reorder in a crescent way the strings, we want consecutive order of images to implement
    // the lab task
    std::sort(filenames.begin(), filenames.end());

    // Check if the images are loaded correctly with a simple printing
    for (int i = 0; i < filenames.size(); i++)
    {
        std::cout<<"Filneame ["<<i<<"] "<<filenames[i]<<std::endl;
    }
    
    // Create a std vector of images in which store the images
    std::vector<cv::Mat> images;
    for (int i = 0; i < filenames.size(); i++)
    {
        images.push_back( cv::imread(filenames[i], cv::IMREAD_COLOR));
    }
    
    cv::namedWindow("Starting image");
    cv::imshow("Starting image", images[2]);


    // Cilindric Projection of all the images
    cv::namedWindow("Image after distorsion1");
    images[2] =  cylindricalProj(images[2], 33);
    cv::imshow("Image after distorsion1", images[2]);

    cv::namedWindow("Image after distorsion2");
    images[3] = cylindricalProj(images[3], 33);
    cv::imshow("Image after distorsion2", images[3]);
    cv::waitKey(0);

    // Feature extraction with SIFT:  we do the matching from 1 dataset to reconstruct the entire 360 degree
    // To do that the idea is to compute the matches between the first and the second image, the matches
    // between the second and the third images and so on

    // SIFT detector object, default call to the constructor
    cv::Ptr<cv::SIFT> detector =  cv::SIFT::create();

    // Descriptor matcher object, default call to the constructor
    //cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create(cv::NORM_L2); // Default setting is to use L2 norm here

    // Vectors of keypotins for image1 and image2
    std::vector<cv::KeyPoint> keypoint_image1;
    std::vector<cv::KeyPoint> keypoint_image2;
    
    // Descriptors for image1 and image2
    cv::Mat descriptors_image1;
    cv::Mat descriptors_image2;

    // Feature extraction for both objects    
    // For every match, there will be a row in the descriptor of 128 entry in wich is conserved
    // the descritor relative to that feature
    detector->detectAndCompute(images[2], cv::noArray(), keypoint_image1, descriptors_image1);
    detector->detectAndCompute(images[3], cv::noArray(), keypoint_image2, descriptors_image2);
   
    // Vector in which store the matches between the 2 images
    std::vector<std::vector<cv::DMatch>> knn_matches;

    // Return a vector of vector (matrix) of matches, the parm 2 means the size of the matrix, so returns
    // for each features of the first image descriptr the 2 nearest matches in the second image descriptor
    matcher->knnMatch(descriptors_image1, descriptors_image2, knn_matches, 2);

    // Match filtering based on distance ratio (Lowe's ratio test is used for selecting best matches,
    // deciding looking at the pair of nearest matches (given by tshe param k = 2 in the function above), in practical terms if
    // the two matches are very close the matching is good second this kind of filter
    std::vector<cv::DMatch> good_matches;
    float ratio_thresh = 0.3f;
    // Save the match of the input image, and output image in two properly vectores 
    // We use Point2f since we want better precision in the features
    std::vector<cv::Point2i> match_image1, match_image2;
    for (size_t i = 0; i < knn_matches.size(); ++i) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
            
        }
    }
    for (int i = 0; i < match_image1.size(); i++)
    {
        std::cout<<match_image1[i]<<" ";
    }

    for(const auto& match : good_matches){
        // Storing the keypoint used to do the match
        // Use data member .pt to return the coordinates (Point2f obj) of that index
        match_image1.push_back(keypoint_image1[match.queryIdx].pt);
        match_image2.push_back(keypoint_image2[match.trainIdx].pt);
    }
    // Drawing matches with 2 images side by side 
    cv::Mat img_matches;
    cv::drawMatches(images[2], keypoint_image1, images[3], keypoint_image2,
                    good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                    std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //  Feature matching visualization
    cv::imshow("Feature Matching", img_matches);
    cv::waitKey();


    // RANSAC
    // Homographic functiion, once we store the matches between consecutives images we compute the homography
    // between 2 scenes, in this way we could understand the amount of translation between the 2
    // This allow us to avoid the usage of the RANSC, this other approach wil calculate in rpactice
    // the average distance between matches of consecutive images
    
    cv::Mat H;
    // To have positive value of the translation is important to pass first the second image and then the first
    H = cv::findHomography(match_image2, match_image1, cv::RANSAC); 
    std::cout<<"H = "<< std::endl; 
    printMatrix(H); 
    
    // Now the output of the H homography matrix will be something like that
    // The matrix H has the following form, where (x, y) are the coordinates in image 1,
    // (x', y') are the corresponding coordinates in image 2, and 's' is a scale factor:
    //
    //  s * [x']   [h11 h12 h13] [x]
    //      [y'] = [h21 h22 h23] [y]
    //      [1 ]   [h31 h32 h33] [1]
    // Where the translation components are given by h13 (x translation), h23 (y translation)
    std::cout<< "Width "<< images[2].cols<<std::endl;
    std::cout<< "Width "<< images[3].cols<<std::endl;



    // Finally at the end, with the properly implementation, we will able to construct from scratch
    // starting from a blank large image, the composition of the final 360 degree image composed
    // by all the images in a such way to exploit the translation given by the homography function
    // We in practice glue the next image considering the correct sliding given bascially from the homograpy
    // we will manipulate this transformation homografy matrxi in order to extract the traslational component

    // Creation of the black long image
    // row size(must be equal for all the image), cols size (sum of all the images - sum of all sliding), image type , value of all pixels
    cv::Mat base_image = cv::Mat(images[2].rows, 2*images[2].cols, images[2].type(), cv::Scalar(0, 0, 0));

    // Coping inside all the images with the rispective translation
    cv::Rect roi_dest0(0, 0 , images[2].cols, images[2].rows);
    std::cout<<"traslation : "<< (int) H.at<double>(0,2)<<std::endl;
    std::cout<<"starting point roi : "<< images[2].cols - (int) H.at<double>(0,2)<<std::endl;
    
    cv::Rect roi_dest1((int) H.at<double>(0,2), 0 , images[2].cols, images[2].rows);

    images[2].copyTo(base_image(roi_dest0));
    images[3].copyTo(base_image(roi_dest1));
    
   
    

    cv::imshow("360 degree image", base_image);
    cv::waitKey(0);

    return 0;
}
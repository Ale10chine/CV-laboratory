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
void parse_command_line(int argc, char* argv[], std::string& dir_path);

// Input : path string as const reference, a vector of string in which would be contained the names of the files
// Open a directory, read the file name and memorize it (pushing back) into vector filenames
void get_all_filenames(const std::string& dir_path, std::vector<std::string>& filenames); 

// Function to print H matrix
void printMatrix(const cv::Mat& matrix); 

// Function to equalize histogram equalization to an input images vector
std::vector<cv::Mat> equalize_images(const std::vector<cv::Mat>& images);

// Componing a 360 degree image starting from a dataset
int main(int argc, char** argv){

// ---------------- LOADING OF THE IMAGES ----------------
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

 // ---------------- APPLICATION OF DISTORSION USING CYLINDRIC PROJECTION ----------------
   
    // Cilindric Projection of all the images
    for (int i = 0; i < images.size(); i++)
    {   
        // Passing image and half of the FoV of the camera
        images[i] = cylindricalProj(images[i], 33); 
    }
    

 // ---------------- FEATURE EXTRACTION ----------------

    // Feature extraction with SIFT: we do the matching from 1 dataset to reconstruct the entire 360 degree
    // To do that the idea is to compute the matches between the first and the second image, the matches
    // between the second and the third images and so on

    // SIFT detector object, default call to the constructor
    cv::Ptr<cv::SIFT> detector =  cv::SIFT::create();

    // Descriptor matcher object, default call to the constructor
    //cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create(cv::NORM_L2); // Default setting is to use L2 norm here

    // Vectors of keypotins for all images (vector of vector aka matrix of features)
    std::vector<std::vector <cv::KeyPoint>> keypoint_images;
    //  std::vector<cv::KeyPoint> keypoint_image2;
    
    // Descriptors for all the image (vector of mat of descriptors)
    std::vector<cv::Mat> descriptor_images;

    // Feature extraction for  all the object
    // For every match, there will be a row in the descriptor of 128 entry in wich is conserved
    // the descritor relative to that feature

    std::vector<cv::KeyPoint> keypoint_tmp;
    cv::Mat descriptors_tmp;

    for (int i = 0; i < images.size(); i++)
    {   
        detector->detectAndCompute(images[i], cv::noArray(), keypoint_tmp, descriptors_tmp);
        keypoint_images.push_back(keypoint_tmp);
        descriptor_images.push_back(descriptors_tmp);
    }


 // ---------------- FEATURE MATCHING AND COMPARISON BETWEEN CONSECUTIVE IMAGES TO DERIVE THE TRANSLATION ----------------

    // Our goal in the end is to recover the h13 term stored in each H (homography matrix)
    // Remember that we will find an H matrix for each consecutive couple of images
    // Here we initialize a vector to store this components for each coupling in the current dataset
    // This information will be usful at the end in order to know how we can create from scratch the 
    // global 360 degree image, merging all the images in this one with the correct overlappings
    // and distorsion applyed
    std::vector<int> x_trans;
    // Now the output of the H homography matrix will be something like that
    // The matrix H has the following form, where (x, y) are the coordinates in image 1,
    // (x', y') are the corresponding coordinates in image 2, and 's' is a scale factor:
    //
    //  s * [x']   [h11 h12 h13] [x]
    //      [y'] = [h21 h22 h23] [y]
    //      [1 ]   [h31 h32 h33] [1]
    // Where the translation components are given by h13 (x translation), h23 (y translation)

    // For our reasonament made here above we are interest on iterating only until n - 1
    // since in the last iteration we will tackle with the n - 1 and n images, remember that 
    // we work always with the i and i + 1 images every iteration 
    for (int i = 0; i < images.size() - 1; i++)
    {
        // Vector in which store the matches between 2 consecutive images:
        // 0 - 1 , 1 - 2, 3 - 4 , 5 - 6 , ...
        std::vector<std::vector<cv::DMatch>> knn_matches;

        // Return a vector of vector (matrix) of matches, the parm 2 means the size of the matrix, so returns
        // for each features of the first image descriptr the 2 nearest matches in the second image descriptor
        matcher->knnMatch(descriptor_images[i], descriptor_images[i + 1], knn_matches, 2);

        // Match filtering based on distance ratio (Lowe's ratio test is used for selecting best matches,
        // deciding looking at the pair of nearest matches (given by tshe param k = 2 in the function above), in practical terms if
        // the two matches are very close the matching is good second this kind of filter
        std::vector<cv::DMatch> good_matches;
        float ratio_thresh = 0.75f;
        // Save the match of the input image, and output image in two properly vectores
        // We use Point2f since we want better precision in the features
        std::vector<cv::Point2i> match_image1, match_image2;
        for (size_t i = 0; i < knn_matches.size(); ++i)
        {
            if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
            {
                good_matches.push_back(knn_matches[i][0]);
            }
        }
        for (int i = 0; i < match_image1.size(); i++)
        {
            std::cout << match_image1[i] << " ";
        }

        for (const auto &match : good_matches)
        {
            // Storing the keypoint used to do the match
            // Use data member .pt to return the coordinates (Point2f obj) of that index
            match_image1.push_back(keypoint_images[i][match.queryIdx].pt);
            match_image2.push_back(keypoint_images[i + 1][match.trainIdx].pt);
        }
        
        // Homographic functiion, once we store the matches between consecutives images we compute the homography
        // between 2 scenes, in this way we could understand the amount of translation between the 2
        // This allow us to avoid the usage of the RANSC, this other approach wil calculate in rpactice
        // the average distance between matches of consecutive images

        cv::Mat H;
        H = cv::findHomography(match_image2, match_image1, cv::RANSAC);
        std::cout << "H = " << std::endl;
        printMatrix(H);
        x_trans.push_back((int)H.at<double>(0, 2));
        
    }
    std::cout << "Vector containing all the translation: \n ";
    std::cout << "size of the vector: " << x_trans.size() << std::endl;
    for (int i = 0; i < x_trans.size(); i++)
    {
        std::cout << x_trans[i] << " ";
    }
    std::cout<<std::endl;


 // ---------------- 360 DEGREE IMAGE CREATION ----------------

    // Finally at the end, with the properly implementation, we will able to construct from scratch
    // starting from a blank large image, the composition of the final 360 degree image composed
    // by all the images in a such way to exploit the translation given by the homography function
    // We in practice glue the next image considering the correct sliding given bascially from the homograpy
    // we will manipulate this transformation homografy matrxi in order to extract the traslational component

    // Creation of the black long image
    // row size(must be equal for all the image), cols size (sum of all the images - sum of all sliding), image type , value of all pixels

    // Images equalization
    images = equalize_images(images);

    int tot_translation;
    for (int i = 0; i < x_trans.size(); i++)
    {
       tot_translation += x_trans[i];
    }

    // widt = tot_images width + total_translation (where notice this value is negative!)
   //int width = images.size() * images[0].cols - (tot_translation);
   int width = images[0].cols + tot_translation;
   std::cout<<"Width of the base image = "<<width<<std::endl;

   cv::Mat base_image = cv::Mat(images[0].rows, width, images[0].type(), cv::Scalar(0, 0, 0));

   // Coping inside the base image all the images with the rispective translation
   int tmp_width;
   std::vector<cv::Rect> roi;

   for (int i = 0; i < images.size() - 1; i++)
   {
       tmp_width += x_trans[i];
       cv::Rect roi_dest_i(tmp_width, 0, images[i].cols, images[i].rows);
       roi.push_back(roi_dest_i);
   }

/*
    // Merge from the last to the first
   for (int i = images.size() - 1; i > 0; i--)
   {
       images[i].copyTo(base_image(roi[i - 1]));
   }
   cv::Rect roi_dest_0(0, 0, images[0].cols, images[0].rows);
   images[0].copyTo(base_image(roi_dest_0));

*/
    // Merge from the first to the lasts
   cv::Rect roi_dest_0(0, 0, images[0].cols, images[0].rows);
   images[0].copyTo(base_image(roi_dest_0));

   for (int i = 1; i < images.size(); i++)
   {
       images[i].copyTo(base_image(roi[i - 1]));
   }
   std::cout<<"Image with "<< images[0].cols <<"\n";

   cv::imshow("360 degree image", base_image);
   cv::waitKey(0);
   // Image saving
   cv::imwrite("out/out.png", base_image);

   return 0;
}

// FUNCTIONS
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

std::vector<cv::Mat> equalize_images(const std::vector<cv::Mat>& images) {
    std::vector<cv::Mat> equalized_images(images.size());
    for (size_t i = 0; i < images.size(); ++i) {
        cv::Mat gray_image;
        if (images[i].channels() > 1) {
            cv::cvtColor(images[i], gray_image, cv::COLOR_BGR2GRAY);
        } else {
            gray_image = images[i].clone();
        }
        cv::equalizeHist(gray_image, equalized_images[i]);
    }
    return equalized_images;
}


/*
Notes: 

RANSAC (RANdom SAmple Consensus) is a robust iterative algorithm to estimate the parameters of a
mathematical model from a dataset containing outliers. It randomly selects a subset of data (sample),
estimates the model, and checks which other points in the original set are consistent with this model
(inliers). It repeats this process multiple times, keeping the model with the most inliers. It's used
in the homography function.

Homography is a planar geometric transformation (a 3x3 matrix) that relates corresponding points
between two different images of the same plane or a planar scene. It allows remapping points from one
image to their corresponding points in the other, accounting for rotation, translation, scale, and perspective.
OpenCV uses findHomography with RANSAC to find the robust homography between two sets of corresponding points,
also identifying inliers (good matches) and outliers (incorrect matches).

*/
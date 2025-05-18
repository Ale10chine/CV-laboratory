#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp> // For the cvtColor function
#include <iostream>
#include <dirent.h> // for DIR, c library
#include <opencv2/calib3d.hpp> // for findChessboardCorners

// Input : path string as const reference, a vector of string in which would be contained the names of the files
// Open a directory, read the file name and memorize it (pushing back) into vector filenames
void get_all_filenames(const std::string& dir_path, std::vector<std::string>& filenames);

// Calibration of the camera process
int main(int argc, char **argv)
{

    // ---------------- LOADING OF THE IMAGES ----------------

    // Vector to store in a string vector all the name of the images
    std::vector<std::string> filenames;

    std::string dir_path = "data/checkerboard_images";
    std::cout << "Dir name: " << dir_path << std::endl;

    get_all_filenames(dir_path, filenames);

    std::sort(filenames.begin(), filenames.end());

    // Check if the images are loaded correctly with a simple printing
    for (int i = 0; i < filenames.size(); i++)
    {
        std::cout << "Filneame [" << i << "] " << filenames[i] << std::endl;
    }

    // Create a std vector of images in which store the  greyscales images
    std::vector<cv::Mat> images;
    for (int i = 0; i < filenames.size(); i++)
    {
        images.push_back(cv::imread(filenames[i], cv::IMREAD_GRAYSCALE));
    }

    // ---------------- DETECTS  THE CHECKERBOARD INTERSECTIONS IN EACH IMAGE ----------------

    // Create a vector of 42 (6*7) cv::Point3f (which is similar to a float32)
    std::vector<cv::Point3f> objp(6 * 5);

    // 3D points: we want to store 3D points in this way
    // Point 0: (0, 0, 0)
    // Point 1: (1, 0, 0)
    // Point 2: (2, 0, 0)
    // Point 3: (3, 0, 0)
    // Point 4: (4, 0, 0)
    // Point 5: (5, 0, 0)
    // Point 6: (0, 1, 0)
    // Point 7: (1, 1, 0)
    // Point 8: (2, 1, 0)
    // Point 9: (3, 1, 0)
    // ....
    for (int y = 0; y < 5; ++y)
    { // Iterate over the height (number of rows of internal corners)
        for (int x = 0; x < 6; ++x)
        { // Iterate over the width (number of columns of internal corners)
            int index = y * 6 + x;
            objp[index] = cv::Point3f(static_cast<float>(x), static_cast<float>(y), 0.0f);
        }
    }
    // vectors of vectors of 3D points and 2D points
    // In practice for each image we have a vector of 3D and 2D points
    std::vector<std::vector<cv::Point3f>> obj_points;
    std::vector<std::vector<cv::Point2f>> images_points;

    // interior number of corners (cols - 1, row - 1 of the chessboard),
    // look at the image on how we fix usally the r.f on the 3d world, we don't considere the
    // entire chessboard since the algorithm behind called by findChessboardCorners find corners
    // where two black squares meet each other
    cv::Size patternsize(6, 5); //  We are expecting a chessboard with 6 internal corners along the width and 5 along the height
    int patt = 0;

    for (int i = 0; i < images.size() ; i++)
    {
        // source image
        cv::Mat in_image = images[i];

        //  Corners filled in case of patterfound by findChessboardCorners
        std::vector<cv::Point2f> corners;

        // Function to find a Chessboard pattern of specific size in the input
        //    - in_image: The input grayscale image.
        //    - patternsize: The size of the chessboard pattern.
        //    - corners:  The vector where the coordinates of the detected corners will be stored.
        //    - flags: A combination of flags to optimize detection.
        // 2D Points detected:  we will obtain somthing like this
        // Point 0: (543.862, 576.276)
        // Point 1: (616.34, 579.922)
        // Point 2: (693.335, 584.11)
        // Point 3: (774.587, 588.532)
        // Point 4: (859.875, 593.61)
        // ...
        bool patternfound = cv::findChessboardCorners(in_image, patternsize, corners,
                                                      cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
        if (patternfound)
        {
            // cv::cornerSubPix: Refines the initial corner detections to sub-pixel accuracy.
            // This makes the corner coordinates more precise than integer pixel values,
            // which is crucial for accurate camera calibration.
            // - in_image: The source image.
            // - corners: The initial coordinates of the corners.
            // - cv::Size(11, 11): The size of the search window for refinement.
            // - cv::Size(-1, -1): Dead zone (not used, set to (-1, -1)).
            // - cv::TermCriteria: Termination criteria for the iteration.
            //   - cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER: Terminate when the desired precision is reached
            //     Or the maximum number of iterations.
            //   - 30: Maximum number of iterations.
            //   - 0.1: Desired precision.
            cv::cornerSubPix(in_image, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));

            // Associating 3D points defined above to each 2D points detected for each image
            obj_points.push_back(objp);
            images_points.push_back(corners);
            patt++;
            
        }

        // Print the first detected points of the pattern of the first image
        if (i == 0)
        {
            cv::Mat img = images[i].clone();
            // Need to convert in BGR image if we want to see better the corners
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
            cv::drawChessboardCorners(img, patternsize, corners, patternfound);

            cv::imshow("Chessboard Corners (first image of the dataset)" , img);
            cv::waitKey(0);
        }
    }
    std::cout << "\n Total images on the dataset: " << images.size() << "\n";
    std::cout << " Pattern founded : " << patt << "\n\n";

    // ---------------- CALIBRATES THE CAMERA USING THE POINTS OF THE PATTERN ----------------

    // Initialize some params needed in which the calibration function will be output something
    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    double reprojectionError;

    // image size
    cv::Size imageSize = images[0].size();

    // Calibration function, finds the camera intrinsic and extrinsic parameters from several views of a calibration pattern.
    reprojectionError = cv::calibrateCamera(
        obj_points,    // Input: 3D object points for each image
        images_points, // Input: 2D image points for each image
        imageSize,     // Input: Size of the image
        cameraMatrix,  // Output: Intrinsic camera matrix
        distCoeffs,    // Output: Distortion coefficients
        rvecs,         // Output: Rotation vectors for each image
        tvecs          // Output: Translation vectors for each image
                       // ,cv::noArray(), cv::noArray(), cv::noArray(), 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, DBL_EPSILON)  //optional
    );

    // ---------------- COMPUTES THE MEAN REPROJECTION ERROR ----------------

    // Output the results.
    std::cout << "Camera Matrix:\n"
              << cameraMatrix << "\n\n";
    std::cout << "Distortion Coefficients:\n"
              << distCoeffs << "\n\n";

    // High value of this parameters means that the distance between the 3d projected point in the plane
    // expoliting the new camera matrix params, and compare it with the original 2D points extracted
    // from the image, if the RMS re-projection error is low, we have done a good calibration
    std::cout << "(RMS) Reprojection Error: " << reprojectionError << "\n\n"; 

    // ---------------- UNDISTORTS AND RECTIFIES A NEW IMAGE ACQUIRED WITH THE SAME CAMERA ----------------

    cv::Rect roi;

    // Calculates a new camera matrix and ROI for undistortion,
    // preserving all original image content (alpha=1)
    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 0, imageSize, &roi);

    //  *IMPORTANT*:  Resize 'out_image' AFTER 'getOptimalNewCameraMatrix'
    cv::Size newImageSize(imageSize.width, imageSize.height);
    if (roi.width > 0 && roi.height > 0)
    {
        newImageSize.width = roi.width;
        newImageSize.height = roi.height;
    }

    // Use the size from newCameraMatrix and roi.
    cv::Mat out_image = cv::Mat(newImageSize, images[0].type());

    // ----------------  SHOWING THE DISTORTED AND UNDISTORTED IMAGES ----------------
    for (int i = i; i < images.size(); i++)
    {
        // The function transforms an image to compensate radial and tangential lens distortion.
        cv::undistort(images[i], out_image, cameraMatrix, distCoeffs, newCameraMatrix);

        cv::imshow("Undistorted images after camera calibration", out_image);
        cv::waitKey(0);
    }

    return 0;
}

// FUNCTIONS
void get_all_filenames(const std::string &dir_path, std::vector<std::string> &filenames)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dir_path.c_str())) != NULL)
    {
        // process all the files insider the directory
        while ((ent = readdir(dir)) != NULL)
        {
            std::string file_name = ent->d_name;
            // Don't consider the current directory '.' and the parent ".."
            if (file_name == "." || file_name == "..")
            {
                continue;
            }
            if (*(dir_path.end() - 1) == '/')
            {
                filenames.push_back(dir_path + file_name);
            }
            else
            {
                filenames.push_back(dir_path + "/" + file_name);
            }
        }
        closedir(dir); // Close the directory.
    }
}
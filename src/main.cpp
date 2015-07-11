#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include "RobustFeatureMatching.hpp"

#include <iostream>

cv::Mat stichImages (cv::Mat imgInput1, std::vector<cv::Point2f> &inlierPoints1, cv::Mat imgInput2, std::vector<cv::Point2f> &inlierPoints2)
{
    cv::Mat H = cv::findHomography(cv::Mat(inlierPoints2),
                                   cv::Mat(inlierPoints1),
                                   cv::FM_RANSAC,
                                   1.0);
    // Use the Homography Matrix to warp the images
    // std::cout << "Homo: " << H << std::endl;
    cv::Mat warpImage;
    cv::warpPerspective(imgInput2, warpImage, H, cv::Size(imgInput2.cols, imgInput2.rows), cv::INTER_CUBIC);

    // cv::imwrite("warp.jpg", warpImage2);
    // cv::Mat finalHomo(cv::Size(imgInput2.cols * 2 + imgInput1.cols, imgInput2.rows * 2), imgInput2.type());
    // cv::imwrite("finalHomo.jpg", finalHomo);
    // cv::Mat roi1(finalHomo, cv::Rect(0, 0,  imgInput1.cols, imgInput1.rows));
    // cv::Mat roi2(finalHomo, cv::Rect(0, 0, warpImage2.cols, warpImage2.rows));
    // cv::imwrite("roi1.jpg", roi1);
    // cv::imwrite("roi2.jpg", roi2);
    // warpImage2.copyTo(roi2);
    // cv::imwrite("roi2.jpg", roi2);
    // imgInput1.copyTo(roi1);
    // cv::imwrite("roi1.jpg", roi1);
    // cv::imwrite("finalHomo.jpg", finalHomo);

    cv::Mat cropImage(warpImage, cv::Rect(0 , 0, imgInput1.cols, imgInput1.rows));
    return cropImage;

    /// crop over the finalHomo
    // int colInx = 0;
    // for (size_t i(0); i < finalHomo.cols; i++)
    // {

    //     cv::Scalar res = cv::sum(finalHomo.col(i));
    //     if (res[0] == 0)
    //     {
    //         colInx = i;
    //         break;
    //     }
    // }
    // std::cout << "Col Inx: " << colInx << std::endl;

    // int rowInx = 0;
    // for (size_t i(0); i < finalHomo.rows; i++)
    // {

    //     cv::Scalar res = cv::sum(finalHomo.row(i));
    //     if (res[0] == 0)
    //     {
    //         rowInx = i;
    //         break;
    //     }
    // }
    // std::cout << "Row Inx: " << rowInx << std::endl;

    // cv::Mat cropImage(finalHomo, cv::Rect(0 , 0, colInx, rowInx));
    // return cropImage;
}

int main(int argc, char *argv[])
{

    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " image_1 " << "image_2 " 
        << "output_matched_image " << "output_croped_image " << "[output_mask_image]"<< std::endl;
        return 1;
    }

	cv::Mat imgInput1 = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    cv::Mat imgInput2 = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
    std::vector <cv::Point2f> inlierPoints1;
    std::vector <cv::Point2f> inlierPoints2;
    cv::Mat fundamental;
    RobustFeatureMatching matcher(0.8, 1.0, 0.99, 6);
    std::vector <cv::DMatch> finalMatches = matcher.run(imgInput1, imgInput2);
    std::cout << "Size Matched Points: " << finalMatches.size() << std::endl;
    std::pair < std::vector <cv::KeyPoint>, std::vector <cv::KeyPoint> > keyPoints = matcher.getKeyPoints();
    cv::Mat matchImage;
    cv::drawMatches(imgInput1, keyPoints.first, imgInput2, keyPoints.second, finalMatches, matchImage);
    cv::imwrite(argv[3], matchImage);
    std::pair < std::vector <cv::Point2f>, std::vector <cv::Point2f> > inlierPoints = matcher.getInlierPoints();
    cv::Mat cropImage = stichImages (imgInput1, inlierPoints.first, imgInput2, inlierPoints.second);
    cv::imwrite(argv[4], cropImage);

    // RobustFeatureMatching matcher2(0.8, 1.0, 0.99, 6);
    // finalMatches = matcher2.run(imgInput1, cropImage);
    // std::cout << "Size Matched Points: " << finalMatches.size() << std::endl;
    // keyPoints = matcher2.getKeyPoints();
    // cv::drawMatches(imgInput1, keyPoints.first, cropImage, keyPoints.second, finalMatches, matchImage);
    // cv::imwrite(argv[5], matchImage);

    // cv::imshow("Image Matches", matchImage);
   	// cv::waitKey(0);
    // cv::Mat result(cropImage, cv::Rect(0 , 0, imgInput1.cols, imgInput1.rows));
    // cv::imwrite("result.jpg", result);

    if (argc == 6)
    {
        cv::Mat diffImage;
        cv::absdiff(imgInput1, cropImage, diffImage);

        // // cv::threshold(diffImage, diffImage, 80, 255, cv::THRESH_BINARY);
        // // cv::erode(diffImage, diffImage, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));
        // cv::imwrite("diff.jpg", diffImage);

        cv::Mat foregroundMask = cv::Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);

        float threshold = 30.0f;

        for(int j=0; j<diffImage.rows; ++j)
            for(int i=0; i<diffImage.cols; ++i)
            {
                if(diffImage.at<uchar>(j,i) > threshold)
                {
                    foregroundMask.at<unsigned char>(j,i) = 255;
                }
            }

        cv::imwrite(argv[5], foregroundMask); 
    }  
	return 0;
}
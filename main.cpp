#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    // ----------------------读取图片---------------------------
    cv::Mat src = cv::imread("../img.png");
    if (src.empty())
    {
        cout << "error:unable to load image" << endl;
        return -1;
    }
//    cv::namedWindow("input_image", cv::WINDOW_AUTOSIZE);
//    imshow("input image", src);


    // ---------------------转灰度图----------------------------
    cv::Mat grayImg;
    cv::cvtColor(src, grayImg, cv::COLOR_BGR2GRAY);
    imshow("input",grayImg);

    //---------------------滤波去噪点----------------------------
//    cv::Mat blurImage;
////    cv::GaussianBlur(grayImg, blurImage, size(9,9), 2,2);     //高斯滤波
//      medianBlur(grayImg, blurImage, 7);              // 中值滤波
////     cv::blur(grayImg, blurImage, size(10,10));       // 均值滤波
//    imshow("模糊滤波", blurImage);


    vector<cv::Vec3f>circles;
    int hough_value = 23;        // Hough变换的累加器阈值，用于检测圆的灵敏度

    // --------------------霍夫圆检测---------------------------
    cv::HoughCircles(grayImg, circles, cv::HOUGH_GRADIENT, 1, 25, 400, hough_value, 30, 42);
    cv::Mat houghcircle = src.clone();

    for (int i = 0; i < circles.size(); ++i)
    {
        //绘制圆圈
        circle(houghcircle, cv::Point(circles[i][0], circles[i][1]), circles[i][2], cv::Scalar(0, 0, 255), 2);
        //绘制圆心
        circle(houghcircle,cv::Point(circles[i][0], circles[i][1]),
               3,
               cv::Scalar(0, 255, 0),
               -1,
               cv::LINE_AA);
        //显示圆心坐标
        std::string coord_text = "(" + std::to_string(int (circles[i][0])) + "," + std::to_string(int (circles[i][1])) + ")";
        cv::putText(houghcircle, coord_text,cv::Point(circles[i][0]+10,circles[i][1]-10),
                    cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,255,255),1);
    }

    std::vector<cv::Point> centers;
    for (int i = 0; i < circles.size(); ++i) {
        centers.emplace_back(cvRound(circles[i][0]), cvRound(circles[i][1]));
    }

    for (size_t i = 0; i < centers.size(); ++i) {
        for (size_t j = i+1; j < centers.size(); ++j) {
            // 计算欧氏距离
            double dx = centers[j].x - centers[i].x;
            double dy = centers[j].y - centers[i].y;
            double distance = std::sqrt(dx*dx + dy*dy);

            cv::line(houghcircle, centers[i], centers[j], cv::Scalar(255,0,0), 2);

            cv::Point midPt((centers[i].x + centers[j].x)/2, (centers[i].y + centers[j].y)/2);
            std::string distText = cv::format("%.2f px", distance);
            cv::putText(houghcircle, distText, midPt + cv::Point(0,-10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 2);
        }
    }

    imwrite("houghcircle.jpg", houghcircle);
    imshow("houghcircle", houghcircle);
    cv::waitKey(0);
    return 0;
}


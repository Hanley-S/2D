#include "CircleDetector.h"
#include <iostream>

CircleDetector::CircleDetector() : filterType(0) {}

void CircleDetector::setImage(const cv::Mat& image) {
    src = image.clone();
    processedImage = cv::Mat();
    circles.clear();
    centers.clear();
    resultImage = cv::Mat();
}

void CircleDetector::setFilterType(int filterType) {
    this->filterType = filterType;
}

void CircleDetector::preprocessImage() {
    if (src.empty()) return;

    // 转换为灰度图像
    cv::cvtColor(src, processedImage, cv::COLOR_BGR2GRAY);

    // 滤波去噪
    switch (filterType) {
        case 1:  // 高斯滤波
            cv::GaussianBlur(processedImage, processedImage, cv::Size(9, 9), 2, 2);
            break;
        case 2:  // 中值滤波
            cv::medianBlur(processedImage, processedImage, 7);
            break;
        case 3:  // 均值滤波
            cv::blur(processedImage, processedImage, cv::Size(10, 10));
            break;
        default:  // 无滤波
            break;
    }
}

bool CircleDetector::detectCircles() {
    if (src.empty()) return false;

    // 图像预处理
    preprocessImage();

    // 霍夫圆检测
    cv::HoughCircles(processedImage, circles, cv::HOUGH_GRADIENT, 1, 25, 400, 23, 30, 42);

    if (circles.empty()) return false;

    // 提取圆心坐标
    for (const auto& circle : circles) {
        centers.emplace_back(cv::Point(cvRound(circle[0]), cvRound(circle[1])));
    }
    for (const auto& center : centers) {
        std::cout << "Center: (" << center.x << ", " << center.y << ")" << std::endl;
    }

    return true;
}

std::vector<cv::Vec3f> CircleDetector::getCircles() {
    return circles;
}

cv::Mat CircleDetector::drawCircles() {
    if (src.empty() || circles.empty()) return cv::Mat();

    resultImage = src.clone();

    // 绘制圆和圆心
    for (size_t i = 0; i < circles.size(); ++i) {
        // 绘制圆圈
        cv::circle(resultImage, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), circles[i][2], cv::Scalar(0, 0, 255), 2);
        // 绘制圆心
        cv::circle(resultImage, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), 3, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);
        // 显示圆心坐标
        std::string coord_text = "(" + std::to_string(int(cvRound(circles[i][0]))) + "," + std::to_string(int(cvRound(circles[i][1]))) + ")";
        cv::putText(resultImage, coord_text, cv::Point(cvRound(circles[i][0]) + 10, cvRound(circles[i][1]) - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    for (size_t i = 0; i < centers.size(); ++i) {
        for (size_t j = i + 1; j < centers.size(); ++j) {
            // 计算欧氏距离
            double dx = centers[j].x - centers[i].x;
            double dy = centers[j].y - centers[i].y;
            double distance = std::sqrt(dx * dx + dy * dy);

            // 绘制连接线
            cv::line(resultImage, centers[i], centers[j], cv::Scalar(255, 0, 0), 2);

            // 显示距离
            cv::Point midPt((centers[i].x + centers[j].x) / 2, (centers[i].y + centers[j].y) / 2);
            std::string distText = cv::format("%.2f px", distance);
            cv::putText(resultImage, distText, midPt + cv::Point(0, -10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
        }
    }
    return resultImage;
}

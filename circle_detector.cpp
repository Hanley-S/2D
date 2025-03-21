#include "circle_detector.h"
#include <iostream>

CircleDetector::CircleDetector() : filter_type_(0) {}

void CircleDetector::set_image(const cv::Mat& image) {
    src_ = image.clone();
    processed_image_ = cv::Mat();
    circles_.clear();
    centers_.clear();
    result_image_ = cv::Mat();
}

void CircleDetector::set_filter_type(int filter_type) {
    filter_type_ = filter_type;
}

void CircleDetector::preprocess_image() {
    if (src_.empty()) return;

    // 转换为灰度图像
    cv::cvtColor(src_, processed_image_, cv::COLOR_BGR2GRAY);

    // 滤波去噪
    switch (filter_type_) {
        case 1:  // 高斯滤波
            cv::GaussianBlur(processed_image_, processed_image_, cv::Size(9, 9), 2, 2);
            break;
        case 2:  // 中值滤波
            cv::medianBlur(processed_image_, processed_image_, 7);
            break;
        case 3:  // 均值滤波
            cv::blur(processed_image_, processed_image_, cv::Size(10, 10));
            break;
        default:  // 无滤波
            break;
    }
}

bool CircleDetector::detect_circles() {
    if (src_.empty()) return false;

    // 图像预处理
    preprocess_image();

    // 霍夫圆检测
    cv::HoughCircles(processed_image_, circles_, cv::HOUGH_GRADIENT, 1, 25, 400, 23, 30, 42);

    if (circles_.empty()) return false;

    // 提取圆心坐标
    for (const auto& circle : circles_) {
        centers_.emplace_back(cv::Point(cvRound(circle[0]), cvRound(circle[1])));
    }
    for (const auto& center : centers_) {
        std::cout << "Center: (" << center.x << ", " << center.y << ")" << std::endl;
    }

    return true;
}

std::vector<cv::Vec3f> CircleDetector::get_circles() const {
    return circles_;
}

cv::Mat CircleDetector::draw_circles() {
    if (src_.empty() || circles_.empty()) return cv::Mat();

    result_image_ = src_.clone();

    // 绘制圆和圆心
    for (size_t i = 0; i < circles_.size(); ++i) {
        // 绘制圆圈
        cv::circle(result_image_, cv::Point(cvRound(circles_[i][0]), cvRound(circles_[i][1])),
                   circles_[i][2], cv::Scalar(0, 0, 255), 2);
        // 绘制圆心
        cv::circle(result_image_, cv::Point(cvRound(circles_[i][0]), cvRound(circles_[i][1])),
                   3, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);
        // 显示圆心坐标
        std::string coord_text = "(" + std::to_string(int(cvRound(circles_[i][0]))) + "," +
                                 std::to_string(int(cvRound(circles_[i][1]))) + ")";
        cv::putText(result_image_, coord_text,
                    cv::Point(cvRound(circles_[i][0]) + 10, cvRound(circles_[i][1]) - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    // 绘制连接线并显示距离
    for (size_t i = 0; i < centers_.size(); ++i) {
        for (size_t j = i + 1; j < centers_.size(); ++j) {
            // 计算欧氏距离
            double dx = centers_[j].x - centers_[i].x;
            double dy = centers_[j].y - centers_[i].y;
            double distance = std::sqrt(dx * dx + dy * dy);

            // 绘制连接线
            cv::line(result_image_, centers_[i], centers_[j], cv::Scalar(255, 0, 0), 2);

            // 显示距离
            cv::Point mid_pt((centers_[i].x + centers_[j].x) / 2, (centers_[i].y + centers_[j].y) / 2);
            std::string dist_text = cv::format("%.2f px", distance);
            cv::putText(result_image_, dist_text, mid_pt + cv::Point(0, -10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
        }
    }
    return result_image_;
}
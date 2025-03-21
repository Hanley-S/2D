#include <cmath>
#include "circle_text.h"

CircleDetector::CircleDetector(const DetectionParams& params)
        : detect_params_(params) {}

void CircleDetector::set_visualization_params(const VisualizationParams& params) {
    visual_params_ = params;
}

bool CircleDetector::detect(const cv::Mat& input_image) {
    // 图像预处理
    preprocess_image(input_image);

    // 执行霍夫圆检测
    cv::HoughCircles(processed_image_, circles_, cv::HOUGH_GRADIENT,
                     detect_params_.dp, detect_params_.min_dist,
                     detect_params_.param1, detect_params_.param2,
                     detect_params_.min_radius, detect_params_.max_radius);

    if (circles_.empty()) return false;

    // 计算圆心间距
    calculate_distances();
    return true;
}

void CircleDetector::preprocess_image(const cv::Mat& input) {
    // 尺寸调整
    cv::resize(input, resized_image_, cv::Size(input.cols / 5, input.rows / 5));

    // 灰度转换
    cv::cvtColor(resized_image_, processed_image_, cv::COLOR_BGR2GRAY);

    // 噪声去除
    switch (detect_params_.blur_type) {
        case 1:
            cv::medianBlur(processed_image_, processed_image_, detect_params_.blur_size);
            break;
        case 2:
            cv::GaussianBlur(processed_image_, processed_image_,
                             cv::Size(detect_params_.blur_size, detect_params_.blur_size), 2);
            break;
        case 3:
            cv::blur(processed_image_, processed_image_,
                     cv::Size(detect_params_.blur_size, detect_params_.blur_size));
            break;
    }
}

void CircleDetector::calculate_distances() {
    connections_.clear();
    distances_.clear();

    std::vector<cv::Point> centers;
    for (const auto& circle : circles_) {
        centers.emplace_back(cvRound(circle[0]), cvRound(circle[1]));
    }

    for (size_t i = 0; i < centers.size(); ++i) {
        for (size_t j = i + 1; j < centers.size(); ++j) {
            const auto dx = centers[j].x - centers[i].x;
            const auto dy = centers[j].y - centers[i].y;
            const double dist = std::sqrt(dx * dx + dy * dy);

            connections_.emplace_back(centers[i], centers[j]);
            distances_.push_back(dist);
        }
    }
}

void CircleDetector::visualize_results(cv::Mat& output_image) const {
    // 使用尺寸调整后的彩色图像作为背景
    output_image = resized_image_.clone();

    // 绘制检测结果
    for (const auto& circle : circles_) {
        const cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        const int radius = cvRound(circle[2]);

        // 绘制圆形
        cv::circle(output_image, center, radius,
                   visual_params_.circle_color, 2);

        // 绘制圆心
        cv::circle(output_image, center, 3,
                   visual_params_.center_color, -1, cv::LINE_AA);

        // 显示坐标
        if (visual_params_.show_coordinates) {
            const std::string coord_text = "(" + std::to_string(center.x) + "," +
                                           std::to_string(center.y) + ")";
            cv::putText(output_image, coord_text, center + cv::Point(10, -10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }
    }

    // 绘制连接线
    if (visual_params_.draw_connections) {
        for (size_t i = 0; i < connections_.size(); ++i) {
            const auto& [pt1, pt2] = connections_[i];

            // 绘制连线
            cv::line(output_image, pt1, pt2,
                     visual_params_.line_color, 2);

            // 显示距离
            const cv::Point mid_pt = (pt1 + pt2) / 2;
            const std::string dist_text = cv::format("%.2f px", distances_[i]);
            cv::putText(output_image, dist_text, mid_pt + cv::Point(0, -10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
        }
    }
}

const std::vector<cv::Vec3f>& CircleDetector::get_detected_circles() const {
    return circles_;
}
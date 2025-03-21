#ifndef IMAGE_PROCESSING_CIRCLEDETECTOR_H_
#define IMAGE_PROCESSING_CIRCLEDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>

// 圆检测器类，用于检测图像中的圆并绘制结果
class CircleDetector {
public:
    // 构造函数
    CircleDetector();

    // 设置待处理的图像
    void set_image(const cv::Mat& image);

    // 设置滤波类型
    // 0: 无滤波, 1: 高斯滤波, 2: 中值滤波, 3: 均值滤波
    void set_filter_type(int filter_type);

    // 检测图像中的圆
    bool detect_circles();

    // 获取检测到的圆
    std::vector<cv::Vec3f> get_circles() const;

    // 绘制检测到的圆并返回结果图像
    cv::Mat draw_circles();

private:
    // 图像预处理，包括灰度转换和滤波
    void preprocess_image();

    cv::Mat src_;                // 原始图像
    cv::Mat processed_image_;    // 预处理后的图像
    std::vector<cv::Vec3f> circles_;  // 检测到的圆
    std::vector<cv::Point> centers_;  // 圆心坐标
    cv::Mat result_image_;       // 绘制结果图像
    int filter_type_;            // 滤波类型
};

#endif  // IMAGE_PROCESSING_CIRCLEDETECTOR_H_
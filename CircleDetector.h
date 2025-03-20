#ifndef CIRCLEDETECTOR_H
#define CIRCLEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>

class CircleDetector {
public:
    // 构造函数
    CircleDetector();

    // 设置图像
    void setImage(const cv::Mat& image);

    // 设置滤波类型
    void setFilterType(int filterType);

    // 检测圆
    bool detectCircles();

    // 获取检测到的圆
    std::vector<cv::Vec3f> getCircles();

    // 绘制检测到的圆
    cv::Mat drawCircles();

private:
    // 图像预处理
    void preprocessImage();

    cv::Mat src;                // 原始图像
    cv::Mat processedImage;     // 预处理后的图像
    std::vector<cv::Vec3f> circles;  // 检测到的圆
    std::vector<cv::Point> centers;  // 圆心坐标
    cv::Mat resultImage;        // 绘制结果图像
    int filterType;             // 滤波类型
};

#endif // CIRCLEDETECTOR_H
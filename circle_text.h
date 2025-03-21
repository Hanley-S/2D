#ifndef IMAGE_PROCESSING_CIRCLE_DETECTOR_H_
#define IMAGE_PROCESSING_CIRCLE_DETECTOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

// 圆检测器类，用于检测图像中的圆并绘制结果
class CircleDetector {
public:
    // 检测参数结构体
    struct DetectionParams {
        int blur_type = 1;               // 1: 中值滤波, 2: 高斯滤波, 3: 均值滤波
        int blur_size = 3;               // 滤波核尺寸
        double dp = 2.0;                 // 累加器分辨率
        double min_dist = 70.0;          // 圆心最小间距
        double param1 = 150.0;           // Canny边缘检测阈值
        double param2 = 40.0;            // 累加器阈值
        int min_radius = 15;             // 最小圆半径
        int max_radius = 18;             // 最大圆半径
    };

    // 可视化参数结构体
    struct VisualizationParams {
        bool show_coordinates = true;    // 是否显示圆心坐标
        bool draw_connections = true;    // 是否绘制连接线
        cv::Scalar circle_color = {0, 0, 255};  // 圆颜色
        cv::Scalar center_color = {0, 255, 0};  // 圆心颜色
        cv::Scalar line_color = {255, 0, 0};    // 连接线颜色
    };

    // 构造函数
    explicit CircleDetector(const DetectionParams& params = DetectionParams());

    // 设置可视化参数
    void set_visualization_params(const VisualizationParams& params);

    // 检测图像中的圆
    bool detect(const cv::Mat& input_image);

    // 可视化检测结果
    void visualize_results(cv::Mat& output_image) const;

    // 获取检测到的圆
    const std::vector<cv::Vec3f>& get_detected_circles() const;

private:
    // 图像预处理
    void preprocess_image(const cv::Mat& input);

    // 计算圆心间距
    void calculate_distances();

    DetectionParams detect_params_;  // 检测参数
    VisualizationParams visual_params_;  // 可视化参数

    cv::Mat resized_image_;  // 尺寸调整后的彩色图像
    cv::Mat processed_image_;  // 预处理后的灰度图像
    std::vector<cv::Vec3f> circles_;  // 检测到的圆
    std::vector<std::pair<cv::Point, cv::Point>> connections_;  // 圆心连接线
    std::vector<double> distances_;  // 圆心间距
};

#endif  // IMAGE_PROCESSING_CIRCLE_DETECTOR_H_
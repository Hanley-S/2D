#include <iostream>
#include <opencv2/opencv.hpp>
#include "circle_text.h"

int main() {
    // 初始化检测器
    CircleDetector::DetectionParams params;
    params.blur_type = 1;  // 使用中值滤波

    CircleDetector detector(params);

    // 设置可视化参数
    CircleDetector::VisualizationParams visual_params;
    visual_params.show_coordinates = true;
    detector.set_visualization_params(visual_params);

    // 批量处理图像
    const std::string folder_path = "E:/MVS_data/";
    int index = 1;

    while (true) {
        // 生成文件名
        const std::string image_path = folder_path + "img_" +
                                       std::to_string(index++) + ".png";

        // 读取图像
        cv::Mat image = cv::imread(image_path);
        if (image.empty()) {
            std::cout << "Processed " << (index - 2) << " images." << std::endl;
            break;
        }

        // 执行检测
        if (detector.detect(image)) {
            // 可视化结果
            cv::Mat result;
            detector.visualize_results(result);

            // 保存并显示
            const std::string result_path = "result_" + std::to_string(index - 1) + ".jpg";
            cv::imwrite(result_path, result);
            cv::imshow("Detection Result", result);
            cv::waitKey(0);
        } else {
            std::cout << "No circles detected in: " << image_path << std::endl;
        }
    }

    return 0;
}
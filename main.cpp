#include <iostream>

#include "circle_detector.h"


int main(int argc, char** argv) {
    // 读取图像
    cv::Mat src = cv::imread("D:/2d code/circle/img.png");
    if (src.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // 创建圆检测器
    CircleDetector detector;
    detector.set_image(src);

    // 设置滤波类型（0: 无滤波, 1: 高斯滤波, 2: 中值滤波, 3: 均值滤波）
    detector.set_filter_type(0);

    // 检测圆
    if (detector.detect_circles()) {
        // 绘制检测到的圆
        cv::Mat result_image = detector.draw_circles();

        // 显示结果
        cv::imshow("Detected Circles", result_image);
        cv::waitKey(0);
    } else {
        std::cerr << "No circles detected!" << std::endl;
    }

    return 0;
}

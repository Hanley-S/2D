#include "CircleDetector.h"
#include <iostream>

int main(int argc, char** argv) {
    // 读取图像
    cv::Mat src = cv::imread("../img.png");
    if (src.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // 创建圆检测器
    CircleDetector detector;
    detector.setImage(src);

    // 设置滤波类型（0: 无滤波, 1: 高斯滤波, 2: 中值滤波, 3: 均值滤波）
    detector.setFilterType(0);

    // 检测圆
    if (detector.detectCircles()) {
        // 绘制检测到的圆
        cv::Mat resultImage = detector.drawCircles();

        // 显示结果
        cv::imshow("Detected Circles", resultImage);
        cv::waitKey(0);
    } else {
        std::cerr << "No circles detected!" << std::endl;
    }

    return 0;
}
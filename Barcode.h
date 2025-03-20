#ifndef BARCODE_H
#define BARCODE_H

#include <opencv2/opencv.hpp>
#include <vector>

class BarcodeDetector {
public:
    // 构造函数
    BarcodeDetector(const std::string& imagePath);

    // 检测条形码
    void detectBarcode();

private:
    // 图像预处理
    cv::Mat preprocessImage();

    // 梯度增强与二值化
    cv::Mat enhanceAndBinarize(const cv::Mat& processed);

    // 形态学操作
    cv::Mat morphologicalOperations(const cv::Mat& binary);

    // 检测条形码区域
    cv::RotatedRect detectBarcodeRegion(const cv::Mat& morph);

    // 在原图上绘制条形码框
    void drawBarcodeBox(cv::Mat& displayImg, const cv::RotatedRect& barcodeRect);

    // 截取条形码区域
    cv::Mat extractBarcodeRegion(const cv::RotatedRect& barcodeRect);

    // 成员变量
    cv::Mat src;          // 原始图像
    cv::Mat resizedSrc;   // 调整大小后的图像
};

#endif // BARCODE_H
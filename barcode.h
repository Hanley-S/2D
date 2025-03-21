#ifndef IMAGE_PROCESSING_BARCODE_DETECTOR_H_
#define IMAGE_PROCESSING_BARCODE_DETECTOR_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// 条形码检测器类，用于检测图像中的条形码并提取条形码区域
class BarcodeDetector {
public:
    // 构造函数，接受图像路径作为参数
    explicit BarcodeDetector(const std::string& image_path);

    // 检测条形码并显示结果
    void DetectBarcode();

private:
    // 图像预处理，包括灰度转换和高斯滤波
    cv::Mat PreprocessImage();

    // 梯度增强与二值化，用于突出条形码区域
    cv::Mat EnhanceAndBinarize(const cv::Mat& processed);

    // 形态学操作，用于填充条形码间隙和去除噪声
    cv::Mat MorphologicalOperations(const cv::Mat& binary);

    // 检测条形码区域，返回旋转矩形
    cv::RotatedRect DetectBarcodeRegion(const cv::Mat& morph);

    // 在原图上绘制条形码框
    void DrawBarcodeBox(cv::Mat& display_img, const cv::RotatedRect& barcode_rect);

    // 截取条形码区域，返回条形码图像
    cv::Mat ExtractBarcodeRegion(const cv::RotatedRect& barcode_rect);

    cv::Mat src_;          // 原始图像
    cv::Mat resized_src_;  // 调整大小后的图像
};

#endif  // IMAGE_PROCESSING_BARCODE_DETECTOR_H_
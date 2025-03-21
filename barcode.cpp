#include "barcode.h"
#include <iostream>

BarcodeDetector::BarcodeDetector(const std::string& image_path) {
    src_ = cv::imread(image_path);
    if (src_.empty()) {
        throw std::runtime_error("Error: Unable to load image!");
    }
    cv::resize(src_, resized_src_, cv::Size(600, 400));  // 调整图像大小
}

cv::Mat BarcodeDetector::PreprocessImage() {
    // 转化为灰度图
    cv::Mat gray;
    cv::cvtColor(resized_src_, gray, cv::COLOR_BGR2GRAY);

    // 高斯平滑滤波
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);

    return blurred;
}

cv::Mat BarcodeDetector::EnhanceAndBinarize(const cv::Mat& processed) {
    // 使用Sobel算子求水平和垂直方向梯度差
    cv::Mat grad_x, grad_y, gradient;
    cv::Sobel(processed, grad_x, CV_16S, 1, 0, 3, 1, 0, 4);  // 水平梯度
    cv::Sobel(processed, grad_y, CV_16S, 0, 1, 3, 1, 0, 4);  // 垂直梯度
    cv::subtract(grad_x, grad_y, gradient);                  // 梯度差
    cv::convertScaleAbs(gradient, gradient);                 // 转换为8位图像

    // 均值滤波，消除高频噪声
    cv::Mat mean_filtered;
    cv::blur(gradient, mean_filtered, cv::Size(3, 3));

    // 二值化
    cv::Mat binary;
    cv::threshold(mean_filtered, binary, 90, 255, cv::THRESH_BINARY);

    return binary;
}

cv::Mat BarcodeDetector::MorphologicalOperations(const cv::Mat& binary) {
    // 闭运算，填充条形码间隙
    cv::Mat closed;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::morphologyEx(binary, closed, cv::MORPH_CLOSE, kernel);

    // 腐蚀，去除孤立的点
    cv::Mat eroded;
    cv::morphologyEx(closed, eroded, cv::MORPH_ERODE, kernel);

    // 膨胀，填充条形码间空隙
    cv::Mat dilated;
    cv::morphologyEx(eroded, dilated, cv::MORPH_DILATE, kernel);
    cv::morphologyEx(eroded, dilated, cv::MORPH_DILATE, kernel);

    return dilated;
}

cv::RotatedRect BarcodeDetector::DetectBarcodeRegion(const cv::Mat& morph) {
    // 找到最大条形码区域
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        throw std::runtime_error("No contours found!");
    }

    // 获取最大轮廓的旋转矩形
    cv::RotatedRect max_rect = cv::minAreaRect(contours[0]);
    for (size_t i = 1; i < contours.size(); i++) {
        cv::RotatedRect tmp = cv::minAreaRect(contours[i]);
        if (tmp.size.area() > max_rect.size.area()) {
            max_rect = tmp;
        }
    }

    return max_rect;
}

void BarcodeDetector::DrawBarcodeBox(cv::Mat& display_img, const cv::RotatedRect& barcode_rect) {
    cv::Point2f vertices[4];
    barcode_rect.points(vertices);
    for (int i = 0; i < 4; i++) {
        cv::line(display_img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }
}

cv::Mat BarcodeDetector::ExtractBarcodeRegion(const cv::RotatedRect& barcode_rect) {
    // 获取旋转矩形的角度和尺寸
    float angle = barcode_rect.angle;
    cv::Size2f rect_size = barcode_rect.size;

    // 确保长边为宽度，并校正角度范围
    if (rect_size.height > rect_size.width) {
        std::swap(rect_size.width, rect_size.height);
        angle += 90.0f;
    }
    angle = fmod(angle + 180.0f, 180.0f);
    if (angle > 90.0f) {
        angle -= 180.0f;
    }

    // 截取条形码区域
    cv::Mat barcode_img;
    if (std::abs(angle) > 5.0) {
        // 计算旋转中心
        cv::Point2f center = barcode_rect.center;
        cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);

        // 执行旋转
        cv::Mat rotated_img;
        cv::warpAffine(resized_src_, rotated_img, rot_mat, resized_src_.size(), cv::INTER_CUBIC);

        // 定义旋转矩形的四个顶点
        cv::Point2f vertices[4];
        barcode_rect.points(vertices);
        std::vector<cv::Point2f> src_points = {vertices[0], vertices[1], vertices[2], vertices[3]};

        // 定义目标矩形的四个顶点
        std::vector<cv::Point2f> dst_points = {
                cv::Point2f(0, 0),
                cv::Point2f(rect_size.width, 0),
                cv::Point2f(rect_size.width, rect_size.height),
                cv::Point2f(0, rect_size.height)
        };

        // 计算透视变换矩阵
        cv::Mat perspective_mat = cv::getPerspectiveTransform(src_points, dst_points);

        // 执行透视变换
        cv::warpPerspective(resized_src_, barcode_img, perspective_mat, cv::Size(rect_size.width, rect_size.height));
    } else {
        // 直接截取原始区域
        barcode_img = resized_src_(barcode_rect.boundingRect());
    }

    return barcode_img;
}

void BarcodeDetector::DetectBarcode() {
    // 图像预处理
    cv::Mat processed = PreprocessImage();

    // 梯度增强与二值化
    cv::Mat binary = EnhanceAndBinarize(processed);

    // 形态学操作
    cv::Mat morph = MorphologicalOperations(binary);

    // 检测条形码区域
    cv::RotatedRect barcode_rect = DetectBarcodeRegion(morph);

    // 在原图上绘制条形码框
    cv::Mat display_img = resized_src_.clone();
    DrawBarcodeBox(display_img, barcode_rect);
    cv::imshow("Original Image with Barcode Box", display_img);

    // 截取条形码区域
    cv::Mat barcode_img = ExtractBarcodeRegion(barcode_rect);
    if (!barcode_img.empty()) {
        cv::namedWindow("Extracted Barcode", cv::WINDOW_NORMAL);
        cv::resizeWindow("Extracted Barcode", barcode_img.cols, barcode_img.rows);
        cv::imshow("Extracted Barcode", barcode_img);
    }

    cv::waitKey(0);
}
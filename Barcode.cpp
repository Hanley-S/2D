#include "Barcode.h"
#include <iostream>

BarcodeDetector::BarcodeDetector(const std::string& imagePath) {
    src = cv::imread(imagePath);
    if (src.empty()) {
        throw std::runtime_error("Error: Unable to load image!");
    }
    cv::resize(src, resizedSrc, cv::Size(600, 400)); // 调整图像大小
}

cv::Mat BarcodeDetector::preprocessImage() {
    // 转化为灰度图
    cv::Mat gray;
    cv::cvtColor(resizedSrc, gray, cv::COLOR_BGR2GRAY);

    // 高斯平滑滤波
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);

    return blurred;
}

cv::Mat BarcodeDetector::enhanceAndBinarize(const cv::Mat& processed) {
    // 使用Sobel算子求水平和垂直方向梯度差
    cv::Mat gradX, gradY, gradient;
    cv::Sobel(processed, gradX, CV_16S, 1, 0, 3, 1, 0, 4); // 水平梯度
    cv::Sobel(processed, gradY, CV_16S, 0, 1, 3, 1, 0, 4); // 垂直梯度
    cv::subtract(gradX, gradY, gradient);       // 梯度差
    cv::convertScaleAbs(gradient, gradient);    // 转换为8位图像

    // 均值滤波，消除高频噪声
    cv::Mat meanFiltered;
    cv::blur(gradient, meanFiltered, cv::Size(3, 3));

    // 二值化
    cv::Mat binary;
    cv::threshold(meanFiltered, binary, 90, 255, cv::THRESH_BINARY);

    return binary;
}

cv::Mat BarcodeDetector::morphologicalOperations(const cv::Mat& binary) {
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

cv::RotatedRect BarcodeDetector::detectBarcodeRegion(const cv::Mat& morph) {
    // 找到最大条形码区域
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        throw std::runtime_error("No contours found!");
    }

    // 获取最大轮廓的旋转矩形
    cv::RotatedRect maxRect = cv::minAreaRect(contours[0]);
    for (size_t i = 1; i < contours.size(); i++) {
        cv::RotatedRect tmp = cv::minAreaRect(contours[i]);
        if (tmp.size.area() > maxRect.size.area()) {
            maxRect = tmp;
        }
    }

    return maxRect;
}

void BarcodeDetector::drawBarcodeBox(cv::Mat& displayImg, const cv::RotatedRect& barcodeRect) {
    cv::Point2f vertices[4];
    barcodeRect.points(vertices);
    for (int i = 0; i < 4; i++) {
        cv::line(displayImg, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }
}

cv::Mat BarcodeDetector::extractBarcodeRegion(const cv::RotatedRect& barcodeRect) {
    // 获取旋转矩形的角度和尺寸
    float angle = barcodeRect.angle;
    cv::Size2f rectSize = barcodeRect.size;

    // 确保长边为宽度，并校正角度范围
    if (rectSize.height > rectSize.width) {
        std::swap(rectSize.width, rectSize.height);
        angle += 90.0f;
    }
    angle = fmod(angle + 180.0f, 180.0f);
    if (angle > 90.0f) {
        angle -= 180.0f;
    }

    // 截取条形码区域
    cv::Mat barcodeImg;
    if (std::abs(angle) > 5.0) {
        // 计算旋转中心
        cv::Point2f center = barcodeRect.center;
        cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, 1.0);

        // 执行旋转
        cv::Mat rotatedImg;
        cv::warpAffine(resizedSrc, rotatedImg, rotMat, resizedSrc.size(), cv::INTER_CUBIC);

        // 定义旋转矩形的四个顶点
        cv::Point2f vertices[4];
        barcodeRect.points(vertices);
        std::vector<cv::Point2f> srcPoints = {vertices[0], vertices[1], vertices[2], vertices[3]};

        // 定义目标矩形的四个顶点
        std::vector<cv::Point2f> dstPoints = {
                cv::Point2f(0, 0),
                cv::Point2f(rectSize.width, 0),
                cv::Point2f(rectSize.width, rectSize.height),
                cv::Point2f(0, rectSize.height)
        };

        // 计算透视变换矩阵
        cv::Mat perspectiveMat = cv::getPerspectiveTransform(srcPoints, dstPoints);

        // 执行透视变换
        cv::warpPerspective(resizedSrc, barcodeImg, perspectiveMat, cv::Size(rectSize.width, rectSize.height));
    } else {
        // 直接截取原始区域
        barcodeImg = resizedSrc(barcodeRect.boundingRect());
    }

    return barcodeImg;
}

void BarcodeDetector::detectBarcode() {
    // 图像预处理
    cv::Mat processed = preprocessImage();

    // 梯度增强与二值化
    cv::Mat binary = enhanceAndBinarize(processed);

    // 形态学操作
    cv::Mat morph = morphologicalOperations(binary);

    // 检测条形码区域
    cv::RotatedRect barcodeRect = detectBarcodeRegion(morph);

    // 在原图上绘制条形码框
    cv::Mat displayImg = resizedSrc.clone();
    drawBarcodeBox(displayImg, barcodeRect);
    cv::imshow("Original Image with Barcode Box", displayImg);

    // 截取条形码区域
    cv::Mat barcodeImg = extractBarcodeRegion(barcodeRect);
    if (!barcodeImg.empty()) {
        cv::namedWindow("Extracted Barcode", cv::WINDOW_NORMAL);
        cv::resizeWindow("Extracted Barcode", barcodeImg.cols, barcodeImg.rows);
        cv::imshow("Extracted Barcode", barcodeImg);
    }

    cv::waitKey(0);
}
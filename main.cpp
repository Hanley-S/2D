#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


Mat rotateImage(const Mat& src, double angle) {
    Mat dst;
    Point2f center(src.cols / 2.0, src.rows / 2.0);       //photo center
    Mat rot = getRotationMatrix2D(center, angle, 1.0);      //Get rotation matrix
    /* 计算旋转后的图像边界框
     * RotatedRect参数：中心点，原图尺寸，旋转角度
     * boundingRect2f()获取旋转后的矩形包围盒 */
    Rect2f bbox = RotatedRect(center, src.size(), angle).boundingRect2f();  // 计算旋转后的边界框
    rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;  // 调整旋转矩阵
    rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;  // 调整旋转矩阵

    warpAffine(src, dst, rot, bbox.size());  // 执行旋转
    return dst;
}

// 函数：校正条形码方向
Mat correctBarcodeOrientation(const Mat& barcode) {
    Mat gray;
    cvtColor(barcode, gray, COLOR_BGR2GRAY);  // 转换为灰度图像

    // 二值化
    Mat thresh;
    threshold(gray, thresh, 128, 255, THRESH_BINARY);

    // 查找轮廓
    vector<vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //
    vector<Point> maxContour;
    double maxArea = 0;
    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area > maxArea) {
            maxArea = area;
            maxContour = contour;
        }
    }

    //
    RotatedRect minRect = minAreaRect(maxContour);

    //
    double angle = minRect.angle;
    if (angle < -45) angle += 90;  // 校正角度

    //
    Mat corrected;
    Point2f center(barcode.cols / 2.0, barcode.rows / 2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    warpAffine(barcode, corrected, rot, barcode.size());

    return corrected;
}

void detectAndCropBarcode(const Mat& src) {
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);  // 转换为灰度图像

    // 使用 Sobel 算子计算梯度
    Mat gradX, gradY;
    Sobel(gray, gradX, CV_32F, 1, 0, -1);
    Sobel(gray, gradY, CV_32F, 0, 1, -1);

    // 计算梯度幅值和方向
    Mat gradient;
    subtract(gradX, gradY, gradient);
    convertScaleAbs(gradient, gradient);

    // 高斯模糊
    GaussianBlur(gradient, gradient, Size(9, 9), 2, 2);
//    imshow("gradient", gradient);

    //binaryzation
    Mat thresh;
    threshold(gradient, thresh, 225, 255, THRESH_BINARY);

    //Morphological operation (closed operation)
    Mat kernel = getStructuringElement(MORPH_RECT, Size(21, 7));
    morphologyEx(thresh, thresh, MORPH_CLOSE, kernel);

    // 查找轮廓
    vector<vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 调试输出
    cout << "=== Detection Results ===" << endl;
    cout << "Total contours: " << contours.size() << endl;
    int validCount = 0;

    // 遍历轮廓
    for (size_t i = 0; i < contours.size(); i++) {
        // 计算轮廓的边界框
        Rect rect = boundingRect(contours[i]);

        // 过滤掉太小的区域
        if (rect.width < 100 || rect.height < 50) continue;

        // 绘制边界框
        rectangle(src, rect, Scalar(0, 255, 0), 2);

        Mat barcode = src(rect);

        imshow("Barcode", barcode);
        imwrite("barcode_cropped.jpg", barcode);
    }
}

int main() {
    // 读取图像
    Mat src = imread("../img.png");
    if (src.empty()) {
        cout << "Error: Unable to load image!" << endl;
        return -1;
    }

    // 旋转图像并检测条形码
    for (double angle = -30; angle <= 30; angle += 10) {
        Mat rotated = rotateImage(src, angle);  // 旋转图像
        detectAndCropBarcode(rotated);  // 检测条形码
//        imshow("Rotation Angle:" + to_string(angle),rotated);
//        waitKey(500);
    }

    // 显示原始图像
    imshow("Original Image", src);
    waitKey(0);

    return 0;
}
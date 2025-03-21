#include "barcode.h"
#include <iostream>

int main() {
    try {
        // 创建 BarcodeDetector 对象
        BarcodeDetector detector("D:/2d code/barcode/img_1.png");

        // 检测条形码
        detector.DetectBarcode();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
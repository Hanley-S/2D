#include "Barcode.h"
#include <iostream>

int main() {
    try {
        // 创建 BarcodeDetector 对象
        BarcodeDetector detector("../img_3.png");

        // 检测条形码
        detector.detectBarcode();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
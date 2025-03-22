#include "image_processor.h"

int main() {
    const std::string folder_path = "E:/MVS_data/MV-CU120-10GC (K62277828)/";
    ImageProcessor::ImageProcessor processor(folder_path);
    processor.ProcessImages();
    return 0;
}
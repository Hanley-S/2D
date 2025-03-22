#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace ImageProcessor {

// 图像处理类
    class ImageProcessor {
    public:
        explicit ImageProcessor(const std::string& folder_path);
        void ProcessImages();

    private:
        // 从文件名中提取数字
        int ExtractNumber(const std::string& filename) const;

        // 处理单张图像
        void ProcessSingleImage(const std::string& image_path);

        // 检测圆并绘制结果
        cv::Mat DetectAndDrawCircles(const cv::Mat& image) const;

        std::string folder_path_;       // 文件夹路径
        std::set<std::string> processed_files_;  // 已处理的文件集合
        int total_processed_ = 0;      // 已处理文件总数
        bool window_created_ = false;  // 窗口是否已创建
    };

}  // namespace ImageProcessor

#endif  // IMAGE_PROCESSOR_H
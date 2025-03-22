#include "image_processor.h"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>

namespace fs = std::filesystem;

namespace ImageProcessor {

    ImageProcessor::ImageProcessor(const std::string& folder_path)
            : folder_path_(folder_path) {}

    void ImageProcessor::ProcessImages() {
        while (true) {
            std::vector<std::string> current_files;
            for (const auto& entry : fs::directory_iterator(folder_path_)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("Image_") != std::string::npos &&
                        filename.find(".png") != std::string::npos) {
                        current_files.push_back(filename);
                    }
                }
            }

            std::sort(current_files.begin(), current_files.end(),
                      [this](const std::string& a, const std::string& b) {
                          return ExtractNumber(a) < ExtractNumber(b);
                      });

            bool new_file_processed = false;
            for (const auto& filename : current_files) {
                if (processed_files_.find(filename) != processed_files_.end())
                    continue;

                std::string image_path = folder_path_ + filename;
                ProcessSingleImage(image_path);
                processed_files_.insert(filename);
                total_processed_++;
                new_file_processed = true;
            }

            if (!new_file_processed) {
                if (window_created_) {
                    cv::destroyWindow("Real-time detection");
                    window_created_ = false;
                }
                std::cout << "Monitoring... (processed " << total_processed_ << " images)"
                          << " ESC to quit" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    int ImageProcessor::ExtractNumber(const std::string& filename) const {
        size_t start = filename.find_last_of('_') + 1;
        size_t end = filename.find_last_of('.');
        return std::stoi(filename.substr(start, end - start));
    }

    void ImageProcessor::ProcessSingleImage(const std::string& image_path) {
        cv::Mat src = cv::imread(image_path);
        if (src.empty()) {
            std::cerr << "Unable to load image: " << image_path << std::endl;
            return;
        }

        cv::Mat resized_image;
        cv::resize(src, resized_image, cv::Size(src.cols / 5, src.rows / 5));

        cv::Mat result = DetectAndDrawCircles(resized_image);

        cv::imshow("Real-time detection", result);
        window_created_ = true;

        int key = cv::waitKey(0);
        if (key == 27) {
            std::cout << "\nTotal images processed: " << total_processed_ << std::endl;
            exit(0);
        }
    }

    cv::Mat ImageProcessor::DetectAndDrawCircles(const cv::Mat& image) const {
        cv::Mat gray_image, blur_image;
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
        cv::medianBlur(gray_image, blur_image, 3);

        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(blur_image, circles, cv::HOUGH_GRADIENT, 2, 70, 150, 40, 15, 18);

        cv::Mat result = image.clone();
        std::vector<cv::Point> centers;

        for (const auto& circle : circles) {
            cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
            int radius = cvRound(circle[2]);

            cv::circle(result, center, radius, cv::Scalar(0, 0, 255), 2);
            cv::circle(result, center, 3, cv::Scalar(0, 255, 0), -1);

            std::string coord_text = "(" + std::to_string(center.x) + "," +
                                     std::to_string(center.y) + ")";
            cv::putText(result, coord_text, center + cv::Point(10, -10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

            centers.push_back(center);
        }

        for (size_t i = 0; i < centers.size(); ++i) {
            for (size_t j = i + 1; j < centers.size(); ++j) {
                double dx = centers[j].x - centers[i].x;
                double dy = centers[j].y - centers[i].y;
                double distance = std::sqrt(dx * dx + dy * dy);

                cv::line(result, centers[i], centers[j], cv::Scalar(255, 0, 0), 2);
                cv::Point mid = (centers[i] + centers[j]) / 2;
                std::string dist_text = cv::format("%.2f px", distance);
                cv::putText(result, dist_text, mid + cv::Point(0, -10),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);
            }
        }

        return result;
    }

}  // namespace ImageProcessor
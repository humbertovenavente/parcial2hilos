#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "imageProcessing.h"

void applySepiaFilter(cv::Mat& image) {
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
            int outputRed = static_cast<int>(pixel[2] * 0.393 + pixel[1] * 0.769 + pixel[0] * 0.189);
            int outputGreen = static_cast<int>(pixel[2] * 0.349 + pixel[1] * 0.686 + pixel[0] * 0.168);
            int outputBlue = static_cast<int>(pixel[2] * 0.272 + pixel[1] * 0.534 + pixel[0] * 0.131);
            pixel[2] = static_cast<uchar>(std::min(255, outputRed));
            pixel[1] = static_cast<uchar>(std::min(255, outputGreen));
            pixel[0] = static_cast<uchar>(std::min(255, outputBlue));
        }
    }
}

// Function to process an image
void processImage(Task task) {
    // Load the image
    cv::Mat image = cv::imread(task.image_path);

    if (image.empty()) {
        std::cerr << "Error loading image: " << task.image_path << std::endl;
        return;
    }

    // Apply the selected filter
    if (task.filter_name == "Grayscale") {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    } else if (task.filter_name == "Sepia") {
        applySepiaFilter(image);
    } else if (task.filter_name == "Negative") {
    cv::bitwise_not(image, image);
    } else if (task.filter_name == "Cartoon") {
    cv::Mat gray, edges, cartoon;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, gray, 7);
    cv::adaptiveThreshold(gray, edges, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 9);
    cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);
    cv::bitwise_and(image, edges, cartoon);
    image = cartoon;
    }else if (task.filter_name == "HistogramEqualization") {
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY); // Convert to grayscale
    // Apply histogram equalization
    cv::equalizeHist(image, image);
    cv::cvtColor(image, image, cv::COLOR_GRAY2BGR); // Convert back to BGR
    } else {
        std::cerr << "Unknown filter: " << task.filter_name << std::endl;
        return;
    }

    // Save the processed image (you can display it or save it to a file)
    std::string output_filename = "processed_" + task.task_name + ".jpg";
    cv::imwrite(output_filename, image);
    std::cout << "Task '" << task.task_name << "' has completed processing." << std::endl;
}

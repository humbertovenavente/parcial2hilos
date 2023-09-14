#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>


// Define a structure to hold information about each task.
struct Task {
    std::string task_name;
    std::string image_path;
    std::string filter_name;
};

// Global shared data
std::vector<Task> task_queue; // Queue to store tasks
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for task queue
pthread_cond_t queue_condition = PTHREAD_COND_INITIALIZER; // Condition variable

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
        applySepiaFilter(image)
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

    // Print a message indicating the task is complete
    std::cout << "Task '" << task.task_name << "' has completed processing." << std::endl;
}

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

// Function to be executed by worker threads
void* workerThread(void* arg) {
    while (true) {
        // Lock the queue_mutex and wait until there's a task to process
        pthread_mutex_lock(&queue_mutex);

        // Check if there are tasks in the queue
        while (task_queue.empty()) {
            pthread_cond_wait(&queue_condition, &queue_mutex); // Wait for a signal
        }

        // Get the task from the queue
        Task task = task_queue.back();
        task_queue.pop_back();

        // Unlock the mutex to allow other threads to access the queue
        pthread_mutex_unlock(&queue_mutex);

        // Process the image
        processImage(task);
    }
    return nullptr;
}

int main() {
    const int num_threads = 4; // Number of worker threads

    // Create worker threads
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], nullptr, workerThread, nullptr);
    }

    // Add tasks to the queue (replace with your UI code)
    Task task1{"Task 1", "image1.jpg", "Filter 1"};
    Task task2{"Task 2", "image2.jpg", "Filter 2"};

    // Lock the queue_mutex and add tasks to the queue
    pthread_mutex_lock(&queue_mutex);
    task_queue.push_back(task1);
    task_queue.push_back(task2);
    pthread_mutex_unlock(&queue_mutex);

    // Signal the worker threads that there are tasks to process
    pthread_cond_broadcast(&queue_condition);

    // Wait for worker threads to finish (in a real application, you would use a signal to exit)
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}

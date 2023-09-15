#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <queue>
#include <wx/thread.h>

// Define a structure to hold information about each task.
struct Task {
    std::string task_name;
    std::string image_path;
    std::string filter_name;
};

extern std::vector<Task> task_queue; // Shared queue to store tasks
extern wxMutex queue_mutex; // Mutex for task queue
extern wxCondition queue_condition; // Condition variable
void processImage(Task task);
// Worker thread class
class WorkerThread : public wxThread {
public:
    WorkerThread() : wxThread(wxTHREAD_DETACHED) {}

protected:
    wxThread::ExitCode Entry() override {
        while (!TestDestroy()) {
            Task task;

            {
                wxMutexLocker lock(queue_mutex);

                // Wait for a signal or exit if the queue is empty
                while (task_queue.empty() && !TestDestroy()) {
                    queue_condition.Wait();
                }

                if (task_queue.empty()) {
                    break; // Exit the thread if it's terminated or the queue is empty
                }

                // Get the task from the queue
                task = task_queue.front();
                task_queue.erase(task_queue.begin());
            }

            // Process the image
            processImage(task);
        }

        return nullptr;
    }
};

#endif // IMAGE_PROCESSING_H

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <queue>
#include <wx/thread.h>
#include <wx/string.h>


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
    WorkerThread() : wxThread(wxTHREAD_JOINABLE), shouldExit(false) {}

    // Destructor to ensure proper thread termination
    ~WorkerThread() {
        if (IsRunning()) {
            Delete(); // This will safely terminate the thread
        }
    }

    // Add a setter method for shouldExit
    void SetShouldExit(bool exit) {
        wxMutexLocker lock(mutex);
        shouldExit = exit;
    }

private:
    bool shouldExit; // Flag to signal thread to exit gracefully
    wxMutex mutex; // Mutex for accessing shouldExit

protected:
    wxThread::ExitCode Entry() override {
        while (true) {
            // Check if the shouldExit flag is set
            {
                wxMutexLocker lock(mutex);
                if (shouldExit) {
                    std::cout << "Thread ID: " << wxThread::This()->GetId() << ", shouldExit: " << shouldExit << std::endl;
                    wxThread::Exit(); // Exit the thread gracefully
                }
            }
            Task task;

            {
                wxMutexLocker lock(queue_mutex);

                // Wait for a signal or exit if the queue is empty
                while (task_queue.empty() && !TestDestroy()) {
                    queue_condition.Wait();
                }

                if (TestDestroy()) {
                    std::cout << "Thread ID: " << wxThread::This()->GetId() << ", terminated" << std::endl;
                    wxThread::Exit(); // Exit the thread if it's terminated
                }

                if (task_queue.empty()) {
                    continue; // Continue to check the shouldExit flag
                }

                // Get the task from the queue
                task = task_queue.front();
                task_queue.erase(task_queue.begin());

                std::cout << "Task '" << task.task_name << "' entered" << std::endl;
            }

            std::cout << "ready to process" << std::endl;
            // Process the image
            processImage(task);
        }
        std::cout << "Task mainloop exited" << std::endl;
        wxThread::Exit();
    }
};


#endif // IMAGE_PROCESSING_H
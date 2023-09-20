#include <wx/wx.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "imageProcessing.h"
#include <chrono>
#include <thread>

std::vector<Task> task_queue;
wxMutex queue_mutex;
wxCondition queue_condition(queue_mutex);


class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size),
    imageDisplay(nullptr),
    imageDisplay1(nullptr),
    imageDisplay2(nullptr),
    imageDisplay3(nullptr),
    imageDisplay4(nullptr),
    imageDisplay5(nullptr){
        
        // Crear panel principal
        wxPanel* panel = new wxPanel(this);

        // Botones para agregar y eliminar tareas
        wxButton* addButton = new wxButton(panel, wxID_ANY, "Agregar Tarea", wxPoint(10, 10));
        wxButton* removeButton = new wxButton(panel, wxID_ANY, "Eliminar Tarea", wxPoint(120, 10));
        // Image
        imageDisplay = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(10, 50), wxSize(100, 100));
        imageDisplay1 = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(220, 50), wxSize(100, 100));
        imageDisplay2 = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(430, 50), wxSize(100, 100));
        imageDisplay3 = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(640, 50), wxSize(100, 100));
        imageDisplay4 = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(860, 50), wxSize(100, 100));
        imageDisplay5 = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxPoint(1080, 50), wxSize(100, 100));
        // Conectar eventos a los botones
        addButton->Bind(wxEVT_BUTTON, &MyFrame::OnAddTask, this);
        removeButton->Bind(wxEVT_BUTTON, &MyFrame::OnRemoveTask, this);

        // Configurar el diseño del panel
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(addButton, 0, wxALL, 10);
        sizer->Add(removeButton, 0, wxALL, 10);
        panel->SetSizer(sizer);

        // Configurar el menú
        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(wxID_EXIT, "Salir\tAlt-X");

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "Archivo");

        SetMenuBar(menuBar);

        // Configurar eventos de cierre
        Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);
    }

    // Method to set shouldExit for all worker threads
    void SetShouldExitForThreads(bool exit) {
        for (auto& thread : worker_threads) {
            thread->SetShouldExit(exit);
        }
    }

private:
    std::vector<WorkerThread*> worker_threads;
    static int taskCounter; // Add a static task counter
    wxStaticBitmap* imageDisplay;
    wxStaticBitmap* imageDisplay1; // Declare imageDisplay1
    wxStaticBitmap* imageDisplay2; // Declare imageDisplay2
    wxStaticBitmap* imageDisplay3; // Declare imageDisplay3
    wxStaticBitmap* imageDisplay4; // Declare imageDisplay4
    wxStaticBitmap* imageDisplay5;

    void DisplayImage1(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay1) {
                    imageDisplay1->SetSize(wxSize(200, 200));
                    imageDisplay1->SetBitmap(bitmap);
                    imageDisplay1->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }
    void DisplayImage2(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay2) {
                    imageDisplay2->SetSize(wxSize(200, 200));
                    imageDisplay2->SetBitmap(bitmap);
                    imageDisplay2->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }void DisplayImage3(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay3) {
                    imageDisplay3->SetSize(wxSize(200, 200));
                    imageDisplay3->SetBitmap(bitmap);
                    imageDisplay3->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }void DisplayImage4(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay4) {
                    imageDisplay4->SetSize(wxSize(200, 200));
                    imageDisplay4->SetBitmap(bitmap);
                    imageDisplay4->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }void DisplayImage5(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay5) {
                    imageDisplay5->SetSize(wxSize(200, 200));
                    imageDisplay5->SetBitmap(bitmap);
                    imageDisplay5->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }
    void DisplayImage(const wxString& imagePath) {
        if (!imagePath.IsEmpty()) {
            wxImage image(imagePath, wxBITMAP_TYPE_ANY);
            if (image.IsOk()) {
                // Resize the image to 100x100 pixels
                image.Rescale(200, 200, wxIMAGE_QUALITY_HIGH); // Adjust the quality if needed

                // Create a wxBitmap from the resized image
                wxBitmap bitmap(image);

                // Set the size of the wxStaticBitmap control
                if (imageDisplay) {
                    imageDisplay->SetSize(wxSize(200, 200));
                    imageDisplay->SetBitmap(bitmap);
                    imageDisplay->Layout();
                }
            } else {
                // Handle the case where image loading failed
                wxMessageBox("Failed to load the image.", "Image Loading Error", wxOK | wxICON_ERROR, this);
            }
        }
    }

    // Create and start the worker threads
    void CreateWorkerThreads(int num_threads) {
        for (int i = 0; i < num_threads; ++i) {
            WorkerThread* thread = new WorkerThread();
            thread->Create();
            thread->Run();
            worker_threads.push_back(thread);
        }
    }

    void OnAddTask(wxCommandEvent& event) {
    // Open a file dialog to select an image
        wxFileDialog openFileDialog(this, _("Open Image File"), "", "", 
                                    "Image files (*.jpg;*.png)|*.jpg;*.png", 
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return; // User canceled the operation
        }

        wxString imagePath = openFileDialog.GetPath();
        wxString filterNames[] = { "Grayscale", "Sepia", "Negative", "Cartoon", "HistogramEqualization" };
        for (const auto& filterName : filterNames) {
            // Create the task based on user input
            Task task;
            task.task_name = "Task" + wxString::Format("%d", taskCounter);
            task.image_path = imagePath.ToStdString();
            task.filter_name = filterName;

            // Increment the task counter
            taskCounter++;

            // Lock the queue_mutex and add the task to the queue
            wxMutexLocker lock(queue_mutex);
            task_queue.push_back(task); // Add a task to the end of the vector

            // Signal the worker threads that there's a new task to process
            queue_condition.Broadcast();
            // Call DisplayImage to display the filtered image
            DisplayImage(task.image_path);
        }
        // Create and start the worker threads (if not already created)
        if (worker_threads.empty()) {
            CreateWorkerThreads(4); // You can specify the number of threads you want here
            // Add debugging output
            std::cout << "Worker threads created and started." << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(6));
        // Call DisplayImage to display the filtered image
        DisplayImage1("processed_Task1.png");
        DisplayImage2("processed_Task2.png");
        DisplayImage3("processed_Task3.png");
        DisplayImage4("processed_Task4.png");
        DisplayImage5("processed_Task5.png");
    }

    void OnRemoveTask(wxCommandEvent& event) {
        // Create a list of task names for the user to select from
        std::vector<std::string> taskNames;
        
        // Lock the queue_mutex to access the task_queue safely
        wxMutexLocker lock(queue_mutex);
        std::queue<Task> temp_queue;
        while (!task_queue.empty()) {
            const Task& task = task_queue.front();
            taskNames.push_back(task.task_name);
            task_queue.erase(task_queue.begin());
        }
        
        if (taskNames.empty()) {
            wxMessageBox("No tasks to remove.", "Task Removal", wxOK | wxICON_INFORMATION, this);
            return;
        }

        // Display a dialog for task selection
        wxArrayString taskNamesArray;
        for (const auto& taskName : taskNames) {
            taskNamesArray.Add(taskName);
        }
        wxString selectedTask = wxGetSingleChoice("Select a task to remove:", "Remove Task", taskNamesArray, this);
        
        if (selectedTask.IsEmpty()) {
            return; // User canceled task removal
        }

        // Find the task to remove and remove it from the global task_queue
        auto it = std::remove_if(task_queue.begin(), task_queue.end(),
            [selectedTask](const Task& task) {
                return task.task_name == selectedTask;
            });

        if (it != task_queue.end()) {
            task_queue.erase(it, task_queue.end());
            wxMessageBox("Task removed successfully.", "Task Removal", wxOK | wxICON_INFORMATION, this);
        } else {
            wxMessageBox("Task not found.", "Task Removal", wxOK | wxICON_ERROR, this);
        }

    }

    void OnClose(wxCloseEvent& event) {
        int result = wxMessageBox("Are you sure you want to exit?", "Confirm Exit", wxYES_NO | wxICON_QUESTION, this);
        if (result == wxYES) {
            std::cout << "Exiting Tasks..." << std::endl;
            // Set the shouldExit flag for all worker threads
            for (auto& thread : worker_threads) {
                std::cout << "exit for thread: " << thread << std::endl; // Debug output
                thread->SetShouldExit(true);
            }

            // Signal the worker threads to wake up and exit
            {
                wxMutexLocker lock(queue_mutex);
                queue_condition.Broadcast();
            }

            // Wait for all worker threads to finish gracefully
            for (auto& thread : worker_threads) {
                thread->Wait(); // Wait for the thread to finish
            }

            // Clean up worker threads
            for (auto& thread : worker_threads) {
                delete thread;
            }
            worker_threads.clear();

            std::cout << "Closing window" << std::endl;

            // Close the frame
            Destroy();
        } else {
            event.Veto(); // Prevent the window from closing
            std::cout << "Close vetoed." << std::endl; // Add this line for debugging
        }
}


    wxDECLARE_EVENT_TABLE();
};


int MyFrame::taskCounter = 1; // Initialize the static task counter

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        wxImage::AddHandler(new wxPNGHandler);
        MyFrame* frame = new MyFrame("Gestor de Tareas de Procesamiento de Imágenes", wxPoint(50, 50), wxSize(800, 600));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP_NO_MAIN(MyApp);

// Define your custom main function
int main(int argc, char* argv[]) {
    // Initialize the wxWidgets framework
    wxApp::SetInstance(new MyApp());
    
    // Initialize and run the application
    wxEntryStart(argc, argv);
    wxTheApp->OnInit();
    wxTheApp->OnRun();
    wxTheApp->OnExit();
    
    // Clean up and exit
    wxEntryCleanup();
    
    return 0;
}
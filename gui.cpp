#include <wx/wx.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "imageProcessing.h"

wxMutex queue_mutex;

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size) {
        
        // Crear panel principal
        wxPanel* panel = new wxPanel(this);

        // Botones para agregar y eliminar tareas
        wxButton* addButton = new wxButton(panel, wxID_ANY, "Agregar Tarea", wxPoint(10, 10));
        wxButton* removeButton = new wxButton(panel, wxID_ANY, "Eliminar Tarea", wxPoint(120, 10));

        // Área para mostrar imágenes originales y procesadas
        originalImage = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(300, 200)), wxPoint(10, 50));
        processedImage = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxImage(300, 200)), wxPoint(320, 50));

        // Conectar eventos a los botones
        addButton->Bind(wxEVT_BUTTON, &MyFrame::OnAddTask, this);
        removeButton->Bind(wxEVT_BUTTON, &MyFrame::OnRemoveTask, this);

        // Configurar el diseño del panel
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(addButton, 0, wxALL, 10);
        sizer->Add(removeButton, 0, wxALL, 10);
        sizer->Add(originalImage, 0, wxALL, 10);
        sizer->Add(processedImage, 0, wxALL, 10);
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

private:
    wxStaticBitmap* originalImage;
    wxStaticBitmap* processedImage;
    std::vector<WorkerThread*> worker_threads;

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
        wxString selectedFilter = wxGetSingleChoice("Select a filter to apply:", "Filter Selection", 
                                                    WXSIZEOF(filterNames), filterNames, this);

        if (selectedFilter.IsEmpty()) {
            return; // User canceled filter selection
        }

        // Create the task based on user input
        Task task;
        task.task_name = "Task " + wxString::Format("%d", task_queue.size() + 1);
        task.image_path = imagePath.ToStdString();
        task.filter_name = selectedFilter.ToStdString();

        // Lock the queue_mutex and add the task to the queue
        wxMutexLocker lock(queue_mutex);
        task_queue.push_back(task); // Add a task to the end of the vector

        // Signal the worker threads that there's a new task to process
        queue_condition.Broadcast();
        // Create and start the worker threads (if not already created)
        if (worker_threads.empty()) {
            CreateWorkerThreads(4); // You can specify the number of threads you want here
        }
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
        // Lock the queue_mutex using wxMutexLocker for thread safety
        wxMutexLocker lock(queue_mutex);

        // Signal worker threads to exit and wait for them to finish
        for (auto& thread : worker_threads) {
            thread->Delete();
        }

        // Wait for threads to finish (Delete will terminate the threads gracefully)
        for (auto& thread : worker_threads) {
            thread->Wait();
        }

        event.Skip(); // Allow the GUI to close
    } else {
        event.Veto(); // Prevent the window from closing
    }
}


    // Display the original and processed images
    void DisplayImages(const wxString& originalImagePath, const wxString& processedImagePath) {
        wxImage originalImage(originalImagePath, wxBITMAP_TYPE_ANY);
        wxImage processedImage(processedImagePath, wxBITMAP_TYPE_ANY);

        if (originalImage.IsOk() && processedImage.IsOk()) {
            originalImage.Rescale(300, 200);
            processedImage.Rescale(300, 200);

            wxStaticBitmap* originalBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(originalImage));
            wxStaticBitmap* processedBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(processedImage));

            // Set positions for the bitmaps or use sizers for layout
            originalBitmap->SetPosition(wxPoint(10, 50));
            processedBitmap->SetPosition(wxPoint(320, 50));

            // Refresh and update bitmaps
            originalBitmap->Refresh();
            processedBitmap->Refresh();

            originalBitmap->Update();
            processedBitmap->Update();
        }
    }


    wxDECLARE_EVENT_TABLE();
};

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

wxIMPLEMENT_APP(wxApp);
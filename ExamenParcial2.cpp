#include <wx/wx.h>
#include <wx/image.h>

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

    void OnAddTask(wxCommandEvent& event) {
        // Lógica para agregar una nueva tarea
    }

    void OnRemoveTask(wxCommandEvent& event) {
        // Lógica para eliminar una tarea existente
    }

    void OnClose(wxCloseEvent& event) {
        // Lógica de cierre de la ventana
        Destroy();
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

wxIMPLEMENT_APP(MyApp);
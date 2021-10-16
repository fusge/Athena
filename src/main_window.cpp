#include "main_window.h"
#include "chessgame.h"
#include "ChesspeerConfig.h"
#include <string>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/msgout.h>
#include <wx/string.h>
#include <wx/image.h>
#include <wx/statbmp.h>

enum
{
    ID_Hello = 1
};

bool UI::Chesspeer::OnInit()
{
    UI::cpMainWindow *frame = new UI::cpMainWindow();
    frame->Show(true);
    return true;
}

UI::cpMainWindow::cpMainWindow()
    : wxFrame(NULL, wxID_ANY, "Chesspeer")
{
    // Load the menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();

    // Create chessboard
    this->board_image = new wxImage();
    this->board_panel_id = wxWindow::NewControlId(1);
    this->board_panel = new wxPanel(this, board_panel_id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, "Chessboard");
    this->loadImages();
    wxBitmap* cpBoardBitmap = new wxBitmap(*board_image, wxBITMAP_SCREEN_DEPTH);
    wxStaticBitmap* static_bitmap = new wxStaticBitmap(this, this->board_panel_id, *cpBoardBitmap, wxDefaultPosition, wxDefaultSize, 0, "Chessboard Bitmap");
    //wxClientDC* dc = new wxClientDC(board_panel);
    //dc->DrawBitmap(*cpBoardBitmap, wxPoint(0, 0), false);
    //cpBoard = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 400));
    //cpBoard->CreateGrid(8, 8);

 
    SetStatusText("Chesspeer loaded successfully!");
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnExit, this, wxID_EXIT);
}

void UI::cpMainWindow::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void UI::cpMainWindow::OnAbout(wxCommandEvent& event)
{
    std::string version = "Chesspeer version ";
    version += std::to_string(CHESSPEER_VERSION_MAJOR);
    version += ".";
    version += std::to_string(CHESSPEER_VERSION_MINOR);
    version += ".";
    version += std::to_string(CHESSPEER_VERSION_PATCH);
    
    wxString message (version);
    wxMessageBox(message , "About Chesspeer", wxOK | wxICON_INFORMATION);
}

void UI::cpMainWindow::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Chesspeer UI loadded");
}

void UI::cpMainWindow::loadImages() {
    wxString message("Loading board image in directory ");
    wxImage::AddHandler(new wxPNGHandler);
    wxFileName img_path = wxFileName::DirName(wxGetCwd());
    img_path.RemoveLastDir();
    img_path.AppendDir("data");
    img_path.SetName("chessboard");
    img_path.SetExt("png");
    SetStatusText(message.append(wxGetCwd()));
    if (!this->board_image->LoadFile(img_path.GetFullPath(), wxBITMAP_TYPE_PNG, -1)) {
        message = "Unable to load Board image:\n ";
        message.append(img_path.GetFullPath());
        message.append("\n File missing?");
        wxMessageBox("Unable to load file");
        return;
    }
    message = "Board Image loaded";
    SetStatusText(message);
}

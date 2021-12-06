#include "main_window.h"
#include "chessgame.h"
#include "ChesspeerConfig.h"
#include <string>
#include <iostream>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/msgout.h>
#include <wx/string.h>
#include <wx/image.h>
#include <wx/statbmp.h>

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
    menuFile->Append(UI::ID_Hello, "&Hello...\tCtrl-H",
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

    // create a generic window panel
    this->window_panel = new wxPanel(this, 
                                    wxID_ANY, 
                                    wxDefaultPosition, 
                                    wxDefaultSize, 
                                    wxTAB_TRAVERSAL,
                                    "chesspeer window");
    window_panel->SetBackgroundColour(wxColour("#4f5049"));

    // Create chessboard panel
    //wxSize boardsize(400, 400);
    this->board_panel_id = wxWindow::NewControlId(1);

    this->board_panel = new cpBoardPanel(window_panel, board_panel_id);
    this->board_panel->SetBackgroundColour(wxColor("#ededed"));
    wxBoxSizer* board_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    board_panel_sizer->Add(board_panel, 1, wxEXPAND | wxALL, 10);
    this->window_panel->SetSizer(board_panel_sizer);

    // Start loading images and data
    this->loadImages(); // Load all images from disk, including board and pieces
    //cpBoardBitmap = new wxBitmap(*board_image, wxBITMAP_SCREEN_DEPTH);
    //wxStaticBitmap* static_bitmap = new wxStaticBitmap(this->board_panel, 
    //                                                   this->board_panel_id, 
    //                                                   *cpBoardBitmap, 
    //                                                   wxDefaultPosition, 
    //                                                   wxDefaultSize, 
    //                                                   0, 
    //                                                   "Chessboard Bitmap");

    // Finish startup
    SetStatusText("Chesspeer loaded successfully!");
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnHello, this, UI::ID_Hello);
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnExit, this, wxID_EXIT);
}

void UI::cpMainWindow::loadImages()
{
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

void UI::cpBoardPanel::loadBoardImage() {
    wxString message("Loading board image in directory ");
    wxImage::AddHandler(new wxPNGHandler);
    wxFileName img_path = wxFileName::DirName(CHESSPEER_BINARY_DIR);
    img_path.AppendDir("data");
    img_path.SetName("chessboard");
    img_path.SetExt("png");
    this->board_image = new wxImage();
    if (!this->board_image->LoadFile(img_path.GetFullPath(), wxBITMAP_TYPE_PNG, -1)) 
    {
        message = "Unable to load Board image:\n ";
        message.append(img_path.GetFullPath());
        message.append("\n File missing?");
        wxMessageBox("Unable to load file");
        return;
    }
}

void UI::cpBoardPanel::onBoardPanelResize(wxSizeEvent &event)
{
    this->Refresh();
    event.Skip();
}

void UI::cpBoardPanel::render(wxDC &dc)
{
    auto width = dc.GetSize().GetWidth();
    auto height = dc.GetSize().GetHeight();
    auto minSize = (width < height) ? width : height; 
    wxImage tempImage = board_image->Scale(minSize, minSize);
    wxBitmap tempBitmap(tempImage);
    //wxStaticBitmap* static_bitmap = new wxStaticBitmap(this, 
    //                                                   this->GetId(), 
    //                                                   *cpBoardBitmap, 
    //                                                   wxDefaultPosition, 
    //                                                   wxDefaultSize, 
    //                                                   0, 
    //                                                   "Chessboard Bitmap");
    //this->Refresh();
    //this->Update();
    dc.DrawBitmap(tempBitmap, 0, 0);
}

void UI::cpBoardPanel::paintEventHandler(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    render(dc);
}

void UI::cpBoardPanel::paintNow()
{
    wxClientDC dc(this);
    this->render(dc);
}

void UI::cpBoardPanel::clearBackground(wxEraseEvent &event)
{
    wxDC *dc = event.GetDC();
    dc->Clear();
    event.Skip();
}

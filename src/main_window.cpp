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
    : wxFrame(NULL, wxID_ANY, "Chesspeer", wxDefaultPosition, wxSize(600,600))
{
    // Load the menu bar
    wxMenu *menuFile = new wxMenu;
    //menuFile->AppendSeparator();
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
    this->board_panel_id = wxWindow::NewControlId(1);

    this->board_panel = new cpBoardPanel(window_panel, board_panel_id);
    this->board_panel->SetBackgroundColour(wxColor("#ededed"));
    wxBoxSizer* board_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    board_panel_sizer->Add(board_panel, 1, wxEXPAND | wxALL, 10);
    this->window_panel->SetSizer(board_panel_sizer);

    // Finish startup
    SetStatusText("Chesspeer loaded successfully!");
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

void UI::cpBoardPanel::loadImages() {
    // load chessboard image 
    wxString message("Loading board image in directory ");
    wxImage::AddHandler(new wxPNGHandler);
    wxFileName img_path = wxFileName::DirName(CHESSPEER_BINARY_DIR);
    img_path.AppendDir("data");
    img_path.SetName("chessboard");
    img_path.SetExt("png");
    this->board_image = new wxImage();
    if (!this->board_image->LoadFile(img_path.GetFullPath(), wxBITMAP_TYPE_PNG, -1)) 
    {
        //message = "Unable to load Board image:\n ";
        //message.append(img_path.GetFullPath());
        //message.append("\n File missing?");
        wxMessageBox("Unable to load file");
    }

    // Load piece images
    std::array<std::string, 6> generalPieceNames = {"pawn", "rook", "bishop", "knight",
                                                    "queen", "king"};
    std::array<std::string, 2> colors = {"w", "b"};
    std::string name;
    std::array<imageInfo, 12>::iterator image_iter = piece_images.begin();
    std::string filename;
    wxFileName tempFilename;
    for(auto name = generalPieceNames.begin(); name != generalPieceNames.end(); ++name)
    {
        for(auto color = colors.begin(); color != colors.end(); ++color)
        {
            image_iter->name = std::string(*name);
            image_iter->color = *color;
            filename = std::string(*color);
            filename.append("_");
            filename.append(*name);
            tempFilename = wxFileName::DirName(CHESSPEER_BINARY_DIR);
            tempFilename.AppendDir("data");
            tempFilename.SetName(filename);
            tempFilename.SetExt("png");
            image_iter->image = new wxImage();
            image_iter->image->LoadFile(tempFilename.GetFullPath(), wxBITMAP_TYPE_PNG, -1);
            ++image_iter;
        }
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

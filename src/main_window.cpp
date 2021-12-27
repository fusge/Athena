#include "main_window.h"
#include "chessgame.h"
#include "AthenaConfig.h"

#include <string>
#include <iostream>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/msgout.h>
#include <wx/string.h>
#include <wx/image.h>
#include <wx/statbmp.h>

bool UI::Athena::OnInit()
{
    UI::cpMainWindow *frame = new UI::cpMainWindow();
    frame->Show(true);
    return true;
}

UI::cpMainWindow::cpMainWindow()
    : wxFrame(NULL, wxID_ANY, "Athena", wxDefaultPosition, wxSize(600,600))
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
                                    "Athena");
    window_panel->SetBackgroundColour(wxColour("#4f5049"));

    // Create chessboard panel
    this->board_panel_id = wxWindow::NewControlId(1);

    this->board_panel = new cpBoardPanel(window_panel, board_panel_id);
    this->board_panel->SetBackgroundColour(wxColor("#ededed"));
    wxBoxSizer* board_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    board_panel_sizer->Add(this->board_panel, 1, wxEXPAND | wxALL, 10);
    this->window_panel->SetSizer(board_panel_sizer);


    // Finish startup
    SetStatusText("Athena loaded successfully!");
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &UI::cpMainWindow::OnExit, this, wxID_EXIT);
}

void UI::cpMainWindow::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void UI::cpMainWindow::OnAbout(wxCommandEvent& event)
{
    std::string version = "Athena version ";
    version += std::to_string(ATHENA_VERSION_MAJOR);
    version += ".";
    version += std::to_string(ATHENA_VERSION_MINOR);
    version += ".";
    version += std::to_string(ATHENA_VERSION_PATCH);
    
    wxString message (version);
    wxMessageBox(message , "About Athena", wxOK | wxICON_INFORMATION);
}

UI::cpBoardPanel::cpBoardPanel(wxWindow * win, wxWindowID id): wxPanel(win, id)
{
    // Initialize game logic
    this->game_system = std::make_unique<Core::Chessgame>();

    // Load images and spacing
    this->loadImages(); 
    wxGridSizer* board_grid_sizer = new wxGridSizer(8, 8, 0, 0);
    for(auto iter = board_squares.begin(); iter != board_squares.end(); ++iter)
    {
        iter->square_panel = new wxPanel(this, wxID_ANY);
        iter->square_panel->SetTransparent(0);
        board_grid_sizer->Add(iter->square_panel, 1, wxEXPAND | wxALL, 0); 
    }
    this->SetSizer(board_grid_sizer);

    // synchronize board state with UI
    for(int iter = 0; iter < 64; ++iter)
    {
        board_squares[iter].piece_type = (Core::Piece_t) this->game_system->identifyPiece(iter); 
    }
    board_grid_sizer->Layout();

    // Bind drawing events to appropriate handlers
    Bind(wxEVT_SIZE, &UI::cpBoardPanel::onBoardPanelResize, this);
    Bind(wxEVT_ERASE_BACKGROUND, &UI::cpBoardPanel::clearBackground, this);
    Bind(wxEVT_PAINT, &UI::cpBoardPanel::paintEventHandler, this);
}

void UI::cpBoardPanel::loadImages() {
    // load chessboard image 
    wxString message("Loading board image in directory ");
    wxImage::AddHandler(new wxPNGHandler);
    wxFileName img_path = wxFileName::DirName(ATHENA_BINARY_DIR);
    img_path.AppendDir("data");
    img_path.SetName("chessboard");
    img_path.SetExt("png");
    this->board_image = new wxImage();
    if (!this->board_image->LoadFile(img_path.GetFullPath(), wxBITMAP_TYPE_PNG, -1)) 
    {
        wxMessageBox("Unable to load file");
    }

    // Load piece images
    std::array<std::string, 6> generalPieceNames = {"pawn", "rook", "bishop", "knight",
                                                    "queen", "king"};
    std::array<std::string, 2> colors = {"w", "b"};
    std::array<imageInfo, 12>::iterator image_iter = piece_images.begin();
    std::string filename;
    wxFileName tempFilename;
    for(auto name = generalPieceNames.begin(); name != generalPieceNames.end(); ++name)
    {
        for(auto color = colors.begin(); color != colors.end(); ++color)
        {
            filename = std::string(*color);
            filename.append("_");
            filename.append(*name);
            tempFilename = wxFileName::DirName(ATHENA_BINARY_DIR);
            tempFilename.AppendDir("data");
            tempFilename.SetName(filename);
            tempFilename.SetExt("png");
            image_iter->filename = filename;
            image_iter->image = new wxImage();
            image_iter->image->LoadFile(tempFilename.GetFullPath(), wxBITMAP_TYPE_PNG, -1);

            if(*name == "pawn"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_PAWN;
                else
                    image_iter->piece_type = Core::BLACK_PAWN;
            }
            else if(*name == "rook"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_ROOK;
                else
                    image_iter->piece_type = Core::BLACK_ROOK;
            }
            else if(*name == "bishop"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_BISHOP;
                else
                    image_iter->piece_type = Core::BLACK_BISHOP;
            }
            else if(*name == "knight"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_KNIGHT;
                else
                    image_iter->piece_type = Core::BLACK_KNIGHT;
            }
            else if(*name == "queen"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_QUEEN;
                else
                    image_iter->piece_type = Core::BLACK_QUEEN;
            }
            else if(*name == "king"){
                if (*color == "w")
                    image_iter->piece_type = Core::WHITE_KING;
                else
                    image_iter->piece_type = Core::BLACK_KING;
            }
            else
                image_iter->piece_type = Core::EMPTY_SQUARE;
            ++image_iter;
        }
    }
}

void UI::cpBoardPanel::onBoardPanelResize(wxSizeEvent &event)
{
    this->Refresh();
    for (auto iter = board_squares.begin(); iter != board_squares.end(); ++iter)
    {
        iter->square_panel->Refresh();
    }
    event.Skip();
}

void UI::cpBoardPanel::render(wxDC &dc)
{
    auto width = dc.GetSize().GetWidth();
    auto height = dc.GetSize().GetHeight();
    auto minSize = (width < height) ? width : height; 

    // Draw the board
    wxImage tempImage = this->board_image->Scale(minSize, minSize);
    wxBitmap tempBitmap(tempImage);
    dc.DrawBitmap(tempBitmap, 0, 0);

    // Draw the pieces
    wxImage tempPieceImage;
    wxBitmap tempPieceBitmap;
    Core::Piece_t piece_type;
    int indx;
    for(int iter = 0; iter < 64; ++iter)
    {
        piece_type = (Core::Piece_t) this->game_system->identifyPiece(iter);
        // Don't bother drawing empty squares
        if (piece_type == Core::EMPTY_SQUARE) continue;
        for(indx = 0; indx < 12; ++indx)
        {
            //std::cout << (char) piece_images[indx].piece_type << " " << indx << std::endl;
            if (piece_images[indx].piece_type == piece_type) break;
        }
        tempPieceImage = piece_images[indx].image->Scale(minSize/8, minSize/8);
        tempPieceBitmap = wxBitmap(tempPieceImage);
        wxClientDC square_dc(board_squares[iter].square_panel);
        square_dc.DrawBitmap(tempPieceBitmap, 2, 2);
    }
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

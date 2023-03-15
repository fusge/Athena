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
    UI::MainWindow *frame = new UI::MainWindow();
    frame->Show(true);
    return true;
}

UI::MainWindow::MainWindow()
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

    // Describe overall layout with sizers 
    wxBoxSizer* main_window_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* board_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* information_region_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pgn_view_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* chat_box_sizer = new wxBoxSizer(wxVERTICAL);
    this->window_panel->SetSizer(main_window_sizer);
    main_window_sizer->Add(board_panel_sizer, 1, wxSHAPED);
    main_window_sizer->Add(information_region_sizer, 0);
    information_region_sizer->Add(pgn_view_sizer, 1);
    information_region_sizer->Add(chat_box_sizer, 1);


    // Create chessboard panel
    this->board_panel_id = wxWindow::NewControlId(1);

    this->board_panel = new BoardPanel(window_panel, board_panel_id);
    this->board_panel->SetBackgroundColour(wxColor("#ededed"));
    board_panel_sizer->Add(this->board_panel, 1, wxEXPAND | wxALL, 10);
    //this->window_panel->SetSizer(board_panel_sizer);


    // Finish startup
    SetStatusText("Athena loaded successfully!");
    Bind(wxEVT_MENU, &UI::MainWindow::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &UI::MainWindow::OnExit, this, wxID_EXIT);
    //Bind(wxEVT_SIZE, &UI::MainWiondow::onMainWindowResize, this);
}

void UI::MainWindow::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void UI::MainWindow::OnAbout(wxCommandEvent& event)
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

//void UI::MainWindow::onMainWindowResize(wxSizeEvent& event) 
//{
//    wxSize windowSize = event.GetSize();    
//    this->
//}

UI::BoardPanel::BoardPanel(wxWindow * win, wxWindowID id): wxPanel(win, id)
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
    Bind(wxEVT_SIZE, &UI::BoardPanel::onBoardPanelResize, this);
    Bind(wxEVT_ERASE_BACKGROUND, &UI::BoardPanel::clearBackground, this);
    Bind(wxEVT_PAINT, &UI::BoardPanel::paintEventHandler, this);
}

void UI::BoardPanel::loadImages() {
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

void UI::BoardPanel::onBoardPanelResize(wxSizeEvent &event)
{
    this->Refresh();
    for (auto iter = board_squares.begin(); iter != board_squares.end(); ++iter)
    {
        iter->square_panel->Refresh();
    }
    event.Skip();
}

void UI::BoardPanel::render(wxDC &dc)
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

void UI::BoardPanel::paintEventHandler(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    render(dc);
}

void UI::BoardPanel::paintNow()
{
    wxClientDC dc(this);
    this->render(dc);
}

void UI::BoardPanel::clearBackground(wxEraseEvent &event)
{
    wxDC *dc = event.GetDC();
    dc->Clear();
    event.Skip();
}

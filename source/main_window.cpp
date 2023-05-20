#include <iostream>
#include <string>

#include "main_window.h"

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/msgout.h>
#include <wx/statbmp.h>
#include <wx/string.h>

#include "AthenaConfig.h"
#include "chessgame.h"

bool ui::athena::OnInit()
{
  auto* frame = new ui::main_window();
  frame->Show(true);
  return true;
}

ui::main_window::main_window()
    : wxFrame(NULL, 
              wxID_ANY, 
              "Athena", 
              wxDefaultPosition, 
              wxSize(ui::default_width, ui::default_height)), 
      m_board_panel_id(wxWindow::NewControlId(1)), 
      m_window_panel(new wxPanel(this,
                                 wxID_ANY,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTAB_TRAVERSAL,
                                 "Athena")),
      m_board_panel(new board_panel(m_window_panel, m_board_panel_id))
{
  // Load the menu bar
  auto* menu_file = new wxMenu;
  // menuFile->AppendSeparator();
  menu_file->Append(wxID_EXIT);
  auto* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);
  auto* menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar();

  // create a generic window panel
  
  m_window_panel->SetBackgroundColour(wxColour("#4f5049"));

  // Describe overall layout with sizers
  auto* main_window_sizer = new wxBoxSizer(wxHORIZONTAL);
  auto* board_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
  auto* information_region_sizer = new wxBoxSizer(wxVERTICAL);
  auto* pgn_view_sizer = new wxBoxSizer(wxVERTICAL);
  auto* chat_box_sizer = new wxBoxSizer(wxVERTICAL);
  this->m_window_panel->SetSizer(main_window_sizer);
  main_window_sizer->Add(board_panel_sizer, 1, wxSHAPED);
  main_window_sizer->Add(information_region_sizer, 0);
  information_region_sizer->Add(pgn_view_sizer, 1);
  information_region_sizer->Add(chat_box_sizer, 1);

  // Create chessboard panel
  this->m_board_panel->SetBackgroundColour(wxColor("#ededed"));
  board_panel_sizer->Add(this->m_board_panel, 1, wxEXPAND | wxALL, 10);

  // Finish startup
  SetStatusText("Athena loaded successfully!");
  Bind(wxEVT_MENU, &ui::main_window::on_about, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &ui::main_window::on_exit, this, wxID_EXIT);
}

void ui::main_window::on_exit(wxCommandEvent&  /*event*/)
{
  Close(true);
}

void ui::main_window::on_about(wxCommandEvent&  /*event*/)
{
  std::string version = "Athena version ";
  version += std::to_string(ATHENA_VERSION_MAJOR);
  version += ".";
  version += std::to_string(ATHENA_VERSION_MINOR);
  version += ".";
  version += std::to_string(ATHENA_VERSION_PATCH);

  wxString message(version);
  wxMessageBox(message, "About Athena", wxOK | wxICON_INFORMATION);
}

// void ui::MainWindow::onMainWindowResize(wxSizeEvent& event)
//{
//     wxSize windowSize = event.GetSize();
//     this->
// }

ui::board_panel::board_panel(wxWindow* win, wxWindowID id)
    : wxPanel(win, id)
{
  // Initialize game logic
  this->m_game_system = std::make_unique<core::chessgame>();

  // Load images and spacing
  this->load_images();
  auto* board_grid_sizer = new wxGridSizer(8, 8, 0, 0);
  for (auto & m_board_square : m_board_squares) {
    m_board_square.square_panel = new wxPanel(this, wxID_ANY);
    m_board_square.square_panel->SetTransparent(0);
    board_grid_sizer->Add(m_board_square.square_panel, 1, wxEXPAND | wxALL, 0);
  }
  this->SetSizer(board_grid_sizer);

  // synchronize board state with ui
  for (int iter = 0; iter < 64; ++iter) {
    m_board_squares[iter].piece_type = this->m_game_system->identify_piece(iter);
  }
  board_grid_sizer->Layout();

  // Bind drawing events to appropriate handlers
  Bind(wxEVT_SIZE, &ui::board_panel::on_boand_panel_resize, this);
  Bind(wxEVT_ERASE_BACKGROUND, &ui::board_panel::clear_background, this);
  Bind(wxEVT_PAINT, &ui::board_panel::paint_event_handler, this);
}

void ui::board_panel::load_images()
{
  // load chessboard image
  wxString message("Loading board image in directory ");
  wxImage::AddHandler(new wxPNGHandler);
  wxFileName img_path = wxFileName::DirName(ATHENA_BINARY_DIR);
  img_path.AppendDir("data");
  img_path.SetName("chessboard");
  img_path.SetExt("png");
  this->m_board_image = new wxImage();
  if (!this->m_board_image->LoadFile(
          img_path.GetFullPath(), wxBITMAP_TYPE_PNG, -1))
  {
    wxMessageBox("Unable to load file");
  }

  // Load piece images
  std::array<std::string, 6> general_piece_names = {
      "pawn", "rook", "bishop", "knight", "queen", "king"};
  std::array<std::string, 2> colors = {"w", "b"};
  auto image_iter = m_piece_images.begin();
  std::string filename;
  wxFileName temp_filename;
  for (auto & general_piece_name : general_piece_names)
  {
    for (auto & color : colors) {
      filename = std::string(color);
      filename.append("_");
      filename.append(general_piece_name);
      temp_filename = wxFileName::DirName(ATHENA_BINARY_DIR);
      temp_filename.AppendDir("data");
      temp_filename.SetName(filename);
      temp_filename.SetExt("png");
      image_iter->filename = filename;
      image_iter->image = new wxImage();
      image_iter->image->LoadFile(
          temp_filename.GetFullPath(), wxBITMAP_TYPE_PNG, -1);

      if (general_piece_name == "pawn") {
        if (color == "w") {
          image_iter->piece_type = core::white_pawn;
        } else {
          image_iter->piece_type = core::black_pawn;
        }
      } else if (general_piece_name == "rook") {
        if (color == "w") {
          image_iter->piece_type = core::white_rook;
        } else {
          image_iter->piece_type = core::black_rook;
        }
      } else if (general_piece_name == "bishop") {
        if (color == "w") {
          image_iter->piece_type = core::white_bishop;
        } else {
          image_iter->piece_type = core::black_bishop;
        }
      } else if (general_piece_name == "knight") {
        if (color == "w") {
          image_iter->piece_type = core::white_knight;
        } else {
          image_iter->piece_type = core::black_knight;
        }
      } else if (general_piece_name == "queen") {
        if (color == "w") {
          image_iter->piece_type = core::white_queen;
        } else {
          image_iter->piece_type = core::black_queen;
        }
      } else if (general_piece_name == "king") {
        if (color == "w") {
          image_iter->piece_type = core::white_king;
        } else {
          image_iter->piece_type = core::black_king;
        }
      } else {
        image_iter->piece_type = core::empty_square;
        }
      ++image_iter;
    }
  }
}

void ui::board_panel::on_boand_panel_resize(wxSizeEvent& event)
{
  this->Refresh();
  for (auto & m_board_square : m_board_squares) {
    m_board_square.square_panel->Refresh();
  }
  event.Skip();
}

void ui::board_panel::render(wxDC& dc)
{
  auto width = dc.GetSize().GetWidth();
  auto height = dc.GetSize().GetHeight();
  auto min_size = (width < height) ? width : height;

  // Draw the board
  wxImage temp_image = this->m_board_image->Scale(min_size, min_size);
  wxBitmap temp_bitmap(temp_image);
  dc.DrawBitmap(temp_bitmap, 0, 0);

  // Draw the pieces
  wxImage temp_piece_image;
  wxBitmap temp_piece_bitmap;
  core::piece_t piece_type;
  int indx;
  for (int iter = 0; iter < 64; ++iter) {
    piece_type = this->m_game_system->identify_piece(iter);
    // Don't bother drawing empty squares
    if (piece_type == core::empty_square) { continue;}
    for (indx = 0; indx < 12; ++indx) {
      if (m_piece_images[indx].piece_type == piece_type) { break; }
    }
    temp_piece_image = m_piece_images[indx].image->Scale(min_size / 8, min_size / 8);
    temp_piece_bitmap = wxBitmap(temp_piece_image);
    wxClientDC square_dc(m_board_squares[iter].square_panel);
    square_dc.DrawBitmap(temp_piece_bitmap, 2, 2);
  }
}

void ui::board_panel::paint_event_handler(wxPaintEvent&  /*event*/)
{
  wxPaintDC dc(this);
  render(dc);
}

void ui::board_panel::paint_now()
{
  wxClientDC dc(this);
  this->render(dc);
}

void ui::board_panel::clear_background(wxEraseEvent& event)
{
  wxDC* dc = event.GetDC();
  dc->Clear();
  event.Skip();
}

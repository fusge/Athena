#ifndef main_window_h
#define main_window_h

#include <array>
#include <string>
#include <memory>

#include <wx/wx.h>
#include <wx/generic/grid.h>
#include <wx/filename.h>

#include "chessgame.h"

namespace ui
{
  constexpr int default_width = 600;
  constexpr int default_height = 600;
  struct image_info{
    core::piece_t piece_type;
    wxFileName filename;
    wxImage *image;
  };

  struct board_square{
    core::piece_t piece_type;
    wxPanel *square_panel;
  };

  class athena : public wxApp
  {
  public:
     bool OnInit() override;
  };

  class board_panel : public wxPanel
  {
  public:
    board_panel(wxWindow * win, wxWindowID id);
    
    void load_images();
    void render(wxDC &dc);
    void paint_now(); 

    // Event Handler functions
    void paint_event_handler(wxPaintEvent &event);
    void clear_background(wxEraseEvent &event);
    void on_boand_panel_resize(wxSizeEvent &event);

    //std::array<std::string *, 12> filenames;

  private:
    // Piece and board images
    wxImage* m_board_image;
    std::array<image_info, 12> m_piece_images;
    std::array<ui::board_square, 64> m_board_squares;

    // Game core System
    std::unique_ptr<core::chessgame> m_game_system;
  };

  class main_window : public wxFrame
  {
  public:
    main_window();
    
  private:
    // we have to manage window ids
    wxWindowID m_board_panel_id;

    // window parts
    wxPanel* m_window_panel;
    ui::board_panel* m_board_panel;

    void on_exit(wxCommandEvent& event);
    void on_about(wxCommandEvent& event);
  };
} // namespace ui

wxDECLARE_APP(ui::athena);
#endif /* main_window_h */

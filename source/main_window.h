#ifndef main_window_h
#define main_window_h

#include <array>
#include <string>
#include <memory>

#include <wx/wx.h>
#include <wx/generic/grid.h>
#include <wx/filename.h>

#include "chessgame.h"

namespace UI
{
    struct imageInfo{
        Core::Piece_t piece_type;
        wxFileName filename;
        wxImage *image;
    };

    struct boardSquare{
        Core::Piece_t piece_type;
        wxPanel *square_panel;
    };

    class Athena : public wxApp
    {
    public:
        virtual bool OnInit();
    };

    class BoardPanel : public wxPanel
    {
    public:
        BoardPanel(wxWindow * win, wxWindowID id);
        
        void loadImages();
        void render(wxDC &dc);
        void paintNow(); 

        // Event Handler functions
        void paintEventHandler(wxPaintEvent &event);
        void clearBackground(wxEraseEvent &event);
        void onBoardPanelResize(wxSizeEvent &event);

        //std::array<std::string *, 12> filenames;

    private:
        // Piece and board images
        wxImage* board_image;
        std::array<imageInfo, 12> piece_images;
        std::array<UI::boardSquare, 64> board_squares;

        // Game Core System
        std::unique_ptr<Core::Chessgame> game_system;
    };

    class MainWindow : public wxFrame
    {
    public:
        MainWindow();
        
    private:
        // we have to manage window ids
        wxWindowID board_panel_id;

        // window parts
        wxPanel* window_panel;
        UI::BoardPanel* board_panel;

        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
    };
}

wxDECLARE_APP(UI::Athena);
#endif /* main_window_h */

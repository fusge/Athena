#ifndef main_window_h
#define main_window_h

#include <array>
#include <string>

#include <wx/wx.h>
#include <wx/generic/grid.h>
#include <wx/filename.h>

#include "chessgame.h"

namespace UI
{

    struct imageInfo{
        std::string name;
        std::string color;
        wxFileName filename;
        wxImage *image;
    };

    class Athena : public wxApp
    {
    public:
        virtual bool OnInit();
    };

    class cpBoardPanel : public wxPanel
    {
    public:
        cpBoardPanel(wxWindow * win, wxWindowID id): wxPanel(win, id)
        {
            this->loadImages(); 
            Bind(wxEVT_SIZE, &UI::cpBoardPanel::onBoardPanelResize, this);
            Bind(wxEVT_ERASE_BACKGROUND, &UI::cpBoardPanel::clearBackground, this);
            Bind(wxEVT_PAINT, &UI::cpBoardPanel::paintEventHandler, this);
        }
        
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
    };

    class cpMainWindow : public wxFrame
    {
    public:
        cpMainWindow();
        
        void loadImages();
       
    private:
        // we have to manage window ids
        wxWindowID board_panel_id;

        // window parts
        wxPanel* window_panel;
        UI::cpBoardPanel* board_panel;

        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
    };
}

wxIMPLEMENT_APP(UI::Athena);
#endif /* main_window_h */

#ifndef main_window_h
#define main_window_h

//#ifdef _WIN32
//#define WXUSINGDLL
//#endif

#include "wx/wx.h"
#include "wx/generic/grid.h"

namespace UI
{
    class Chesspeer : public wxApp
    {
    public:
        virtual bool OnInit();
    };

    class cpMainWindow : public wxFrame
    {
    public:
        cpMainWindow();
        
        void loadImages();
        

    private:
        // we have to manage window ids
        wxWindowID board_panel_id;

        // Piece and board images
        std::array<wxImage, 12>* piece_images;
        wxImage* board_image;

        // window parts
        wxPanel* board_panel;

        void OnHello(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
    };

}

wxIMPLEMENT_APP(UI::Chesspeer);
#endif /* main_window_h */

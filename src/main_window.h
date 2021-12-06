#ifndef main_window_h
#define main_window_h

//#ifdef _WIN32
//#define WXUSINGDLL
//#endif

#include "wx/wx.h"
#include "wx/generic/grid.h"

namespace UI
{
    enum
    {
        ID_Hello = 1,
        ID_BoardResize = 2
    };

    class Chesspeer : public wxApp
    {
    public:
        virtual bool OnInit();
    };

    class cpBoardPanel : public wxPanel
    {
    public:
        cpBoardPanel(wxWindow * win, wxWindowID id): wxPanel(win, id)
        {
            this->loadBoardImage(); 
            Bind(wxEVT_SIZE, &UI::cpBoardPanel::onBoardPanelResize, this);
            Bind(wxEVT_ERASE_BACKGROUND, &UI::cpBoardPanel::clearBackground, this);
            Bind(wxEVT_PAINT, &UI::cpBoardPanel::paintEventHandler, this);
        }
        
        void loadBoardImage();
        void render(wxDC &dc);
        void paintNow();

        // Event Handler functions
        void paintEventHandler(wxPaintEvent &event);
        void clearBackground(wxEraseEvent &event);
        void onBoardPanelResize(wxSizeEvent &event);

    private:
        wxImage* board_image;
        wxBitmap* cpBoardBitmap;
    };

    class cpMainWindow : public wxFrame
    {
    public:
        cpMainWindow();
        
        void loadImages();
       
        // Initialize event system

    private:
        // we have to manage window ids
        wxWindowID board_panel_id;

        // Piece and board images
        std::array<wxImage, 12>* piece_images;

        // window parts
        wxPanel* window_panel;
        UI::cpBoardPanel* board_panel;

        void OnHello(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
    };
}

wxIMPLEMENT_APP(UI::Chesspeer);
#endif /* main_window_h */

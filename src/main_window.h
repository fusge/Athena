#ifndef main_window_h
#define main_window_h

//#ifdef _WIN32
//#define WXUSINGDLL
//#endif

#include "wx/wx.h"

namespace UI
{
    class MainWindow : public wxApp
    {
    public:
        virtual bool OnInit();
    };

    class cpFrame : public wxFrame
    {
    public:
        cpFrame();
    private:
        void OnHello(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
    };
}

wxIMPLEMENT_APP(UI::MainWindow);
#endif /* main_window_h */

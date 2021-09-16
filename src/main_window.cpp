#include "main_window.h"

enum
{
    ID_Hello = 1
};

bool UI::MainWindow::OnInit()
{
    UI::ChessBoard *frame = new UI::ChessBoard();
    frame->Show(true);
    return true;
}

UI::ChessBoard::ChessBoard()
    : wxFrame(NULL, wxID_ANY, "Hello World")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &UI::ChessBoard::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &UI::ChessBoard::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &UI::ChessBoard::OnExit, this, wxID_EXIT);
}
void UI::ChessBoard::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void UI::ChessBoard::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void UI::ChessBoard::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

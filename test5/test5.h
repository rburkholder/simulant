#pragma once

class AppTest5: public wxApp {
public:
protected:
private:

  wxFrame *frame;

  virtual bool OnInit( );
  virtual int OnExit( );
  void OnClose( wxCloseEvent& event );
};

// Implements MyApp& wxGetApp()
DECLARE_APP( AppTest5 )


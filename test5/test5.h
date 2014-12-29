/*
  Copywrite (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

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


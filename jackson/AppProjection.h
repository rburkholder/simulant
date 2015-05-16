/* 
 * File:   jackson.h
 * Author: rpb
 *
 * Created on February 28, 2015, 7:38 PM
 */

#pragma once

#include <wx/wx.h>

#include "FrameMain.h"

class AppProjection : public wxApp {
public:

protected:
private:
  
  wxString m_sMediaDirectory;
  
  FrameMain *m_pFrameMain;
  PanelSurfaceSources* m_pSurfaceSources;
  
  virtual bool OnInit( );
  virtual void OnClose( wxCloseEvent& event );
  virtual int OnExit( void );
  
  //void HandleMouseMoved( wxMouseEvent& event ) { event.Skip(); } // EVT_MOTION
  //void HandleMouseWheel( wxMouseEvent& event ) { event.Skip(); } // EVT_MOUSEWHEEL
  //void HandleMouseLeftDown( wxMouseEvent& event ) { event.Skip(); } // EVT_LEFT_DOWN
  //void HandleMouseLeftUp( wxMouseEvent& event ) { event.Skip(); }  // EVT_LEFT_UP
  //void HandleMouseRightDown( wxMouseEvent& event ) { event.Skip(); } // EVT_RIGHT_DOWN
//  void HandleMouseLeaveWindow( wxMouseEvent& event ) { event.Skip(); } // EVT_LEAVE_WINDOW
  //void HandleKeyDown( wxKeyEvent& event ) { event.Skip(); } // EVT_KEY_DOWN
  //void HandleKeyUp( wxKeyEvent& event ) { event.Skip(); }  // EVT_KEY_UP
  void HandleKey( wxKeyEvent& event );
  
  void MediaFileStats( void );
  
};


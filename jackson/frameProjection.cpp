/* 
 * File:   frameProjection.cpp
 * Author: rpb
 * 
 * Created on March 8, 2015, 12:59 PM
 */

#include "frameProjection.h"

FrameProjection::FrameProjection(void) {
  Init();
}

FrameProjection::FrameProjection( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, caption, pos, size, style );
}

FrameProjection::~FrameProjection(void) {
}

bool FrameProjection::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
  SetBackgroundColour( wxColour( 0, 0, 0 ) );
  return true;
}

void FrameProjection::Init() {
}

void FrameProjection::CreateControls( void ) {

    FrameProjection* itemFrame1 = this;

   
//    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );
//    Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
}
wxBitmap FrameProjection::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameProjection::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}


/* 
 * File:   KeyFrameDiamond.cpp
 * Author: rpb
 * 
 * Created on May 31, 2015, 9:32 PM
 */

//#include <wx/sizer.h>
#include <wx/icon.h>
#include <wx/imagpng.h>

#include "KeyFrameDiamond.h"

IMPLEMENT_DYNAMIC_CLASS( KeyFrameDiamond, wxWindow )

KeyFrameDiamond::KeyFrameDiamond( ) {
  Init();
}

KeyFrameDiamond::KeyFrameDiamond( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void KeyFrameDiamond::Init() {
}

bool KeyFrameDiamond::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  //SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxWindow::Create( parent, id, pos, size, style );
  
  assert( m_image.LoadFile( "diamond.png", wxBITMAP_TYPE_PNG ) );

  CreateControls();
  //if (GetSizer()) {
  //  GetSizer()->SetSizeHints(this);
  //}
  //Centre();
  
  return true;
}

void KeyFrameDiamond::CreateControls() {
  //Bind( wxEVT_PAINT, &WaveformView::HandlePaint, this );
  //Bind( wxEVT_ERASE_BACKGROUND, &WaveformView::HandleEraseBackground, this );
  //Bind( wxEVT_SIZE, &WaveformView::HandleSize, this );
  //Bind( wxEVT_SIZING, &WaveformView::HandleSizing, this );
  //Bind( wxEVT_LEFT_DOWN, &WaveformView::HandleMouseLeftDown, this );
  //Bind( wxEVT_LEFT_UP, &WaveformView::HandleMouseLeftUp, this );
  //Bind( wxEVT_MOUSEWHEEL, &WaveformView::HandleMouseWheel, this );
  //Bind( wxEVT_MOTION, &WaveformView::HandleMouseMotion, this );
  //Bind( wxEVT_LEAVE_WINDOW, &WaveformView::HandleLeaveWindow, this );
  //Bind( wxEVT_IDLE, &WaveformView::HandleIdle, this );
  //Bind( wxEVT_COMMAND_ENTER, &WaveformView::HandlePlayCursor, this, ID_EVENT_PLAYCURSOR );
}

KeyFrameDiamond::~KeyFrameDiamond( ) {
}

wxBitmap KeyFrameDiamond::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon KeyFrameDiamond::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

/* 
 * File:   KeyFrameView.cpp
 * Author: rpb
 * 
 * Created on June 8, 2015, 10:37 PM
 */

#include <wx/wx.h>

#include "KeyFrameView.h"

IMPLEMENT_DYNAMIC_CLASS( KeyFrameView, wxPanel )

KeyFrameView::KeyFrameView( ) {
  Init();
}

KeyFrameView::KeyFrameView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void KeyFrameView::Init() {
}

bool KeyFrameView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  
  // this code snippet should be in a method
  //wxRect rect( this->GetClientRect() );
  //SummarizeSamples( rect.GetWidth(), m_ixFirstSampleInWindow, m_nSamplesInWindow );
  
  return true;
}

void KeyFrameView::CreateControls() {
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

KeyFrameView::~KeyFrameView( ) {
}

wxBitmap KeyFrameView::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon KeyFrameView::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

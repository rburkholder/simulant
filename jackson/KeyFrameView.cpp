/* 
 * File:   KeyFrameView.cpp
 * Author: rpb
 * 
 * Created on June 8, 2015, 10:37 PM
 */

#include <wx/wx.h>

#include "KeyFrameView.h"

IMPLEMENT_DYNAMIC_CLASS( KeyFrameView, SceneViewCommon )

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
  SceneViewCommon::Create( parent, id, pos, size, style );

  std::cout << "KeyFrameView colour" << std::endl;
  m_colourBackground = wxColour( 100, 100, 100 );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  //Centre();
  
  // this code snippet should be in a method
  //wxRect rect( this->GetClientRect() );
  //SummarizeSamples( rect.GetWidth(), m_ixFirstSampleInWindow, m_nSamplesInWindow );
  
  return true;
}

void KeyFrameView::CreateControls() {
  Bind( wxEVT_PAINT, &KeyFrameView::HandlePaint, this );
  Bind( wxEVT_ERASE_BACKGROUND, &KeyFrameView::HandleEraseBackground, this );
  Bind( wxEVT_SIZE, &KeyFrameView::HandleSize, this );
  Bind( wxEVT_RIGHT_UP, &KeyFrameView::HandleMouseRightUp, this );
  Bind( wxEVT_LEFT_UP, &KeyFrameView::HandleMouseLeftUp, this );
  //Bind( wxEVT_SIZING, &WaveformView::HandleSizing, this );
  //Bind( wxEVT_LEFT_DOWN, &WaveformView::HandleMouseLeftDown, this );
  //Bind( wxEVT_MOUSEWHEEL, &WaveformView::HandleMouseWheel, this );
  //Bind( wxEVT_MOTION, &KeyFrameView::HandleMouseMotion, this );
  //Bind( wxEVT_LEAVE_WINDOW, &WaveformView::HandleLeaveWindow, this );
  //Bind( wxEVT_IDLE, &WaveformView::HandleIdle, this );
  //Bind( wxEVT_COMMAND_ENTER, &WaveformView::HandlePlayCursor, this, ID_EVENT_PLAYCURSOR );
  
  // may need to be more dynamic, figure out which are needed
  m_pContextMenu = new wxMenu();
  m_pContextMenu->Append( MIAddKeyFrame, "Add KeyFrame" );
  m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleAddKeyFrame, this, MIAddKeyFrame );
  m_pContextMenu->Append( MIDeleteKeyFrame, "Delete KeyFrame" );
  m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleDeleteKeyFrame, this, MIDeleteKeyFrame );
  m_pContextMenu->Append( MIEditKeyFrame, "Edit KeyFrame" );
  m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleEditKeyFrame, this, MIEditKeyFrame );
  m_pContextMenu->Append( MICopy, "Copy Settings" );
  m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleCopy, this, MICopy );
  m_pContextMenu->Append( MIPaste, "Paste Settings" );
  m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandlePaste, this, MIPaste );
  
}

KeyFrameView::~KeyFrameView( ) {
}

void KeyFrameView::HandleEraseBackground( wxEraseEvent& event ) {
  event.Skip();
}

void KeyFrameView::HandlePaint( wxPaintEvent& event ) {

  wxPaintDC dc(this);
  wxRect rectClientArea( this->GetClientRect() );
  int width( rectClientArea.GetWidth() );
  int height( rectClientArea.GetHeight() );
  wxBrush brush( dc.GetBrush() );
  brush.SetColour( m_colourBackground );
  dc.SetBrush( brush );
  dc.DrawRectangle( rectClientArea );  // blank out background

}

void KeyFrameView::HandleSize( wxSizeEvent& event ) {
  //std::cout << "sized" << std::endl;
  Refresh();
  event.Skip();
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

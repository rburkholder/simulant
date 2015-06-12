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

  std::cout << "KeyFrameView colour" << std::endl;
  m_colourBackground = wxColour( 100, 100, 100 );

}

bool KeyFrameView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  SceneViewCommon::Create( parent, id, pos, size, style );

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

void KeyFrameView::UnDrawCursor( Cursor& cursor ) {

  wxRect rectClientArea( this->GetClientRect() );
  int yMax( rectClientArea.height - 1 );
  wxClientDC dc( this );
  wxPen pen( cursor.m_colourCursor, 1, wxPENSTYLE_SOLID );
  dc.SetPen( pen );

  // redraw data that was under the cursor
  if ( cursor.m_bCursorDrawn ) {

    // draw vertical line with background colour
    wxBrush brush( dc.GetBrush() );
    brush.SetColour( m_colourBackground );
    dc.SetBrush( brush );
    wxPen pen( dc.GetPen() );
    pen.SetColour( m_colourBackground );
    dc.SetPen( pen );
    dc.DrawLine( cursor.m_locCursor, 0, cursor.m_locCursor, yMax );

    cursor.m_bCursorDrawn = false;
  }
}

void KeyFrameView::HandleMouseRightUp( wxMouseEvent& event ) {
  m_pointLatestMouse = event.GetPosition();
  this->PopupMenu( m_pContextMenu );
  event.Skip();
}

void KeyFrameView::HandleMouseLeftUp( wxMouseEvent& event ) { // click to activate a keyframe (how to select one?)
  m_pointLatestMouse = event.GetPosition();
  // do some pre-filter first?
  m_signalMouseEventSelectKeyFrame( m_pointLatestMouse );
  event.Skip();
}

//void KeyFrameView::HandleMouseMotion( wxMouseEvent& event ) { // move the cursor, signal back to scene to update other windows
//  // common code from wave form then?
//  m_pointLatestMouse = event.GetPosition();
//  m_signalMouseEventMovement( m_pointLatestMouse );
//}

void KeyFrameView::HandleAddKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventAddKeyFrame( m_pointLatestMouse );
}

void KeyFrameView::HandleEditKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventEditKeyFrame( m_pointLatestMouse );
}

void KeyFrameView::HandleDeleteKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
}

void KeyFrameView::HandleCopy( wxCommandEvent& event ) {
  //m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
}

void KeyFrameView::HandlePaste( wxCommandEvent& event ) {
  //m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
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

/* 
 * File:   frameProjection.cpp
 * Author: rpb
 * 
 * Created on March 8, 2015, 12:59 PM
 */

#include "FrameProjection.h"

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
  
  m_bMouseDown = false;
  m_pOutline = 0;
  
  // need to handle on paint event, for drawing the placement outline
  wxFrame::Bind( wxEVT_PAINT, &FrameProjection::HandlePaint, this );
  wxFrame::Bind( wxEVT_ERASE_BACKGROUND, &FrameProjection::HandleEraseBackground, this );
  wxFrame::Bind( wxEVT_SIZE, &FrameProjection::HandleSize, this );  // window specific, not in app
  wxFrame::Bind( wxEVT_MOTION, &FrameProjection::HandleMouseMoved, this );
  wxFrame::Bind( wxEVT_LEFT_DOWN, &FrameProjection::HandleMouseLeftDown, this );
  wxFrame::Bind( wxEVT_LEFT_UP, &FrameProjection::HandleMouseLeftUp, this );
  //wxFrame::Bind( wxEVT_CHAR, &FrameProjection::HandleKey, this ); // doesn't work here, needs some sort of object attached
  wxFrame::Bind( wxEVT_KEY_DOWN, &FrameProjection::HandleKey, this ); 
  //wxFrame::Bind( wxEVT_KEY_UP, &FrameProjection::HandleKey, this );
}

void FrameProjection::CreateControls( void ) {

    FrameProjection* itemFrame1 = this;

   
//    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );
//    Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
}

void FrameProjection::HandleKey( wxKeyEvent& event ) {
  //if ( event.GetKeyCode() = )
  std::cout << event.GetKeyCode() << std::endl;
}

void FrameProjection::HandleMouseMoved( wxMouseEvent& event ) {
  if ( 0 != m_pOutline ) {
    wxClientDC dc( this );
    wxCursor cursor = m_pOutline->TrackMouse( event.GetPosition(), m_bMouseDown, dc );
    this->SetCursor( cursor );
  }
  event.Skip();
}

void FrameProjection::HandleMouseLeftDown( wxMouseEvent& event ) {
  m_bMouseDown = true;
  event.Skip();
}

void FrameProjection::HandleMouseLeftUp( wxMouseEvent& event ) {
  m_bMouseDown = false;
  this->SetCursor( *wxSTANDARD_CURSOR );
  this->Refresh();
  event.Skip();
}

void FrameProjection::HandleEraseBackground( wxEraseEvent& event ) {
//  wxPaintDC dc( this );
 // if ( 0 != m_pOutline ) m_pOutline->Erase( dc );
  event.Skip();
}

void FrameProjection::HandlePaint( wxPaintEvent& event ) {
  wxPaintDC dc( this );
  Render( dc );
  if ( 0 != m_pOutline ) m_pOutline->Render( dc );
}

void FrameProjection::Render( wxDC& dc ) { // actual DC independent render
//  dc.SetPen( wxPen( wxColor( 255,255,255), 2 ) );
//  dc.DrawLine( 100, 100, 200, 200 );
}

void FrameProjection::HandleSize( wxSizeEvent& event ) {
  this->Refresh();
}  

wxBitmap FrameProjection::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameProjection::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}


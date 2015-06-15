/* 
* File:   SceneViewCommon.cpp
* Author: rpb
*
* Created on June 8, 2015, 10:37 PM
*/

#include <boost/date_time/posix_time/posix_time.hpp>

#include <wx/wx.h>

#include "SceneViewCommon.h"

IMPLEMENT_DYNAMIC_CLASS( SceneViewCommon, wxPanel )

SceneViewCommon::SceneViewCommon( ) {
  Init();
}

SceneViewCommon::SceneViewCommon( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void SceneViewCommon::Init() {

  m_pvSamples = 0;

  //m_bMouseLeftDown = false;
  //m_ixFirstSampleInWindow = 0;
  //m_nSamplesInWindow = 0;
  std::cout << "SceneViewCommon colour" << std::endl;
  m_colourBackground = wxColour( 0, 0, 0 );

  m_colourName = wxColour( 255, 255, 255 );
  m_pointName = wxPoint( 2, 2 );

  m_cursorInteractive.m_colourCursor = wxColour( 218,112,214 );
  m_cursorInteractive.m_pointStatusText = wxPoint( 120, 2 );

  m_cursorPlay.m_colourCursor = wxColour( 0, 255, 0 );
  m_cursorPlay.m_pointStatusText = wxPoint( 22, 2 );

}

bool SceneViewCommon::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

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

void SceneViewCommon::CreateControls() {
  //Bind( wxEVT_ERASE_BACKGROUND, &SceneViewCommon::HandleEraseBackground, this );
  //Bind( wxEVT_PAINT, &SceneViewCommon::HandlePaint, this );
  //Bind( wxEVT_SIZE, &SceneViewCommon::HandleSize, this );
  //Bind( wxEVT_SIZING, &SceneViewCommon::HandleSizing, this );
  Bind( wxEVT_LEFT_DOWN, &SceneViewCommon::HandleMouseLeftDown, this );
  //Bind( wxEVT_LEFT_UP, &WaveformView::HandleMouseLeftUp, this );
  Bind( wxEVT_MOUSEWHEEL, &SceneViewCommon::HandleMouseWheel, this );
  Bind( wxEVT_MOTION, &SceneViewCommon::HandleMouseMotion, this );
  //Bind( wxEVT_LEAVE_WINDOW, &SceneViewCommon::HandleLeaveWindow, this );
  //Bind( wxEVT_IDLE, &WaveformView::HandleIdle, this );
  Bind( wxEVT_LEAVE_WINDOW, &SceneViewCommon::HandleLeaveWindow, this );
  //Bind( wxEVT_COMMAND_ENTER, &SceneViewCommon::HandlePlayCursor, this, ID_EVENT_PLAYCURSOR );
}

SceneViewCommon::~SceneViewCommon( ) {
}

void SceneViewCommon::SetName( const std::string& sName ) { 
  m_sName = sName; 
}

void SceneViewCommon::DrawName( wxClientDC& dc ) {

  wxSize sizeText = dc.GetTextExtent( m_sName );

  wxBrush brush( dc.GetBrush() );
  brush.SetColour( m_colourBackground );
  dc.SetBrush( brush );

  wxPen pen( dc.GetPen() );
  pen.SetColour( m_colourBackground );
  dc.SetPen( pen );

  dc.DrawRectangle( m_pointName, sizeText );

  dc.SetTextBackground( m_colourBackground );
  dc.SetTextForeground( m_colourName );
  dc.DrawText( m_sName, m_pointName );
}

void SceneViewCommon::HandleEraseBackground( wxEraseEvent& event ) {
  event.Skip();
}

void SceneViewCommon::HandlePaint( wxPaintEvent& event ) {

  wxPaintDC dc(this);
  wxRect rectClientArea( this->GetClientRect() );
  int width( rectClientArea.GetWidth() );
  int height( rectClientArea.GetHeight() );
  wxBrush brush( dc.GetBrush() );
  brush.SetColour( m_colourBackground );
  dc.SetBrush( brush );
  dc.DrawRectangle( rectClientArea );  // blank out background
  DrawName( dc );

}

void SceneViewCommon::HandleSize( wxSizeEvent& event ) {
  //std::cout << "sized" << std::endl;
  Refresh();
  event.Skip();
}

void SceneViewCommon::HandleMouseWheel( wxMouseEvent& event ) {
  wxPoint posMouse = event.GetPosition();
  //std::cout << "0 wheel: " << event.GetWheelRotation() << "," << event.GetWheelDelta()
  //  << "," << event.GetLinesPerAction() << "," << posMouse.x << "," << posMouse.y 
  //  << std::endl;
  
  if ( event.GetWheelRotation() > 0 ) {
    m_signalZoomIn( posMouse.x );
  }
  else {
    m_signalZoomOut( posMouse.x );
  }
  
  event.Skip();
}

void SceneViewCommon::HandleMouseLeftDown( wxMouseEvent& event ) {
  m_posMouseOnLeftDown = event.GetPosition();
  event.Skip();
}

void SceneViewCommon::HandleMouseMotion( wxMouseEvent& event ) {
  wxPoint point = event.GetPosition();
  //std::cout << "0 motion: " << point.x << "," << point.y << "," << event.Dragging() << std::endl;
  int x = point.x;
  // UnDrawCursor?
  if (x != m_posMouseOnLeftDown.x) {
    int diff = x - m_posMouseOnLeftDown.x;
    if ( event.Dragging() ) {
      //std::cout << "shift " << diff << std::endl;
      m_signalMouseShift( diff );
    }
    else {
      m_signalMouseMotion( x, diff );
    }
    m_posMouseOnLeftDown = point;
  }
  event.Skip();
}

void SceneViewCommon::HandleLeaveWindow( wxMouseEvent& event ) {
  //std::cout << "wfv leave window" << std::endl;
  UnDrawCursor( m_cursorInteractive );
  SceneViewCommon::DrawCursor( -1, m_cursorInteractive );  
  EraseTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText );
  event.Skip();
}

void SceneViewCommon::UnDrawCursor( Cursor& cursor ) {

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
    // apply background colour top to bottom where cursor was
    dc.DrawLine( cursor.m_locCursor, 0, cursor.m_locCursor, yMax );

    cursor.m_bCursorDrawn = false;
  }
}

void SceneViewCommon::DrawCursor( int ix, Cursor& cursor ) {

  wxRect rectClientArea( this->GetClientRect() );
  int yMax( rectClientArea.height - 1 );
  wxClientDC dc( this );
  wxPen pen( cursor.m_colourCursor, 1, wxPENSTYLE_SOLID );
  dc.SetPen( pen );

  // draw the cursor in the new spot
  if ( 0 <= ix ) {
    pen.SetColour( cursor.m_colourCursor );
    dc.SetPen( pen );
    dc.DrawLine( ix, 0, ix, rectClientArea.height - 1 );
    cursor.m_locCursor = ix;
    cursor.m_bCursorDrawn = true;
  }
}

const std::string SceneViewCommon::TimeAtSample( size_t nSample, size_t numerator, size_t denominator ) {

  // *** note: there is a problem with the numerator in the remainder part
  // *** this needs to be fixed before fully generalized
  assert( 1 == numerator );

  // generalize this for generic cursor management and keyframe management
  size_t nSeconds = ( numerator * nSample ) / denominator;
  size_t microseconds = ( 1000000 * ( nSample % denominator ) ) / denominator;
  boost::posix_time::time_duration time( 0, 0, nSeconds );
  time += boost::posix_time::microseconds( microseconds );
  //std::cout << "samples: " << nSamples << "," << remainder << "," << ms << std::endl;

  std::stringstream ss;
  ss << time;
  std::string s( ss.str() );

  return s;
}

void SceneViewCommon::DrawTime( Cursor& cursor, wxPoint& point, const std::string& sTime ) {

  wxClientDC dc( this );
  wxBrush brush( dc.GetBrush() );
  wxPen pen( dc.GetPen() );

  wxSize sizeText = dc.GetTextExtent( sTime );

  brush.SetColour( m_colourBackground );
  dc.SetBrush( brush );
  pen.SetColour( m_colourBackground );
  dc.SetPen( pen );
  dc.DrawRectangle( point, sizeText );
  dc.SetTextBackground( m_colourBackground );
  dc.SetTextForeground( cursor.m_colourCursor );
  dc.DrawText( sTime, point );

}

void SceneViewCommon::EraseTime( Cursor& cursor, wxPoint& point ) {
  static const std::string s( "00:00:00.000000");
  wxClientDC dc( this );
  wxSize size = dc.GetTextExtent( s );
  wxBrush brush( dc.GetBrush() );
  brush.SetColour( m_colourBackground );
  dc.SetBrush( brush );
  wxPen pen( dc.GetPen() );
  pen.SetColour( m_colourBackground );
  dc.SetPen( pen );
  dc.DrawRectangle( point, size );
}

void SceneViewCommon::UpdateInteractiveCursor( int x ) {
  UnDrawCursor( m_cursorInteractive );
  DrawCursor( x, m_cursorInteractive );
}

// gui thread
void SceneViewCommon::UpdatePlayCursor( size_t nFramesPlayed ) {

  if ( 0 != nFramesPlayed ) { // some frames played, so move/display cursor
    Cursor& cursor( m_cursorPlay );
    if ( cursor.m_ixFrame != nFramesPlayed ) {  // cursor has moved, so update
      vVertical_t::const_iterator iter = std::lower_bound( m_vVertical.begin(), m_vVertical.end(), nFramesPlayed );
      if ( m_vVertical.end() == iter ) { // error if can't find something
                                         // don't do anything, play may continue with nothing to play
      }
      else { // something found
        bool bCanDrawCursor( true );
        if ( iter->index > nFramesPlayed ) { // if found vertical has value greater than frame, need to back up
          if ( m_vVertical.begin() == iter ) {  // check if can back up
                                                // means cursor is off left end so don't draw anything
            bCanDrawCursor = false;
          }
          else {
            --iter; // backup
            assert( iter->index <= nFramesPlayed ); // and test that things are now fine
          }
        }
        if ( bCanDrawCursor ) {
          size_t n = iter - m_vVertical.begin(); // index into the vector for cursor calc
          if ( n != cursor.m_locCursor ) { // cursor location has changed 
            UnDrawCursor( m_cursorPlay );
            SceneViewCommon::DrawCursor( n, m_cursorPlay );
            cursor.m_ixFrame = nFramesPlayed;
          }
        }
        wxPoint point( m_cursorPlay.m_pointStatusText );
        DrawTime( m_cursorPlay, point, TimeAtSample( nFramesPlayed, 1, 44100 ) );
        point.y += 13;
        size_t size( m_pvSamples->size() );
        if ( nFramesPlayed <= size ) {
          DrawTime( m_cursorPlay, point, TimeAtSample( size - nFramesPlayed, 1, 44100 ) );
        }
        else {
          EraseTime( m_cursorPlay, point );
        }
      }
    }
  }
}

void SceneViewCommon::HandleMouseRightUp( wxMouseEvent& event ) {
  m_pointLatestMouse = event.GetPosition();
  this->PopupMenu( m_pContextMenu );
  event.Skip();
}

void SceneViewCommon::HandleMouseLeftUp( wxMouseEvent& event ) { // click to activate a keyframe (how to select one?)
  m_pointLatestMouse = event.GetPosition();
  // do some pre-filter first?
  m_signalMouseEventSelectKeyFrame( m_pointLatestMouse );
  event.Skip();
}

void SceneViewCommon::HandleAddKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventAddKeyFrame( m_pointLatestMouse );
}

void SceneViewCommon::HandleEditKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventEditKeyFrame( m_pointLatestMouse );
}

void SceneViewCommon::HandleDeleteKeyFrame( wxCommandEvent& event ) {
  m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
}

void SceneViewCommon::HandleCopy( wxCommandEvent& event ) {
  //m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
}

void SceneViewCommon::HandlePaste( wxCommandEvent& event ) {
  //m_signalMouseEventDeleteKeyFrame( m_pointLatestMouse );
}

wxBitmap SceneViewCommon::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon SceneViewCommon::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

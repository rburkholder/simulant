/* 
 * File:   SceneView.cpp
 * Author: rpb
 * 
 * Created on June 15, 2015, 10:28 AM
 */

#include <math.h>

#include <wx/wx.h>

#include "SceneMgmtView.h"

IMPLEMENT_DYNAMIC_CLASS( SceneMgmtView, SceneViewCommon )

SceneMgmtView::SceneMgmtView( ) {
  Init();
}

SceneMgmtView::SceneMgmtView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void SceneMgmtView::Init() {
}

bool SceneMgmtView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  SceneViewCommon::Create( parent, id, pos, size, style );

  //std::cout << "SceneView colour" << std::endl;
  m_colourBackground = wxColour( 200, 200, 200 );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  
  return true;
}

SceneMgmtView::~SceneMgmtView( ) {
}

void SceneMgmtView::CreateControls() {
  Bind( wxEVT_PAINT, &SceneMgmtView::HandlePaint, this );   // from SceneViewCommon
  Bind( wxEVT_ERASE_BACKGROUND, &SceneMgmtView::HandleEraseBackground, this );   // from SceneViewCommon
  Bind( wxEVT_SIZE, &SceneMgmtView::HandleSize, this );   // from SceneViewCommon
  Bind( wxEVT_RIGHT_UP, &SceneMgmtView::HandleMouseRightUp, this );   // from SceneViewCommon
  Bind( wxEVT_LEFT_UP, &SceneMgmtView::HandleMouseLeftUp, this );   // from SceneViewCommon
  //Bind( wxEVT_SIZING, &WaveformView::HandleSizing, this );
  //Bind( wxEVT_LEFT_DOWN, &WaveformView::HandleMouseLeftDown, this );
  //Bind( wxEVT_MOUSEWHEEL, &WaveformView::HandleMouseWheel, this );
  //Bind( wxEVT_MOTION, &KeyFrameView::HandleMouseMotion, this );
  //Bind( wxEVT_LEAVE_WINDOW, &WaveformView::HandleLeaveWindow, this );
  //Bind( wxEVT_IDLE, &WaveformView::HandleIdle, this );
  //Bind( wxEVT_COMMAND_ENTER, &WaveformView::HandlePlayCursor, this, ID_EVENT_PLAYCURSOR );
  
  // may need to be more dynamic, figure out which are needed
  m_pContextMenu = new wxMenu();
  //m_pContextMenu->Append( MIAddKeyFrame, "Add KeyFrame" );
  //m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleAddKeyFrame, this, MIAddKeyFrame );
  //m_pContextMenu->Append( MIDeleteKeyFrame, "Delete KeyFrame" );
  //m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleDeleteKeyFrame, this, MIDeleteKeyFrame );
  //m_pContextMenu->Append( MIEditKeyFrame, "Edit KeyFrame" );
  //m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleEditKeyFrame, this, MIEditKeyFrame );
  //m_pContextMenu->Append( MICopy, "Copy Settings" );
  //m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandleCopy, this, MICopy );
  //m_pContextMenu->Append( MIPaste, "Paste Settings" );
  //m_pContextMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &KeyFrameView::HandlePaste, this, MIPaste );
  
  
  wxClientDC dc( this );
  DrawLegend( dc );
}

// how often does this need to be called?
// maybe keep track of changes, and not process so often
void SceneMgmtView::HandlePaint( wxPaintEvent& event ) {  
  SceneViewCommon::HandlePaint( event );
  wxPaintDC dc(this);
  DrawLegend( dc );  // todo:  may only need this on mouse translation or scaling, plus on init
}

void SceneMgmtView::UpdateInteractiveCursor( int x ) {
  wxClientDC dc( this );
  //this->DrawName( dc );
  // change the following to  use dc at some point
  //UnDrawCursor(dc, m_cursorInteractive );
  //DrawCursor( dc, x, m_cursorInteractive );

  // need this in SceneView only
  //DrawTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText, TimeAtSample( x, 1, 44100 ) );
  
  boost::posix_time::time_duration td;
  td = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * x;
  
  std::stringstream ss;
  
  ss << td;
  std::string s( ss.str() );
  //SceneViewCommon::DrawTime( wxColour( 0, 0, 0 ), wxPoint( 2, 2 ), s );
  DrawTime( s );
          
}

void SceneMgmtView::UnDrawCursor( wxClientDC& dc, Cursor& cursor ) {
  SceneViewCommon::UnDrawCursor( dc, cursor );
  //DrawLegend( dc );
}

void SceneMgmtView::DrawTime( const std::string& sTime ) {
  wxClientDC dc( this );
  SceneViewCommon::DrawTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText, sTime );
}

void SceneMgmtView::DrawLegend( wxClientDC& dc ) {

  std::stringstream ss;
  
  boost::posix_time::time_input_facet *input_facet;
  input_facet = new boost::posix_time::time_input_facet();
  // fractional seconds are supposed to always show, but aren't
  input_facet->format( "%H:%M:%S.%f" );  // http://www.boost.org/doc/libs/1_60_0/doc/html/date_time/date_time_io.html
  ss.imbue( std::locale( ss.getloc(), input_facet ) );
  
  wxRect rect = GetClientRect();

  // text sizing calcs
  wxSize sizeText = dc.GetTextExtent( SceneViewCommon::sZeroTime + " " );
  int widthText = sizeText.GetWidth();
  int widthTextBy2 = widthText / 2;
  
  // play with drawing time ticks
  if ( ( 0 < widthText ) && ( 10 < rect.width ) ) {  // non zero width text and at least 10 pixel widths to draw
    int nTextExtents = rect.width / ( widthText );  // how many ticks can we draw
    if ( ( 0 < nTextExtents ) && ( 4 <= rect.height ) ) { // there are some extents to draw, and some height to draw them in

      // ticks are 10 pixels
      // prep tick locations, top of rect is 0,0, need to reference bottom of rectangle
      int bot = rect.height - 1;
      int top = bot - std::min<int>( 10, rect.height / 2 ); // max 10 pixels high

      // will need work on normalizing values, but at least get some sort of value in place for now
      // ticks start at 1/2 size of extent
      boost::posix_time::time_duration td;
      // 2 is for pixels to left border
      for ( int ix = widthTextBy2 + 2; ix < ( rect.width - widthTextBy2 + 2 ); ix += widthText + 2 ) {
        
        // draw tick
        wxPen pen( dc.GetPen() );
        pen.SetColour( wxColour( 0, 0, 0 ) );
        dc.SetPen( pen );
        dc.DrawLine( ix, bot, ix, top );

        // figure out time-at-sample based upon m_tdWinStart, m_tdPixelWidth
        if ( 2 + sizeText.GetHeight() < rect.height ) { // ensure there is space to draw
          td = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * ix;
          ss.str( std::string() );
          ss << td;
          std::string s( ss.str() );
          // colour for the tick mark legend, and is black
          SceneViewCommon::DrawTime( wxColour( 0, 0, 0 ), wxPoint( ix - widthTextBy2, top - 1 - sizeText.GetHeight() ), s );
        }
      }
    }
  }
}

// takes cursor position in window, calculates and returns new window beginning and pixel width
SceneMgmtView::TimePixelMapping SceneMgmtView::UpdateMouseZoomIn( const int x ) {
  
  assert( 0 <= x );
  
  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  assert( x < width );
  
  // is there a minimum pixel width to be checked?
  
  boost::posix_time::time_duration tdAtX 
    = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * x;
  boost::posix_time::time_duration tdPixelWidth = (m_tdTimePixelMapping.tdPixelWidth * 3) / 4;  // use this ratio for now
  if ( boost::posix_time::milliseconds( 1 ) > tdPixelWidth ) tdPixelWidth = boost::posix_time::milliseconds( 1 );
  //boost::posix_time::time_duration tdRelativeOffset = (tdPixelWidth * x) / width; // linear interpolation
  boost::posix_time::time_duration tdWinStart = tdAtX - ( tdPixelWidth * x );  // ** needs an offset
  assert( boost::posix_time::time_duration( 0, 0, 0 ) <= tdWinStart );  // assert rather than fix to assess math correctness
  
  m_tdTimePixelMapping.tdPixelWidth = tdPixelWidth;
  m_tdTimePixelMapping.tdWinStart = tdWinStart;
  
  return m_tdTimePixelMapping;

}

// takes cursor position in window, calculates and returns new window beginning and pixel width
SceneMgmtView::TimePixelMapping SceneMgmtView::UpdateMouseZoomOut( const int x ) {
  
  assert( 0 <= x );
  
  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  assert( x < width );
  
  // is there a minimum pixel width to be checked?
  static const boost::posix_time::time_duration tdOneMinute( boost::posix_time::time_duration( 0, 1, 0 ) );
  boost::posix_time::time_duration tdAtX 
    = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * x;
  boost::posix_time::time_duration tdPixelWidth = (m_tdTimePixelMapping.tdPixelWidth * 4) / 3;  // use this ratio for now
  if ( tdOneMinute < tdPixelWidth ) tdPixelWidth = tdOneMinute;
  //boost::posix_time::time_duration tdRelativeOffset = (tdPixelWidth * x) / width; // linear interpolation
  boost::posix_time::time_duration tdOffset = tdPixelWidth * x;
  if ( tdOffset >= tdAtX ) {
    m_tdTimePixelMapping.tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );
  }
  else {
    m_tdTimePixelMapping.tdWinStart = tdAtX - tdOffset;
  }
  assert( boost::posix_time::time_duration( 0, 0, 0 ) <= m_tdTimePixelMapping.tdWinStart );
  
  m_tdTimePixelMapping.tdPixelWidth = tdPixelWidth;
  
  return m_tdTimePixelMapping;

}

// takes cursor position in window, calculates and returns new window beginning and pixel width
SceneMgmtView::TimePixelMapping SceneMgmtView::UpdateMouseShift( const int x ) {

  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  
  if ( 0 != x ) {
    if ( 0 > x ) { // closer to origin
      m_tdTimePixelMapping.tdWinStart += m_tdTimePixelMapping.tdPixelWidth * -x;
    }
    else { // farther from origin
      boost::posix_time::time_duration tdOffset = m_tdTimePixelMapping.tdPixelWidth * x;
      if ( tdOffset >= m_tdTimePixelMapping.tdWinStart ) {
        m_tdTimePixelMapping.tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );
      }
      else {
        m_tdTimePixelMapping.tdWinStart -= tdOffset;
      }
    }
  }
  return m_tdTimePixelMapping;
}

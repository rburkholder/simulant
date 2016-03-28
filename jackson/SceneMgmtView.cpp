/* 
 * File:   SceneView.cpp
 * Author: rpb
 * 
 * Created on June 15, 2015, 10:28 AM
 */

#include <math.h>

#include <string>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

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
  
  std::cout << "scene constructed" << std::endl;
  
  return true;
}

SceneMgmtView::~SceneMgmtView( ) {
  //ClearViews();   // maybe bind the ondestroy event in the map'd views
  std::cout << "scene destroyed" << std::endl;
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

  ConnectSignals( m_view, this );
  
// there is an already existing paint event for this, so can we trigger the event?  HandlePaint
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

void SceneMgmtView::ConnectSignals( View& view, SceneViewCommon* pview ) {
  namespace args = boost::phoenix::arg_names;

  view.connMouseMotion = pview->m_signalMouseMotion.connect( boost::phoenix::bind( &SceneMgmtView::HandleMouseMotion, this, args::arg1, args::arg2 ) );
  view.connMouseShift = pview->m_signalMouseShift.connect( boost::phoenix::bind( &SceneMgmtView::HandleMouseShift, this, args::arg1 ) );
  view.connZoomIn = pview->m_signalZoomIn.connect( boost::phoenix::bind( &SceneMgmtView::HandleZoomIn, this, args::arg1 ) );
  view.connZoomOut = pview->m_signalZoomOut.connect( boost::phoenix::bind( &SceneMgmtView::HandleZoomOut, this, args::arg1 ) );
  view.connMouseDeparts = pview->m_signalMouseDeparts.connect( boost::phoenix::bind( &SceneMgmtView::HandleMouseDeparts, this, args::arg1 ) );
}

void SceneMgmtView::AddView(const std::string& sViewName, SceneViewCommon* pview) {
  
  if ( m_mapViews.end() != m_mapViews.find( sViewName ) ) {
    throw std::invalid_argument( "name exists: " + sViewName );
  }
  
  View view;
  
  view.pSceneViewCommon = pview;
  
  ConnectSignals( view, pview );
  
  m_mapViews[ sViewName ] = view;  // are the connections copied, or do we need to put onto the heap?
  
  pview->SetTimePixelMapping( m_tdTimePixelMapping );  // set the new addition to the same time frame
}

void SceneMgmtView::DisconnectSignals( View& view ) {
  view.connMouseDeparts.disconnect();
  view.connMouseMotion.disconnect();
  view.connMouseShift.disconnect();
  view.connZoomIn.disconnect();
  view.connZoomOut.disconnect();
}

void SceneMgmtView::DeleteView( const std::string& sViewName ) {
  mapViews_t::iterator iter = m_mapViews.find( sViewName );
  if ( m_mapViews.end() == iter ) {
    throw std::invalid_argument( "name not found: " + sViewName );
  }
  
  DisconnectSignals( iter->second );
  
  m_mapViews.erase( iter ); // when the view is destroyed, the connections are disconnected
}

void SceneMgmtView::RenameView(const std::string& sOldName, const std::string& sNewName) {
  // is case sensitive
  if ( sOldName != sNewName ) {
    mapViews_t::const_iterator iterOld = m_mapViews.find( sOldName );
    if ( m_mapViews.end() == iterOld ) {
      throw std::invalid_argument( "name not found: " + sOldName );
    }
    mapViews_t::const_iterator iterNew = m_mapViews.find( sNewName );
    if ( m_mapViews.end() != iterNew ) {
      throw std::invalid_argument( "name already exists: " + sNewName );
    }
    // erase first or insert first?
    m_mapViews[ sNewName ] = iterOld->second;
    m_mapViews.erase( iterOld );
  }
}

void SceneMgmtView::ClearViews( void ) {
  while ( !m_mapViews.empty() ) {
    mapViews_t::iterator iter = m_mapViews.begin();
    DeleteView( iter->first );
  }
}

void SceneMgmtView::HandleMouseMotion( int x, int diff ) {
  //std::cout << "mouse motion" << std::endl;
  UpdateInteractiveCursor( x );
  //m_psv->Refresh();
  for ( mapViews_t::iterator iter = m_mapViews.begin(); m_mapViews.end() != iter; ++iter ) {
    iter->second.pSceneViewCommon->UpdateInteractiveCursor( x );
    //iter->second.pSceneViewCommon->Refresh(); // is this one needed?
  }
}

void SceneMgmtView::HandleMouseShift( int diff ) {
  //std::cout << "mouse shift" << std::endl;
  SceneMgmtView::TimePixelMapping tpm;
  tpm = UpdateMouseShift( diff );
  //m_psv->Refresh();
  for ( mapViews_t::iterator iter = m_mapViews.begin(); m_mapViews.end() != iter; ++iter ) {
    iter->second.pSceneViewCommon->UpdateMouseShift( diff, tpm.tdWinStart, tpm.tdPixelWidth );
    iter->second.pSceneViewCommon->Refresh();
  }
}

void SceneMgmtView::HandleZoomIn( wxCoord x ) { 
  //assert( 0 != m_psv );
  SceneMgmtView::TimePixelMapping tpm;
  tpm = UpdateMouseZoomIn( x );
  //
  //std::cout << "mouse zoom in" << std::endl;
  for ( mapViews_t::iterator iter = m_mapViews.begin(); m_mapViews.end() != iter; ++iter ) {
    iter->second.pSceneViewCommon->UpdateMouseZoomIn( x, tpm.tdWinStart, tpm.tdPixelWidth );
    iter->second.pSceneViewCommon->Refresh();
  }
}

void SceneMgmtView::HandleZoomOut( wxCoord x ) {
  //assert( 0 != m_psv );
  SceneMgmtView::TimePixelMapping tpm;
  tpm = UpdateMouseZoomOut( x );
  //m_psv->Refresh();
  //std::cout << "mouse zoom out" << std::endl;
  for ( mapViews_t::iterator iter = m_mapViews.begin(); m_mapViews.end() != iter; ++iter ) {
    iter->second.pSceneViewCommon->UpdateMouseZoomOut( x, tpm.tdWinStart, tpm.tdPixelWidth );
    iter->second.pSceneViewCommon->Refresh();
  }
}

void SceneMgmtView::HandleMouseDeparts( int x ) {
  //std::cout << "mouse departs" << std::endl;
  UpdateInteractiveCursor( x );
  for ( mapViews_t::iterator iter = m_mapViews.begin(); m_mapViews.end() != iter; ++iter ) {
    iter->second.pSceneViewCommon->UpdateInteractiveCursor( x, false );  // turn the cursor off
  }
}

void SceneMgmtView::UpdateInteractiveCursor( int x, bool bTurnOn ) {
  
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


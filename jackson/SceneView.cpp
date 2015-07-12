/* 
 * File:   SceneView.cpp
 * Author: rpb
 * 
 * Created on June 15, 2015, 10:28 AM
 */

#include <math.h>

#include <wx/wx.h>

#include "SceneView.h"

IMPLEMENT_DYNAMIC_CLASS( SceneView, SceneViewCommon )

SceneView::SceneView( ) {
  Init();
}

SceneView::SceneView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void SceneView::Init() {
}

bool SceneView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
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

SceneView::~SceneView( ) {
}

void SceneView::CreateControls() {
  Bind( wxEVT_PAINT, &SceneView::HandlePaint, this );   // from SceneViewCommon
  Bind( wxEVT_ERASE_BACKGROUND, &SceneView::HandleEraseBackground, this );   // from SceneViewCommon
  Bind( wxEVT_SIZE, &SceneView::HandleSize, this );   // from SceneViewCommon
  Bind( wxEVT_RIGHT_UP, &SceneView::HandleMouseRightUp, this );   // from SceneViewCommon
  Bind( wxEVT_LEFT_UP, &SceneView::HandleMouseLeftUp, this );   // from SceneViewCommon
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
  
}

void SceneView::HandlePaint( wxPaintEvent& event ) {
  SceneViewCommon::HandlePaint( event );
  wxPaintDC dc(this);
  DrawLegend( dc );
}

void SceneView::UnDrawCursor( wxClientDC& dc, Cursor& cursor ) {
  SceneViewCommon::UnDrawCursor( dc, cursor );
  DrawLegend( dc );
}

void SceneView::DrawTime( const std::string& sTime ) {
  wxClientDC dc( this );
  SceneViewCommon::DrawTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText, sTime );
}

void SceneView::DrawLegend( wxClientDC& dc ) {

  std::stringstream ss;

  wxRect rect = GetClientRect();

  // text sizing calcs
  wxSize sizeText = dc.GetTextExtent( SceneViewCommon::sZeroTime );
  int widthText = sizeText.GetWidth();
  int widthTextBy2 = widthText / 2;
  
  // play with drawing time ticks
  if ( ( 0 < widthText ) && ( 10 < rect.width ) ) {  // non zero width text and at least 10 pixel widths to draw
    int nTextExtents = rect.width / ( widthText + 4 );  // how many ticks can we draw
    if ( ( 0 < nTextExtents ) && ( 4 <= rect.height ) ) { // there are some extents to draw, and some height to draw them in

      // prep tick locations
      int top = std::min<int>( rect.height, rect.height - 10 );  // might not be set quite right for x pixel high tic
      top = std::max<int>( 1, top );
      int bot = rect.height - 1;

      // will need work on normalizing values, but at least get some sort of value in place for now
      // ticks start at 1/2 size of extent
      boost::posix_time::time_duration td;
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
          SceneViewCommon::DrawTime( wxColour( 0, 0, 0 ), wxPoint( ix - widthTextBy2, 2 ), s );
        }
      }
    }
  }
}

void SceneView::UpdateMouseZoomIn( const int x, TimePixelMapping& tpm ) {
  
  assert( 0 <= x );
  
  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  assert( x < width );
  
  // is there a minimum pixel width to be checked?
  boost::posix_time::time_duration tdAtX 
    = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * x;
  boost::posix_time::time_duration tdPixelWidth = (m_tdTimePixelMapping.tdPixelWidth * 3) / 4;  // use this ratio for now
  //boost::posix_time::time_duration tdRelativeOffset = (tdPixelWidth * x) / width; // linear interpolation
  boost::posix_time::time_duration tdWinStart = tdAtX - ( tdPixelWidth * x );  // ** needs an offset
  assert( boost::posix_time::time_duration( 0, 0, 0 ) <= tdWinStart );  // assert rather than fix to assess math correctness
  
  m_tdTimePixelMapping.tdPixelWidth = tdPixelWidth;
  m_tdTimePixelMapping.tdWinStart = tdWinStart;
  
  tpm = m_tdTimePixelMapping;

}

void SceneView::UpdateMouseZoomOut( const int x, TimePixelMapping& tpm ) {
  
  assert( 0 <= x );
  
  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  assert( x < width );
  
  // is there a minimum pixel width to be checked?
  boost::posix_time::time_duration tdAtX 
    = m_tdTimePixelMapping.tdWinStart + m_tdTimePixelMapping.tdPixelWidth * x;
  boost::posix_time::time_duration tdPixelWidth = (m_tdTimePixelMapping.tdPixelWidth * 4) / 3;  // use this ratio for now
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
  
  tpm = m_tdTimePixelMapping;

}

void SceneView::UpdateMouseShift( const int x, TimePixelMapping& tpm ) {

  const wxRect rect = GetClientRect();
  const int width( rect.GetWidth() );
  assert( 0 < width );
  
  if ( 0 != x ) {
    if ( 0 < x ) { // positive
      m_tdTimePixelMapping.tdWinStart += m_tdTimePixelMapping.tdPixelWidth * x;
    }
    else { // negative
      boost::posix_time::time_duration tdOffset = m_tdTimePixelMapping.tdPixelWidth * -x;
      if ( tdOffset >= m_tdTimePixelMapping.tdWinStart ) {
        m_tdTimePixelMapping.tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );
      }
      else {
        m_tdTimePixelMapping.tdWinStart -= tdOffset;
      }
    }
  }
}

wxBitmap SceneView::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon SceneView::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

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

  std::cout << "SceneView colour" << std::endl;
  m_colourBackground = wxColour( 200, 200, 200 );

  m_tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );  // time starts at 0
  static boost::posix_time::time_duration tdOneSecond = boost::posix_time::seconds( 1 );
  m_tdPixelWidth = tdOneSecond / 24;  // 24 frames per second, one frame per pixel to start

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
  DrawDecorations( dc );
}

void SceneView::DrawDecorations( wxClientDC& dc ) {
  wxRect rect = GetClientRect();
  if (0 < rect.width) {
    if (1 <= rect.height) { // can draw ticks
      int top = std::min<int>( rect.height, rect.height - 10 );  // might not be set quite right for x pixel high tic
      top = std::max<int>( 1, top );
      int bot = rect.height - 1;
      for (int ix = 0; ix < rect.width; ix += 15) {
        wxPen pen( dc.GetPen() );
        pen.SetColour( wxColour( 0, 0, 0 ) );
        dc.SetPen( pen );
        dc.DrawLine( ix, bot, ix, top );
      }
    }
  }
}

void SceneView::ZoomIn( int x ) {
  wxRect rect = GetClientRect();
  int width( rect.GetWidth() );
  if ( 0 != width ) {
    boost::posix_time::time_duration tdPixelWidth = (m_tdPixelWidth * 3) / 4;  // use this ratio for now
    boost::posix_time::time_duration tdRelativeOffset = (tdPixelWidth * x) / width;
    boost::posix_time::time_duration tdWinStart = tdPixelWidth * x;
    m_tdWinStart = tdWinStart;
  }
}
/*void WaveformView::UpdateMouseZoomIn( int x ) {

  if ( 0 != m_pvSamples ) {
    const size_t width( m_vVertical.size() );
    if ( width == m_nSamplesInWindow ) {
      // can't zoom in any more
    }
    else {
      const size_t width( m_vVertical.size() );
      assert( x <= width );
      size_t ixAbsoluteSample = m_vVertical[ x ].index;
      size_t nSamplesInWindow = ( m_nSamplesInWindow * 3 ) / 4;  // use this ratio for now
      if ( width > nSamplesInWindow ) nSamplesInWindow = width;  // minimum of 1 to 1 samples
      size_t offsetRelative = ( x * nSamplesInWindow ) / width;
      size_t startAbsolute = ixAbsoluteSample - offsetRelative;
      assert( m_pvSamples->size() > ( startAbsolute + nSamplesInWindow ) );
      SummarizeSamples( width, startAbsolute, nSamplesInWindow );
    }
  }

}
*/

void SceneView::ZoomOut( int x ) {

}

void SceneView::Shift( int x ) { // number of pixels

}

void SceneView::DrawTime( const std::string& sTime ) {
  wxClientDC dc( this );
  SceneViewCommon::DrawTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText, sTime );
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

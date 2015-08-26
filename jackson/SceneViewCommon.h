/* 
* File:   SceneViewCommon.h
* Author: rpb
*
* Created on June 8, 2015, 10:37 PM
*/

#pragma once

#include <boost/signals2.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <wx/panel.h>
#include <wx/dcclient.h>

#define SYMBOL_SCENEVIEWCOMMON_STYLE wxTAB_TRAVERSAL
#define SYMBOL_SCENEVIEWCOMMON_TITLE _("SceneViewCommon")
#define SYMBOL_SCENEVIEWCOMMON_IDNAME ID_SCENEVIEWCOMMON
#define SYMBOL_SCENEVIEWCOMMON_SIZE wxSize(100, 50)
#define SYMBOL_SCENEVIEWCOMMON_POSITION wxDefaultPosition

class SceneViewCommon: public wxPanel {
  DECLARE_DYNAMIC_CLASS( SceneViewCommon )
public:

  typedef boost::signals2::signal<void (int, int)> signalMouseMotion_t; // x, diff
  typedef signalMouseMotion_t::slot_type slotMouseMotion_t;

  typedef boost::signals2::signal<void (int)> signalMouseShift_t; //diff
  typedef signalMouseShift_t::slot_type slotMouseShift_t;

  typedef boost::signals2::signal<void (wxCoord)> signalMouseWheel_t;
  typedef signalMouseWheel_t::slot_type slotMouseWheel_t;

  struct TimePixelMapping {
    boost::posix_time::time_duration tdWinStart;  // first pixel starts at this time, 00:00;00 is minimum
    boost::posix_time::time_duration tdPixelWidth;  // each pixel represents this duration
    TimePixelMapping( void ) {
      tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );
      tdPixelWidth = boost::posix_time::seconds( 1 ); // to start, one pixel is one second of waveform or video
      //m_tdPixelWidth = tdOneSecond / 100;  // 100 frames per second, one frame per pixel to start
    }
  };
  
  SceneViewCommon( );
  SceneViewCommon( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_SCENEVIEWCOMMON_IDNAME, 
    const wxPoint& pos = SYMBOL_SCENEVIEWCOMMON_POSITION, 
    const wxSize& size = SYMBOL_SCENEVIEWCOMMON_SIZE, 
    long style = SYMBOL_SCENEVIEWCOMMON_STYLE );

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_SCENEVIEWCOMMON_IDNAME, 
    const wxPoint& pos = SYMBOL_SCENEVIEWCOMMON_POSITION, 
    const wxSize& size = SYMBOL_SCENEVIEWCOMMON_SIZE, 
    long style = SYMBOL_SCENEVIEWCOMMON_STYLE );

  virtual ~SceneViewCommon( );

  void SetName( const std::string& sName );

  signalMouseMotion_t m_signalMouseMotion;  // interactive cursor
  signalMouseShift_t m_signalMouseShift; // shifting content
  signalMouseWheel_t m_signalZoomIn; // zoom in
  signalMouseWheel_t m_signalZoomOut; // zoom out

  void UpdateInteractiveCursor( int x );
  
  virtual void UpdateMouseZoomIn( const int x, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel ) {}  // need to pass the time begin, pixel width structure
  virtual void UpdateMouseZoomOut( const int x, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel ) {}
  virtual void UpdateMouseShift( const int diff, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel ) {}

protected:

  struct Cursor {
    //std::string m_sDescription;
    bool m_bCursorActive;
    bool m_bCursorDrawn;
    size_t m_locCursor;  // x cursor location/index into Vertical
    size_t m_ixFrame;  // for frame indexing
    wxColour m_colourCursor;
    wxPoint m_pointStatusText;
    Cursor( void ): 
      m_bCursorActive( false ), m_bCursorDrawn( false ), 
      m_pointStatusText( wxPoint( 0, 0 ) ),
      m_locCursor( std::numeric_limits<size_t>::max() ), 
      m_ixFrame( std::numeric_limits<size_t>::max() ),
      m_colourCursor( wxColour( 255,255,255 ) ) {}
  };

  static const std::string sZeroTime;

  TimePixelMapping m_tdTimePixelMapping;
  
  wxColour m_colourBackground;

  Cursor m_cursorInteractive;
  Cursor m_cursorPlay;

  void DrawCursor( wxClientDC& dc, int ix, Cursor& cursor ); // if < 0, don't draw
  static const std::string TimeAtSample( size_t nSample, size_t numerator, size_t denominator );
  void DrawTime( const Cursor& cursor, const wxPoint& point, const std::string& sTime );
  void DrawTime( wxColour colourText, const wxPoint& point, const std::string& sTime, bool bErase=true );

  virtual void UnDrawCursor( wxClientDC& dc, Cursor& cursor );
  void EraseTime( Cursor& cursor, wxPoint& point );

  wxPoint m_pointLatestMouse;

  wxMenu* m_pContextMenu;
  
  virtual void HandlePaint( wxPaintEvent& );
  virtual void HandleEraseBackground( wxEraseEvent& );
  virtual void HandleSize( wxSizeEvent& );

  void HandleMouseRightUp( wxMouseEvent& );
  void HandleMouseLeftUp( wxMouseEvent& );
  
  void HandleAddKeyFrame( wxCommandEvent& event );
  void HandleEditKeyFrame( wxCommandEvent& event );
  void HandleDeleteKeyFrame( wxCommandEvent& event );
  
  void HandleCopy( wxCommandEvent& event );
  void HandlePaste( wxCommandEvent& event );

  typedef boost::signals2::signal<void (wxPoint&)> signalMouseEvent_t;
  typedef signalMouseEvent_t::slot_type slotMouseEvent_t;
  
  // how many are actually necessary for the owner
  signalMouseEvent_t m_signalMouseEventAddKeyFrame;
  signalMouseEvent_t m_signalMouseEventEditKeyFrame;
  signalMouseEvent_t m_signalMouseEventDeleteKeyFrame;
  signalMouseEvent_t m_signalMouseEventSelectKeyFrame;
  signalMouseEvent_t m_signalMouseEventMovement;
  
private:

  enum {
    ID_Null = wxID_HIGHEST,
    ID_SCENEVIEWCOMMON
  };

  std::string m_sName;
  wxColour m_colourName;
  wxPoint m_pointName;

  wxPoint m_posMouseOnLeftDown;

  void DrawName( wxClientDC& dc );

  void HandleMouseWheel( wxMouseEvent& );
  void HandleMouseMotion( wxMouseEvent& );

  void HandleMouseLeftDown( wxMouseEvent& event );

  void HandleLeaveWindow( wxMouseEvent& );

  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };

};


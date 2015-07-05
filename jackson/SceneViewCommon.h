/* 
* File:   SceneViewCommon.h
* Author: rpb
*
* Created on June 8, 2015, 10:37 PM
*/

#pragma once

#include <boost/signals2.hpp>

#include <wx/panel.h>
#include <wx/dcclient.h>

#define SYMBOL_SCENEVIEWCOMMON_STYLE wxTAB_TRAVERSAL
#define SYMBOL_SCENEVIEWCOMMON_TITLE _("WaveFormView")
#define SYMBOL_SCENEVIEWCOMMON_IDNAME ID_SCENEVIEWCOMMON
#define SYMBOL_SCENEVIEWCOMMON_SIZE wxSize(100, 50)
#define SYMBOL_SCENEVIEWCOMMON_POSITION wxDefaultPosition

class SceneViewCommon: public wxPanel {
  DECLARE_DYNAMIC_CLASS( SceneViewCommon )
public:

  typedef std::vector<int16_t> vSamples_t;

  typedef boost::signals2::signal<void (int, int)> signalMouseMotion_t; // x, diff
  typedef signalMouseMotion_t::slot_type slotMouseMotion_t;

  typedef boost::signals2::signal<void (int)> signalMouseShift_t; //diff
  typedef signalMouseShift_t::slot_type slotMouseShift_t;

  typedef boost::signals2::signal<void (wxCoord)> signalMouseWheel_t;
  typedef signalMouseWheel_t::slot_type slotMouseWheel_t;

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
  void UpdatePlayCursor( size_t nFramesPlayed );
  virtual void UpdateMouseShift( int diff ) {}
  virtual void UpdateMouseZoomIn( int x ) {}
  virtual void UpdateMouseZoomOut( int x ) {}

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

  struct Vertical { // tracks a line for pixel width of the waveform
    size_t index;  // index into supplied waveform in m_pvSamples
    int16_t sampleMin;  // value we want to show  ( may use floats (6 digits) or double (15 digits) for everything )
    int16_t sampleMax;
    bool operator<( size_t rhs ) const { return ( index < rhs ); }
  };

  static const std::string sZeroTime;

  vSamples_t* m_pvSamples;

  typedef std::vector<Vertical> vVertical_t;
  vVertical_t m_vVertical;

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


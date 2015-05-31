/* 
 * File:   WaveformView.h
 * Author: rpb
 *
 * Created on May 28, 2015, 11:37 AM
 */

#pragma once

#include <vector>

#include <boost/atomic/atomic.hpp>

#include <wx/panel.h>

#define SYMBOL_CONTROLWAVEFORMVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLWAVEFORMVIEW_TITLE _("WaveFormView")
#define SYMBOL_CONTROLWAVEFORMVIEW_IDNAME ID_CONTROLWAVEFORMVIEW
#define SYMBOL_CONTROLWAVEFORMVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLWAVEFORMVIEW_POSITION wxDefaultPosition

// may turn this into a template to handle various sample types, or settle on one or two samples types and specialize
// possibly int32_t and float or double (for better precision)

class WaveformView: public wxPanel {
  DECLARE_DYNAMIC_CLASS( WaveformView )
public:
  
  typedef std::vector<int16_t> vSamples_t;
  
  WaveformView( );
  WaveformView( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLWAVEFORMVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLWAVEFORMVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLWAVEFORMVIEW_SIZE, 
          long style = SYMBOL_CONTROLWAVEFORMVIEW_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLWAVEFORMVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLWAVEFORMVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLWAVEFORMVIEW_SIZE, 
          long style = SYMBOL_CONTROLWAVEFORMVIEW_STYLE );
  virtual ~WaveformView( );
  
  void SetSamples( vSamples_t* );
  
  void ZoomIn( int x );
  void ZoomOut( int x );
  
  void Shift( int x ); // x is relative left or right
  
  void ResetPlayCursor( void );  // set to 0, maybe need a seek at some point
  void UpdatePlayCursor( size_t nFrames );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLWAVEFORMVIEW, 
    ID_EVENT_PLAYCURSOR
  };
  
  bool m_bMouseLeftDown;
  wxRect m_rectLast;
  
  struct Vertical { // tracks a line for pixel width of the waveform
    size_t index;  // index into supplied waveform in m_pvSamples
    int16_t sampleMin;  // value we want to show  ( may use floats (6 digits) or double (15 digits) for everything )
    int16_t sampleMax;
    bool operator<( size_t rhs ) const { return ( index < rhs ); }
  };
  
  typedef std::vector<Vertical> vVertical_t;
  vVertical_t m_vVertical;
  
  vSamples_t* m_pvSamples;
  
  size_t m_ixFirstSampleInWindow;
  size_t m_nSamplesInWindow;
  
  wxColour m_colourBackground;
  wxColour m_colourWaveform;
  
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
  
  Cursor m_cursorInteractive;
  Cursor m_cursorPlay;
  
  boost::atomic<size_t> m_nFramesPlayed;
  boost::atomic<size_t> m_nEventsQueued;
  
  void SummarizeSamplesOnEvent( void );
  void SummarizeSamples( unsigned long width, size_t ixStart, size_t n ); // sub-sample at interval based upon number of pixels present
  
  void HandlePaint( wxPaintEvent& );
  void HandleEraseBackground( wxEraseEvent& );
  
  void HandleIdle( wxIdleEvent& );
  
  void HandleSize( wxSizeEvent& );
  void HandleSizing( wxSizeEvent& );
  void HandleMouseLeftDown( wxMouseEvent& );
  void HandleMouseLeftUp( wxMouseEvent& );
  void HandleMouseWheel( wxMouseEvent& );
  void HandleMouseMotion( wxMouseEvent& );
  void HandleLeaveWindow( wxMouseEvent& );
  
  void HandlePlayCursor( wxCommandEvent& );
  
  void DrawTime( Cursor& cursor, wxPoint& point, size_t nSample, size_t numerator, size_t denominator );
  void EraseTime( Cursor& cursor, wxPoint& point );
  
  void DrawCursor( int ix, Cursor& cursor ); // if < 0, don't draw
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
};


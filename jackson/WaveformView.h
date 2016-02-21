/* 
 * File:   WaveformView.h
 * Author: rpb
 *
 * Created on May 28, 2015, 11:37 AM
 */

#pragma once

// 2015/08/02 this is currently a full waveform view in the scene view elements.
//   this structure ultimately needs to be segretated into two portions:
//   1) a clip describing the begin/end of displayed waveform
//   2) waveform viewer containing 0, 1, or more waveform clips
//   ultimately, this should simply draw the contained waveform in a provided visual space
//   but for the meantime, to get the view working properly:
//     a single waveform is present, is 00:00:00 based, and full length
//
//   clips in one waveform view are non-overlapping

#include <vector>

#include <boost/shared_ptr.hpp>

#include "SceneViewCommon.h"

#define SYMBOL_CONTROLWAVEFORMVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLWAVEFORMVIEW_TITLE _("WaveFormView")
#define SYMBOL_CONTROLWAVEFORMVIEW_IDNAME ID_CONTROLWAVEFORMVIEW
#define SYMBOL_CONTROLWAVEFORMVIEW_SIZE wxSize(100, 50)
#define SYMBOL_CONTROLWAVEFORMVIEW_POSITION wxDefaultPosition

// may turn this into a template to handle various sample types, or settle on one or two samples types and specialize
// possibly int32_t and float or double (for better precision)

class WaveformView: public SceneViewCommon {
  DECLARE_DYNAMIC_CLASS( WaveformView )
public:
  
  struct Waveform {
    typedef std::vector<int16_t> vSamples_t;
    unsigned int SamplesPerSecondNumerator;
    unsigned int SamplesPerSecondDenominator;
    vSamples_t* pvSamples;
    Waveform( void ): SamplesPerSecondNumerator( 1 ), SamplesPerSecondDenominator( 1 ), pvSamples( 0 ) {}
  };
  typedef boost::shared_ptr<Waveform> pWaveform_t;
  
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
  
  void SetSamples( pWaveform_t, boost::posix_time::time_duration tdPixelWidth );
  
  virtual void UpdateMouseShift( int x, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel );
  virtual void UpdateMouseZoomIn( int x, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel );
  virtual void UpdateMouseZoomOut( int x, boost::posix_time::time_duration start, boost::posix_time::time_duration widthPixel );
  
  virtual void UnDrawCursor( wxClientDC& dc, Cursor& cursor );
  
  void UpdatePlayCursor( size_t nFramesPlayed );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLWAVEFORMVIEW
  };
  
  // **** used in UpdatePlayCursor, need to update play cursor in waveform instead
  struct Vertical { // tracks a line for pixel width of the waveform
    size_t index;  // index into supplied waveform in m_pvSamples
    int16_t sampleMin;  // value we want to show  ( may use floats (6 digits) or double (15 digits) for everything )
    int16_t sampleMax;
    bool operator<( size_t rhs ) const { return ( index < rhs ); }
    bool operator<( const Vertical& rhs ) const { return index < rhs.index; }
    Vertical( void ): index( 0 ), sampleMin( 0 ), sampleMax( 0 ) {}
  };

  typedef std::vector<Vertical> vVertical_t;
  vVertical_t m_vVertical;  // contains waveform sub-samples

  bool m_bMouseLeftDown;
  wxRect m_rectLast;
  
  size_t m_ixFirstSampleInWindow;
  size_t m_nSamplesInWindow;
  
  wxColour m_colourWaveform;
  
  pWaveform_t m_pWaveform;

  void SummarizeSamplesOnEvent( void );
  void SummarizeSamples( unsigned long width, size_t ixStart, size_t n ); // sub-sample at interval based upon number of pixels present
  
  void SummarizeSamples( boost::posix_time::time_duration tdPixelWidth );
  
  void HandlePaint( wxPaintEvent& );
  void HandleEraseBackground( wxEraseEvent& );
  
  void HandleSize( wxSizeEvent& );
  void HandleSizing( wxSizeEvent& );
  void HandleMouseLeftDown( wxMouseEvent& );
  void HandleMouseLeftUp( wxMouseEvent& );
  void HandleMouseWheel( wxMouseEvent& );
  void HandleMouseMotion( wxMouseEvent& );
  
  void Init();
  void CreateControls();
};


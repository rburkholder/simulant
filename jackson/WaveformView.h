/* 
 * File:   WaveformView.h
 * Author: rpb
 *
 * Created on May 28, 2015, 11:37 AM
 */

#pragma once

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
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLWAVEFORMVIEW
  };
  
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
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
};


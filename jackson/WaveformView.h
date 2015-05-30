/* 
 * File:   WaveformView.h
 * Author: rpb
 *
 * Created on May 28, 2015, 11:37 AM
 */

#pragma once

#include <vector>

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
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLWAVEFORMVIEW
  };
  
  bool m_bMouseLeftDown;
  wxRect m_rectLast;
  
  struct Vertical { // tracks a line for pixel width of the waveform
    size_t index;  // index into supplied waveform in m_pvSamples
    int16_t sampleMin;  // value we want to show  ( may use floats (6 digits) or double (15 digits) for everything )
    int16_t sampleMax;
  };
  
  typedef std::vector<Vertical> vVertical_t;
  vVertical_t m_vVertical;
  
  vSamples_t* m_pvSamples;
  
  size_t m_ixFirstSampleInWindow;
  size_t m_nSamplesInWindow;
  
  void SummarizeSamplesOnEvent( void );
  void SummarizeSamples( unsigned long width, size_t ixStart, size_t n ); // sub-sample at interval based upon number of pixels present
  
  void HandlePaint( wxPaintEvent& );
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


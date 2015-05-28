/* 
 * File:   WaveformView.h
 * Author: rpb
 *
 * Created on May 28, 2015, 11:37 AM
 */

#pragma once

#include <vector>

#include <boost/thread/mutex.hpp>

#include <wx/panel.h>

#define SYMBOL_CONTROLWAVEFORMVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLWAVEFORMVIEW_TITLE _("WaveFormView")
#define SYMBOL_CONTROLWAVEFORMVIEW_IDNAME ID_CONTROLWAVEFORMVIEW
#define SYMBOL_CONTROLWAVEFORMVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLWAVEFORMVIEW_POSITION wxDefaultPosition

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
  
  void Clear( void );
  void AppendSamples( int nSamples, int16_t* );
  vSamples_t::size_type GetSampleCount( void ) { return m_vSamples.size(); }
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLWAVEFORMVIEW
  };
  
  boost::mutex m_mutexSamples;
  
  vSamples_t m_vSamples;
  
  void HandlePaint( wxPaintEvent& );
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
};


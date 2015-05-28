/* 
 * File:   WaveformView.cpp
 * Author: rpb
 * 
 * Created on May 28, 2015, 11:37 AM
 */

#include <limits>

#include <boost/thread/lock_guard.hpp>

#include <wx/wx.h>
#include <boost/chrono/process_cpu_clocks.hpp>

#include "WaveformView.h"

IMPLEMENT_DYNAMIC_CLASS( WaveformView, wxPanel )

WaveformView::WaveformView( ) {
  Init();
}

WaveformView::WaveformView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool WaveformView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

WaveformView::~WaveformView( ) {
}

void WaveformView::Init() {
  
}

void WaveformView::Clear( void ) { 
  boost::lock_guard<boost::mutex> guard( m_mutexSamples );
  m_vSamples.clear(); 
}

void WaveformView::AppendSamples( int nSamples, int16_t* p ) {
  boost::lock_guard<boost::mutex> guard( m_mutexSamples );
  while ( 0 != nSamples ) {
    m_vSamples.push_back( *p );
    ++p;
    --nSamples;
  }
}

void WaveformView::HandlePaint( wxPaintEvent& event ) {
  // when drawing, sub-sample at interval based upon number of pixels present
  wxPaintDC dc(this);
  dc.DrawText( wxT( "waveform" ), 2, 2 );
  boost::lock_guard<boost::mutex> guard( m_mutexSamples );
  if ( 0 != m_vSamples.size() ) {
    wxRect rect( this->GetClientRect() );
    // make a vector with an entry for each pixel for moving the line back and forth, each entry has relative time stamp
    // logarithmic zoom scale
    // ultimately:
    //   scan samples, looking for zero crossings, 
    //     on up->down: reset min and and adjust downwards
    //     on down->up: reset max and and adjust upwards
    // or keep track of min/max over 1 sec intervals, 1 ms intervals
    // for now, just sample every nth to get something on the screen
    int width( rect.GetWidth() );
    int height( rect.GetHeight() );
    int dxStep = m_vSamples.size() / ( 1 + width ); // the one for getting last value on display, check if correct for #samples < width
    int ixStep = 0;
    int halfheight = height / 2;
    int scale = std::numeric_limits<short>::max() / halfheight;
    int xPrv, yPrv;
    for ( int ix = 0; ix < width; ++ix ) {
      int val = ( m_vSamples[ ixStep ] / scale ) + halfheight;
      if ( 0 == ix ) {
        xPrv = 0;
        yPrv = val;  // may need to flip this as 0 is on top
        dc.DrawPoint( xPrv, yPrv );
      }
      else {
        dc.DrawLine( xPrv, yPrv, ix, val );
        xPrv = ix;
        yPrv = val;
      }
      ixStep += dxStep;
    }
  }
}

void WaveformView::CreateControls() {
  Bind( wxEVT_PAINT, &WaveformView::HandlePaint, this );
}

wxBitmap WaveformView::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon WaveformView::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

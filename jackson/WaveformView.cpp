/* 
 * File:   WaveformView.cpp
 * Author: rpb
 * 
 * Created on May 28, 2015, 11:37 AM
 */

#include <limits>
#include <algorithm>
#include <sstream>

#include <boost/thread/lock_guard.hpp>

#include <wx/wx.h>

#include "WaveformView.h"

IMPLEMENT_DYNAMIC_CLASS( WaveformView, SceneViewCommon )

WaveformView::WaveformView( ) {
  Init();
}

WaveformView::WaveformView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

void WaveformView::Init() {
  m_bMouseLeftDown = false;
  m_ixFirstSampleInWindow = 0;
  m_nSamplesInWindow = 0;
  
}

bool WaveformView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  SceneViewCommon::Create( parent, id, pos, size, style );

  m_colourWaveform = wxColour( 255, 102, 0 );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  //Centre();
  
  // this code snippet should be in a method
  wxRect rect( this->GetClientRect() );
  SummarizeSamples( rect.GetWidth(), m_ixFirstSampleInWindow, m_nSamplesInWindow );
  
  return true;
}

void WaveformView::CreateControls() {
  Bind( wxEVT_PAINT, &WaveformView::HandlePaint, this );
  //Bind( wxEVT_ERASE_BACKGROUND, &WaveformView::HandleEraseBackground, this );
  //Bind( wxEVT_MOTION, &WaveformView::HandleMouseMotion, this );

  Bind( wxEVT_SIZE, &WaveformView::HandleSize, this );
  //Bind( wxEVT_SIZING, &WaveformView::HandleSizing, this );
  //Bind( wxEVT_LEFT_DOWN, &WaveformView::HandleMouseLeftDown, this );
  //Bind( wxEVT_LEFT_UP, &WaveformView::HandleMouseLeftUp, this );
  //Bind( wxEVT_MOUSEWHEEL, &WaveformView::HandleMouseWheel, this );
  //Bind( wxEVT_LEAVE_WINDOW, &WaveformView::HandleLeaveWindow, this );
}

WaveformView::~WaveformView( ) {
}

void WaveformView::UnDrawCursor( Cursor& cursor ) {
  wxRect rectClientArea( this->GetClientRect() );
  int yMax( rectClientArea.height - 1 );
  wxClientDC dc( this );
  wxPen pen( cursor.m_colourCursor, 1, wxPENSTYLE_SOLID );
  dc.SetPen( pen );

  // redraw data that was under the cursor
  if ( cursor.m_bCursorDrawn ) {
    // draw vertical line with background colour
    wxBrush brush( dc.GetBrush() );
    brush.SetColour( m_colourBackground );
    dc.SetBrush( brush );
    wxPen pen( dc.GetPen() );
    pen.SetColour( m_colourBackground );
    dc.SetPen( pen );
    dc.DrawLine( cursor.m_locCursor, 0, cursor.m_locCursor, yMax );

    // prep calcs for redrawing waveform data
    pen.SetColour( m_colourWaveform );
    dc.SetPen( pen );

    int halfheight = rectClientArea.height / 2;
    int scale = std::numeric_limits<short>::max() / halfheight;
    //assert( width == m_vVertical.size() );  // not sure if this is the correct test

    // draw the pixel wide waveform data
    Vertical& vertical( m_vVertical[ cursor.m_locCursor ] );
    int valMin = ( vertical.sampleMin / scale ) + halfheight;
    int valMax = ( vertical.sampleMax / scale ) + halfheight;
    dc.DrawLine( cursor.m_locCursor, valMin, cursor.m_locCursor, valMax );
    cursor.m_bCursorDrawn = false;
  }
}

void WaveformView::SetSamples( vSamples_t* p ) {
  //std::cout << "Set Samples start" << std::endl;
  m_pvSamples = p;
  wxRect rect( this->GetClientRect() );
  if ( 0 == p ) {
    SummarizeSamples( rect.GetWidth(), 0, 0 );
  }
  else {
    SummarizeSamples( rect.GetWidth(), 0, p->size() );
  }
  this->Refresh();  // is this needed?
  //std::cout << "Set Samples done" << std::endl;
}

void WaveformView::HandleEraseBackground( wxEraseEvent& event ) {
  event.Skip();
}

void WaveformView::HandlePaint( wxPaintEvent& event ) {
  
  wxPaintDC dc(this);
  wxRect rectClientArea( this->GetClientRect() );
  int width( rectClientArea.GetWidth() );
  int height( rectClientArea.GetHeight() );
  if ((width != m_vVertical.size()) || (1 >= height)) {
    //std::cout << "window width: " << width << ", sample width: " << m_vVertical.size() << std::endl;
  }
  else {
    wxBrush brush( dc.GetBrush() );
    brush.SetColour( m_colourBackground );
    dc.SetBrush( brush );
    dc.DrawRectangle( rectClientArea );  // black out background
    wxPen pen( dc.GetPen() );
    pen.SetColour( m_colourWaveform );
    dc.SetPen( pen );
    
    int halfheight = height / 2;
    int scale = std::numeric_limits<short>::max() / halfheight;
    assert( width == m_vVertical.size() );

    vVertical_t::const_iterator iter = m_vVertical.begin();
    for ( int ix = 0; ix < width; ++ix ) {
      int valMin = ( iter->sampleMin / scale ) + halfheight;
      int valMax = ( iter->sampleMax / scale ) + halfheight;
      dc.DrawLine( ix, valMin, ix, valMax );
      ++iter;
    }
  }
}

// may want to do in background thread at some point
void WaveformView::SummarizeSamples( unsigned long width, size_t ixStart, size_t n ) {
  
  struct EmptyFill {
    inline void operator()( Vertical& vertical ) {
      vertical.index = ix;  ++ix;
      vertical.sampleMin = vertical.sampleMax = 0;
    }
    EmptyFill(): ix( 0 ) {};
    size_t ix;
  };
  
  struct StairCase {
    typedef WaveformView::vSamples_t::const_iterator citer;
    // I think there is an overflow case when no more stuff to put in remaining verticals
    // hopefully resolve, but a minor change if still goes over end
    StairCase( citer iterBegin_, citer iterEnd_, size_t step_ )
      : iterBegin( iterBegin_ ), iterEnd( iterEnd_ ), step( step_ ), remaining_in_step( step_ ), ix( 0 ) {}
    
    inline void operator()( Vertical& vertical ) {
      vertical.index = ix;
      vertical.sampleMin = vertical.sampleMax = ( iterEnd != iterBegin ) ? *iterBegin : 0;
      --remaining_in_step;
      if ( 0 == remaining_in_step ) {
        ++iterBegin;
        ++ix;
        remaining_in_step = step;
      }
    }
    
    size_t ix;
    size_t step;
    size_t remaining_in_step;
    citer iterBegin;
    citer iterEnd;
  };
  
  struct FullFill {
    typedef WaveformView::vVertical_t::iterator iter;
    typedef WaveformView::vVertical_t::const_iterator citer;
    FullFill( iter iterBegin_, citer iterEnd_, size_t start_, size_t step_ )
      : iterBegin( iterBegin_ ), iterEnd( iterEnd_ ), step( step_ ), remaining_in_step( step_ ), 
      ix( start_ ), ixBeginOfSummary( start_ ), min {}, max {}, initType {} {
        assert( 0 != step_ );
                                                      }
      ~FullFill( void ) {
        if ( 0 != remaining_in_step ) {
          //std::cout << "~fullfill " << step << "," << remaining_in_step << ", " << iterEnd - iterBegin << std::endl;
          if ( iterEnd != iterBegin ) {
            iterBegin->index = ixBeginOfSummary;  // index to beginning of summarized samples
            iterBegin->sampleMin = min;
            iterBegin->sampleMax = max;
            ++iterBegin;
          }
        }
      }
      
    inline void operator() ( int16_t sample ) {
      min = std::min<int16_t>( min, sample );
      max = std::max<int16_t>( max, sample );
      --remaining_in_step;
      if ( 0 == remaining_in_step ) {
        if ( iterEnd == iterBegin ) {
          //std::cout << "left over? " << ixBeginOfSummary << "," << ix << std::endl;
        }
        else {
          //assert( iterEnd != iterBegin );
          iterBegin->index = ixBeginOfSummary;  // index to beginning of summarized samples
          ixBeginOfSummary = ix;
          iterBegin->sampleMin = min;
          iterBegin->sampleMax = max;
          min = max = initType;
          remaining_in_step = step;
          ++iterBegin; 
        }
      }
      ++ix;
    }
    int16_t initType;
    int16_t min;
    int16_t max;
    size_t ixBeginOfSummary;
    size_t ix; // may not start at 0 if we are taking a sub-segment of samples
    size_t step;
    size_t remaining_in_step;
    iter iterBegin;
    citer iterEnd;
  };
  
  m_ixFirstSampleInWindow = ixStart;
  m_nSamplesInWindow = n;
  
  m_vVertical.resize( width );
  if ( ( 0 == m_pvSamples ) || ( 4 >= width ) ) { // show a straight line for little or nothing
    // fill with empty values
    //std::cout << "summary with EmptyFill 1" << std::endl;
    std::for_each( m_vVertical.begin(), m_vVertical.end(), EmptyFill() );
  }
  else {
    if ( ( 10 >= m_pvSamples->size() ) || ( 4 >= width ) ) {
      //std::cout << "summary with EmptyFill 2" << std::endl;
      std::for_each( m_vVertical.begin(), m_vVertical.end(), EmptyFill() );
    }
    else {
      if ( width >= m_pvSamples->size() ) { // should do a linear interpolation, or better yet, a curve of some sort
        // one to one sampling, or fill to match width
        //std::cout << "summary with StairCase" << std::endl;
        size_t step = width / m_pvSamples->size();     // integer arithmetic, drop the remainder
        std::for_each( m_vVertical.begin(), m_vVertical.end(), StairCase( m_pvSamples->begin(), m_pvSamples->end(), step ) );
      }
      else {
        // sub sample the samples
        // there might be an overflow when no data to fill end verticals, fill with zero
        // may have an overflow issue with not enough samples to fill vertical
        //std::cout << "summary with FullFill: " << width << ", v=" << m_vVertical.size() << ", s=" << m_pvSamples->size() << std::endl;
        size_t step = n / ( width ); // # samples per pixel width, integer arithmetic, make use of remainder
        vSamples_t::const_iterator iterBegin( m_pvSamples->begin() + ixStart );
        std::for_each( iterBegin, iterBegin + n, FullFill( m_vVertical.begin(), m_vVertical.end(), ixStart, step ) );
      }
    }
  }
}

void WaveformView::UpdateMouseShift( int x ) {
  if ( 0 != x ) {
    if ( 0 != m_pvSamples ) {
      const size_t width( m_vVertical.size() );
      const size_t nSamplesTotal( m_pvSamples->size() );
      assert( width <= m_nSamplesInWindow );
      size_t stepSamples = m_nSamplesInWindow / width;
      if ( 0 <= x ) { // positive
        size_t diff = x * stepSamples;
        size_t first = 0;
        if ( diff < m_ixFirstSampleInWindow ) first = m_ixFirstSampleInWindow - diff;
        SummarizeSamples( width, first, m_nSamplesInWindow );
      }
      else {
        size_t diff = -x * stepSamples;
        size_t first = m_ixFirstSampleInWindow + diff;
        if ( first >  ( nSamplesTotal - m_nSamplesInWindow ) ) first = nSamplesTotal - m_nSamplesInWindow;
        SummarizeSamples( width, first, m_nSamplesInWindow );

      }
    }
  }
}

void WaveformView::UpdateMouseZoomIn( int x ) {
  
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

void WaveformView::UpdateMouseZoomOut( int x ) {
  
  if ( 0 != m_pvSamples ) {
    const size_t size( m_pvSamples->size() );
    if ( size == m_nSamplesInWindow ) {
      // can't zoom out any more, need to check for the < issue, check done below
    }
    else {
      const size_t width( m_vVertical.size() );
      assert( x <= width );
      size_t ixAbsoluteSample = m_vVertical[ x ].index;
      size_t nSamplesInWindow = ( m_nSamplesInWindow * 4 ) / 3;  // use this ratio for now
      if ( size < nSamplesInWindow ) nSamplesInWindow = size;
      size_t offsetRelative = ( x * nSamplesInWindow ) / width;
      if ( offsetRelative > ixAbsoluteSample ) offsetRelative = ixAbsoluteSample;
      size_t startAbsolute = ixAbsoluteSample - offsetRelative;
      if ( startAbsolute > ( size - nSamplesInWindow ) ) startAbsolute = size - nSamplesInWindow;
      //std::cout << "final: " << startAbsolute << "," << size << "," << nSamplesInWindow << std::endl;
      assert( size >= ( startAbsolute + nSamplesInWindow ) );
      SummarizeSamples( width, startAbsolute, nSamplesInWindow );
    }
  }
}

void WaveformView::SummarizeSamplesOnEvent( void ) {
  wxRect rectNew = this->GetRect();
  if ( rectNew != m_rectLast ) {
    m_cursorInteractive.m_bCursorDrawn = false;
    m_cursorPlay.m_bCursorDrawn = false;
    m_rectLast = rectNew;
    SummarizeSamples( m_rectLast.GetWidth(), m_ixFirstSampleInWindow, m_nSamplesInWindow );
  }
}

void WaveformView::HandleSizing( wxSizeEvent& event ) {
  std::cout << "sizing" << std::endl;  // doesn't appear to be invoked
  event.Skip();
}

void WaveformView::HandleSize( wxSizeEvent& event ) {
  //std::cout << "sized" << std::endl;
  if ( !m_bMouseLeftDown ) {
    SummarizeSamplesOnEvent();
  }
  event.Skip();
}

void WaveformView::HandleMouseLeftDown( wxMouseEvent& event ) {
  std::cout << "leftdown" << std::endl;  // isn't being called
  m_bMouseLeftDown = true;
  m_rectLast = this->GetRect();
  event.Skip();
}

void WaveformView::HandleMouseLeftUp( wxMouseEvent& event ) {
  std::cout << "leftup" << std::endl; // isn't being called
  m_bMouseLeftDown = false;
  SummarizeSamplesOnEvent();
  event.Skip();
}

void WaveformView::HandleMouseWheel( wxMouseEvent& event ) {
  //std::cout << "wheel: " << event.GetWheelRotation() << std::endl;
  event.Skip();
}

void WaveformView::HandleMouseMotion( wxMouseEvent& event ) {
  wxPoint posMouse = event.GetPosition();
  if ( 0 != m_pvSamples ) {
     
    int x = event.GetPosition().x;
    UnDrawCursor( m_cursorInteractive );
    SceneViewCommon::DrawCursor( x, m_cursorInteractive );

    size_t nSample( m_vVertical[ posMouse.x ].index );
    DrawTime( m_cursorInteractive, m_cursorInteractive.m_pointStatusText, TimeAtSample( nSample, 1, 44100 ) );
    
  }
  event.Skip();
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

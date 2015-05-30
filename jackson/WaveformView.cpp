/* 
 * File:   WaveformView.cpp
 * Author: rpb
 * 
 * Created on May 28, 2015, 11:37 AM
 */

#include <limits>
#include <algorithm>

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

void WaveformView::Init() {
  m_pvSamples = 0;
  m_bMouseLeftDown = false;
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
  
  // this code snippet should be in a method
  wxRect rect( this->GetClientRect() );
  SummarizeSamples( rect.GetWidth() );
  
  return true;
}

void WaveformView::CreateControls() {
  Bind( wxEVT_PAINT, &WaveformView::HandlePaint, this );
  Bind( wxEVT_SIZE, &WaveformView::HandleSize, this );
  Bind( wxEVT_SIZING, &WaveformView::HandleSizing, this );
  Bind( wxEVT_LEFT_DOWN, &WaveformView::HandleMouseLeftDown, this );
  Bind( wxEVT_LEFT_UP, &WaveformView::HandleMouseLeftUp, this );
}

WaveformView::~WaveformView( ) {
}

void WaveformView::SetSamples( vSamples_t* p ) {
  //std::cout << "Set Samples start" << std::endl;
  m_pvSamples = p;
  wxRect rect( this->GetClientRect() );
  SummarizeSamples( rect.GetWidth() );
  this->Refresh();
  //std::cout << "Set Samples done" << std::endl;
}

//void WaveformView::Clear( void ) { 
//  boost::lock_guard<boost::mutex> guard( m_mutexSamples );
//  m_vSamples.clear(); 
//}

//void WaveformView::AppendSamples( int nSamples, int16_t* p ) {
//  boost::lock_guard<boost::mutex> guard( m_mutexSamples );
//  while ( 0 != nSamples ) {
//    m_vSamples.push_back( *p );
//    ++p;
//    --nSamples;
//  }
//}

void WaveformView::HandlePaint( wxPaintEvent& event ) {
      // make a vector with an entry for each pixel for moving the line back and forth, each entry has relative time stamp
      // logarithmic zoom scale
      // ultimately:
      //   scan samples, looking for zero crossings, 
      //     on up->down: reset min and and adjust downwards
      //     on down->up: reset max and and adjust upwards
      // or keep track of min/max over 1 sec intervals, 1 ms intervals
  
  wxPaintDC dc(this);
  dc.DrawText( wxT( "waveform" ), 2, 2 );
  //boost::lock_guard<boost::mutex> guard( m_mutexSamples );
  wxRect rect( this->GetClientRect() );
  int width( rect.GetWidth() );
  int height( rect.GetHeight() );
  if ( width != m_vVertical.size() ) {
    //std::cout << "window width: " << width << ", sample width: " << m_vVertical.size() << std::endl;
  }
  else {
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
void WaveformView::SummarizeSamples( unsigned long width ) {
  
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
    FullFill( iter iterBegin_, citer iterEnd_, size_t step_ )
      : iterBegin( iterBegin_ ), iterEnd( iterEnd_ ), step( step_ ), remaining_in_step( step_ ), 
      ix( 0 ), ixBeginOfSummary( 0 ), min {}, max {}, initType {} {
        assert( 0 != step_ );
                                                      }
      ~FullFill( void ) {
        if ( 0 != remaining_in_step ) {
          std::cout << "~fullfill " << step << "," << remaining_in_step << ", " << iterEnd - iterBegin << std::endl;
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
        assert( iterEnd != iterBegin );
        iterBegin->index = ixBeginOfSummary;  // index to beginning of summarized samples
        ixBeginOfSummary = ix;
        iterBegin->sampleMin = min;
        iterBegin->sampleMax = max;
        min = max = initType;
        remaining_in_step = step;
        ++iterBegin; 
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
        // not sure yet if a +1 is needed after width
        // may have an overflow issue with not enough samples to fill vertical
        //std::cout << "summary with FullFill: " << width << ", v=" << m_vVertical.size() << ", s=" << m_pvSamples->size() << std::endl;
        size_t step = m_pvSamples->size() / ( width - 1 ); // # samples per pixel width, integer arithmetic, make use of remainder
        //std::for_each( m_vVertical.begin(), m_vVertical.end(), FullFill( m_pvSamples->begin(), m_pvSamples->end(), step ) );
        std::for_each( m_pvSamples->begin(), m_pvSamples->end(), FullFill( m_vVertical.begin(), m_vVertical.end(), step ) );
      }
    }
  }
}

void WaveformView::SummarizeSamplesOnEvent( void ) {
  wxRect rectNew = this->GetRect();
  if ( rectNew != m_rectLast ) {
    m_rectLast = rectNew;
    SummarizeSamples( m_rectLast.GetWidth() );
  }
}

void WaveformView::HandleSizing( wxSizeEvent& event ) {
  std::cout << "sizing" << std::endl;  // doesn't appear to be invoked
}

void WaveformView::HandleSize( wxSizeEvent& event ) {
  //std::cout << "sized" << std::endl;
  if ( !m_bMouseLeftDown ) {
    SummarizeSamplesOnEvent();
  }
}

void WaveformView::HandleMouseLeftDown( wxMouseEvent& ) {
  std::cout << "leftdown" << std::endl;  // isn't being called
  m_bMouseLeftDown = true;
  m_rectLast = this->GetRect();
}

void WaveformView::HandleMouseLeftUp( wxMouseEvent& ) {
  std::cout << "leftup" << std::endl; // isn't being called
  m_bMouseLeftDown = false;
  SummarizeSamplesOnEvent();
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

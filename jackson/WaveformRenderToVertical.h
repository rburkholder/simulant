/* 
 * File:   WavefromRenderToVertical.h
 * Author: rpb
 *
 * Created on February 21, 2016, 9:28 PM
 */

#pragma once

#include <vector>

#include "QuadraticInterpolation.h"

// have to think about waveform subsets:
//   may be space prior to the waveform
//   may be space after the waveform

template<typename sample_t>
struct Vertical { // tracks a line for pixel width of the waveform
  // #samples > #pixels: summarize samples into Vertical, index step > 1
  // #samples = #pixels: one sample per Vertical, index step = 1
  // #samples < #pixels: interpolate between samples, multiple Verticals have same index
  size_t index;  // index into supplied waveform
  sample_t sampleMin;  // value we want to show  ( may use floats (6 digits) or double (15 digits) for everything )
  sample_t sampleMax;
  bool operator<( size_t rhs ) const { return ( index < rhs ); }
  bool operator<( const Vertical& rhs ) const { return index < rhs.index; }
  Vertical( void ): index( 0 ), sampleMin( sample_t() ), sampleMax( sample_t() ) {}
  Vertical( size_t index_ ): index( index_ ), sampleMin {}, sampleMax {} {};
  Vertical( size_t index_, sample_t min_, sample_t max_ ): index( index_ ), sampleMin( min_ ), sampleMax( max_ ) {};
  void Set( sample_t min, sample_t max ) { sampleMin = min; sampleMax = max; }
};

template<typename sample_t>
struct WaveformRenderToVertical {
  
  typedef typename std::vector<sample_t> vSample_t;
  typedef typename std::vector<Vertical< sample_t> > vVertical_t;
  
  struct Results {
    long PixelsPerSample;
    long SamplesPerPixel;
    Results( void ): PixelsPerSample( 0 ), SamplesPerPixel( 0 ) {}
    Results( long pps, long spp ): PixelsPerSample( pps ), SamplesPerPixel( spp ) {}
  };
  
  vVertical_t& m_vVertical;
  
  WaveformRenderToVertical( vVertical_t& vVertical )
  : m_vVertical( vVertical )
  {}
  
  void SummarizeWorkInProgress( 
    typename vSample_t::const_iterator begin, 
    typename vSample_t::const_iterator end, 
    size_t nPixelsToCover, // may not be able to cover all pixels
    size_t nSamplesPerPixel 
  ) {
  }

  // will have to re-write this sometime because rounding will not sync further down the waveform in the scene
  void Summarize( 
    typename vSample_t::const_iterator begin, 
    typename vSample_t::const_iterator end, 
    size_t nVerticalsToCover, // may not be able to cover all pixels
    size_t nSamplesPerPixel 
) {
    
    typename vVertical_t::reverse_iterator iter;
    
    if ( 0 == nSamplesPerPixel ) return;
    
    m_vVertical.clear();
    m_vVertical.reserve( nVerticalsToCover );
    
    int nVerticalsToProcess( nVerticalsToCover );
    size_t cnt( 0 ); // repeat the count to pack the samples
    size_t ixSample( 0 );
    size_t nSamplesProcessed( 0 );
    size_t nPixelsProcessed( 0 );
    while ( ( begin != end ) && ( 0 != nVerticalsToProcess ) ) {
      if ( 0 == cnt ) {
	m_vVertical.push_back( Vertical<sample_t>( ixSample, *begin, *begin ) );
	iter = m_vVertical.rbegin();
	cnt = nSamplesPerPixel;
        nVerticalsToProcess--;  // does this leave us with an empty vertical, or maybe an unfinished vertical?
	nPixelsProcessed++;
      }
      else {
	iter->sampleMin = std::min<sample_t>( iter->sampleMin, *begin );
	iter->sampleMax = std::max<sample_t>( iter->sampleMax, *begin );
      }
      cnt--;
      ixSample++;
      begin++;
      nSamplesProcessed++;
    }
    std::cout << "Summarized: " << nSamplesProcessed << " samples, " << nPixelsProcessed << " pixels" << std::endl;
  }
  
  Results SummarizeGivenTotalPixels( 
    typename vSample_t::const_iterator begin, 
    typename vSample_t::const_iterator end, 
    size_t ttlPixels 
  ) {
    Results results;
    m_vVertical.clear();
    m_vVertical.reserve( ttlPixels );
    typename vSample_t::difference_type ttlSamples( end - begin );
    // need at least three samples
    if ( ( 3 > ttlSamples ) || ( 0 == ttlPixels ) ) return;
    if ( ttlSamples < ttlPixels ) {  // pixels per sample, integer rounding down
      // interpolate between samples
      results.PixelsPerSample = ttlPixels / ttlSamples;  // rounding involved here, which requires a fill at the end
      long ttlOps = ttlSamples - 3 + 1;  // based upon three samples required for QuadraticInterpolation
      //long cntFills = 0;  // used for post fill
      typename vSample_t::const_iterator iy1( begin );
      typename vSample_t::const_iterator iy2( iy1 ); iy2++;
      typename vSample_t::const_iterator iy3( iy2 ); iy3++;
      long ix = 0; // step through Verticals
      QuadraticInterpolation<long, sample_t, sample_t> qi;
      long x1( 0 ), x2( 0 ), x3( 0 ), cnt( 0 );
      for ( cnt = 0; cnt < ttlOps; cnt++ ) {
	x1 = ix;
	x2 = x1 + results.PixelsPerSample;
	x3 = x2 + results.PixelsPerSample;
	qi.Reset( x1, *iy1, x2, *iy2, x3, *iy3 );
	while ( ix < x2 ) {
	  sample_t sample = qi( ix );
	  m_vVertical.push_back( Vertical<sample_t>( cnt, sample, sample ) );
	  ix++;
	}
	iy1++; iy2++; iy3++;
      }
      while ( ix < ttlPixels) { // finish the fill
	// repeat the previous code
	sample_t sample = qi( ix );
	m_vVertical.push_back( Vertical<sample_t>( ( ix < x3 ? cnt : cnt + 1 ), sample, sample ) );
	ix++;
      }
    }
    else { // samples per pixel
      // summarize samples
      results.SamplesPerPixel = ttlSamples / ttlPixels;  // integer rounding down
      SummarizeGivenSamplesPerPixel( begin, end, results.SamplesPerPixel );
    }
  return results;
  }
  
};


/*
template<typename sample_t>
void SummarizeSamples( unsigned long width, size_t ixStart, size_t n ) {
  
  struct EmptyFill {
    inline void operator()( Vertical& vertical ) {  // maybe use iterator instead, no because Vertical needs an index
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
      if ( width >= m_pvSamples->size() ) { // should do a linear interpolation, or better yet, a curve of some sort, or spline
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
*/

/*
template<typename sample_t>
void WaveformView::SummarizeSamples( boost::posix_time::time_duration tdPixelWidth ) {
  // each clip needs it's offset into the timeline
  
  // do things differently:
  //  cache the whole file once per time frame change
  //    issue:  when many media files are available, need to recache them all
  //      so maybe only cache the portions required?
  //      but, at the extremes, all needs to be cached anyway
  //   so, maybe, there is a different way of doing this
  //  not sure how, as a waveform requires summarization for display, and depends upon the resolution present
  //  therefore, if many waveforms present, get a bunch of workthreads to perform the work
  
  // current thought:
  //   summarize whole waveform, then display appropriate portions later
  //   which, I think, kinda provides a two stage pipeline:  summarize whole waveform, then display what ever is visible
  
  // =====
  // only a portion of this implemented here, the display portion is elsewhere
  // check for overlap in one direction or the other
  // three stages
  //  check for pre-blank, fill with empty
  //  check for start of media or interior of media
  //  run till end of media or end of window
  //  if window left, fill with empty
  
  // need to maintain time intervals in window (maintained in the scene manager), all windows have same pixel relationships
  //   vector maintains vertical lines for display
  //   different media types will need different ways of caching the view
  
  // ===
  
  // Since this is time based, and the time at the cursor is known, displaying the waveform should be easy
  // What needs to be done now, re-sample the whole thing based upon the given pixelwidth
  // then the actual display is performed elsewhere
  
  struct BuildVertical {
    size_t ix;  // offset into waveform
    size_t nSamplesPerPixel;
    size_t nSamplesToProcess;
    WaveformView::vVertical_t& vVertical;
    WaveformView::vVertical_t::reverse_iterator iterVertical;
    
    void operator() ( int16_t sample ) {  // may want template for this at some point
      if ( 0 == nSamplesToProcess ) {
        vVertical.push_back( WaveformView::Vertical() );  // another structure
        iterVertical = vVertical.rbegin();
        iterVertical->index = ix;
        nSamplesToProcess = nSamplesPerPixel;  // reset count down
      }
      WaveformView::Vertical& v( *iterVertical );
      v.sampleMin = std::min<int16_t>( v.sampleMin, sample );
      v.sampleMax = std::max<int16_t>( v.sampleMax, sample );
      --nSamplesToProcess;
      ++ix;
    }
    BuildVertical( size_t nSamplesPerPixel_, WaveformView::vVertical_t& vVertical_ )
    : nSamplesPerPixel( nSamplesPerPixel_ ), 
      nSamplesToProcess( 0 ), // with 0, does a pre-init to start, always have at least one vertical to show
      vVertical( vVertical_ ),
      ix( 0 )
      { 
      assert( 0 < nSamplesPerPixel );
    };
    ~BuildVertical( void ) {} // any residual stuff to take care of?
  };
  
  m_vVertical.clear();
  if ( 0 != m_pWaveform.get() ) {
    if ( 0 != m_pWaveform->pvSamples ) {
      if ( 0 != m_pWaveform->pvSamples->size() ) {
        // calculate samples per pixel
        // is there a remainder?  
        size_t nSamplesPerPixel = ( ( tdPixelWidth.ticks() * m_pWaveform->SamplesPerSecondNumerator ) / m_pWaveform->SamplesPerSecondDenominator );
        nSamplesPerPixel /= boost::posix_time::seconds ( 1 ).ticks();
        // build vertical vector
        std::for_each( m_pWaveform->pvSamples->begin(), m_pWaveform->pvSamples->end(), BuildVertical( nSamplesPerPixel, m_vVertical ));
      }
    }
  }
  
}
*/

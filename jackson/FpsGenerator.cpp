/* 
 * File:   FpsGenerator.cpp
 * Author: rpb
 * 
 * Created on April 18, 2015, 6:08 PM
 */

#include <vector>
#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
//#include <boost/thread/thread_functors.hpp>
#include <boost/function.hpp>

//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/phoenix/core.hpp>
//#include <boost/phoenix/bind.hpp>

//#include <boost/spirit/include/phoenix_operator.hpp>

//#include <boost/phoenix/bind/bind_member_function.hpp>
//#include <boost/phoenix/core/reference.hpp>
//#include <boost/phoenix/bind/bind_function.hpp>


//#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/duration.hpp>
//#include <boost/chrono/io/duration_io.hpp>
#include <boost/chrono/io/time_point_io.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/foreach.hpp>

#include "FpsGenerator.h"

//typedef boost::chrono::milliseconds ms;
//typedef boost::chrono::microseconds mu;

// 2015/05/10 optimization:
//   maybe use least downcount to determine when next to set the alarm
  
class FrameRate {  // counters are based upon 1ms intervals
public:
  
  typedef std::vector<size_t> vCounts_t;
  
  FrameRate( 
    FpsGenerator::FPS fps, const vCounts_t vCounts, // vCounts - ms down count per frame
    size_t frames, size_t ms,  // for validation of #frames in ms duration
    size_t fr_num, size_t fr_den  // for looking up a frame rate
  ) 
  : m_fps( fps ), m_ixCounts( 0 ), m_nCount( 0 ), m_vCounts( vCounts ),
    m_nFrameRateNumerator( fr_num ), m_nFrameRateDenominator( fr_den )
  { // frame pull down rates
    assert( frames == m_vCounts.size() );
    size_t msSum( 0 );
    BOOST_FOREACH( size_t ms_, m_vCounts ) {
      msSum += ms_;
    }
    assert( ms == msSum );
    m_nCount = m_vCounts[ m_ixCounts ];
  }
  ~FrameRate( void ) {};
  void Check( void ) {
    assert( 0 != m_nCount );
    --m_nCount;
    if ( 0 == m_nCount ) {
      // generate event
      m_signal( m_fps );  // send fps as check that correct events are happening
      // then update counters
      ++m_ixCounts;
      if ( m_vCounts.size() == m_ixCounts ) {
        m_ixCounts = 0;
      }
      m_nCount = m_vCounts[ m_ixCounts ];
    }
  }
  
  FpsGenerator::FPS GetFrameRate( void ) const { return m_fps; }
  bool TestForFrameRate( size_t num, size_t den ) const {
    return ( ( num == m_nFrameRateNumerator ) && ( den == m_nFrameRateDenominator ) );
  }
  
  FpsGenerator::signalFrame_t m_signal;
  
protected:
private:

  const FpsGenerator::FPS m_fps;
  const size_t m_nFrameRateNumerator;
  const size_t m_nFrameRateDenominator;
  
  vCounts_t m_vCounts;
  size_t m_ixCounts;
  size_t m_nCount;
  
};

struct mess{
	void operator()() {
		m_fps.Thread();
	}
	mess(FpsGenerator& fps) : m_fps(fps) {}
	FpsGenerator& m_fps;
};

void FpsGenerator::operator()(void) {
	Thread();
}
  
FpsGenerator::FpsGenerator()
	: m_bStopThread(false), m_bThreadRunning(false),
	//m_thread(boost::phoenix::ref( boost::phoenix::bind( &FpsGenerator::Thread, this ) ) )
	  //m_thread( mess(*this ) )
	//m_thread(boost::ref(*this))
	m_thread( boost::bind( &FpsGenerator::Thread, this ) )
{
  using namespace boost::assign;
  {
    FrameRate::vCounts_t v24xfps;
    for ( unsigned int ix = 1; ix <= 7; ++ix ) {
      v24xfps += 42, 42, 41;
    } // ( 7 * 3 ) entries above + 3 entries below is 24 frames in 1001 ms
    v24xfps += 42, 42, 42;
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps24x, pFrameRate_t( new FrameRate( fps24x, v24xfps, 24, 1001, 24000, 1001 ) ) ) );
  }

  {
    FrameRate::vCounts_t v24fps;
    v24fps += 42, 42, 41;  // 3 frames in 125 ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps24, pFrameRate_t( new FrameRate( fps24, v24fps, 3, 125, 24, 1 ) ) ) );
  }

  {
    FrameRate::vCounts_t v25fps; 
    v25fps += 40;  // 1 frame every 40 ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps25, pFrameRate_t( new FrameRate( fps25, v25fps, 1, 40, 25, 1 ) ) ) );
  }

  {
    FrameRate::vCounts_t v30xfps;
    for ( unsigned int ix = 1; ix <= 9; ++ix ) {
      v30xfps += 33, 33, 34;
    } // ( 9 * 3 ) entries above + 3 entries below is 30 frames in 1001 ms
    v30xfps += 33, 34, 34;
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps30x, pFrameRate_t( new FrameRate( fps30x, v30xfps, 30, 1001, 30000, 1001 ) ) ) );
  }

  {
    FrameRate::vCounts_t v30fps; 
    v30fps += 33, 33, 34;  // 3 frames in 100ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps30, pFrameRate_t( new FrameRate( fps30, v30fps, 3, 100, 30, 1 ) ) ) );
  }

  {
    FrameRate::vCounts_t v48fps; 
    v48fps += 21, 21, 21, 21, 21, 20; // 6 frames in 125ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps48, pFrameRate_t( new FrameRate( fps48, v48fps, 6, 125, 48, 1 ) ) ) );
  }

  {
    FrameRate::vCounts_t v60fps; 
    v60fps += 17, 17, 16; // 3 frames in 50ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps60, pFrameRate_t( new FrameRate( fps60, v60fps, 3, 50, 60, 1 ) ) ) );
  }
  
  {
    FrameRate::vCounts_t v100fps; 
    v100fps += 10; // 1 frame in 10ms
    m_mapFrameRate.insert( 
      mapFrameRate_t::value_type( fps100, pFrameRate_t( new FrameRate( fps100, v100fps, 1, 10, 100, 1 ) ) ) );
  }
  
  }

FpsGenerator::~FpsGenerator( ) {
  m_bStopThread = true;
  m_thread.join();  // wait for thread to end
}

FpsGenerator::FPS FpsGenerator::FindFrameRate( size_t num, size_t den ) {
  FPS fps( fps24 );
  bool bFound( false );
  BOOST_FOREACH( const mapFrameRate_t::value_type& fr, m_mapFrameRate ) {
    bFound |= fr.second->TestForFrameRate( num, den );
    if ( bFound ) {
      fps = fr.first;
      break;
    }
  }
  assert( bFound != false );
  return fps;
}

boost::signals2::connection FpsGenerator::Connect( FPS fps, const slotFrame_t& slot ) {
  mapFrameRate_t::iterator iter = m_mapFrameRate.find( fps );
  assert( m_mapFrameRate.end() != iter );
  return iter->second->m_signal.connect( slot );
}

void FpsGenerator::Thread( void ) {
  
  typedef boost::chrono::high_resolution_clock::time_point tp;
   
  static boost::chrono::duration<int64_t, boost::milli> duration( 1 );  // 1 milliseconds per step
  
  tp tpBase = boost::chrono::high_resolution_clock::now();  // cycle relative to discrete calculations
  
  m_bThreadRunning = true;
  while ( !m_bStopThread ) {
    // generate events
    for ( mapFrameRate_t::iterator iter = m_mapFrameRate.begin(); m_mapFrameRate.end() != iter; ++iter ) {
      iter->second->Check();
    }
    // perform delay to next
    // could move this to prior to Check and provide current time to slots as well
    tp tpNow = boost::chrono::high_resolution_clock::now();
    tp tpNext = tpBase + duration;
    tpBase = tpNext;  // keep track for next cycle through
    if ( tpNext <= tpNow ) { //stutter step, not processing fast enough
      //boost::chrono::duration<int64_t, boost::micro> diff( tpNow - tpNext );
      //std::cout << "FpsGenerator stutter step " << tpNext << ", " << tpNow << std::endl;
//      std::cout << "FpsGenerator stutter step " << ( tpNow - tpNext ) << std::endl;
      //tpNext = tpNow + duration;
      // making the assumption we can catch up
    }
    else {
      boost::this_thread::sleep_until( tpNext );
    }
  }
}
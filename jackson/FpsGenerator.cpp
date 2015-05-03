/* 
 * File:   FpsGenerator.cpp
 * Author: rpb
 * 
 * Created on April 18, 2015, 6:08 PM
 */

#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/duration.hpp>

#include "FpsGenerator.h"

//typedef boost::chrono::milliseconds ms;
//typedef boost::chrono::microseconds mu;
  
class DownCount {
public:
  
  static const size_t nSourceCounts = 3;
  DownCount( FpsGenerator::FPS fps, const size_t* pCounts ) 
  : m_fps( fps ), m_ixSourceCounts( 0 ), m_nCount( 0 )
  { // frame pull down rates
    m_rSourceCounts[ 0 ] = pCounts[ 0 ];
    m_rSourceCounts[ 1 ] = pCounts[ 1 ];
    m_rSourceCounts[ 2 ] = pCounts[ 2 ];
    m_nCount = m_rSourceCounts[ m_ixSourceCounts ];
  }
  ~DownCount( void ) {};
  void Check( void ) {
    assert( 0 != m_nCount );
    --m_nCount;
    if ( 0 == m_nCount ) {
      // generate event
      m_signal( m_fps );  // send fps as check that correct events are happening
      // then update counters
      ++m_ixSourceCounts;
      if ( nSourceCounts == m_ixSourceCounts ) {
        m_ixSourceCounts = 0;
      }
      m_nCount = m_rSourceCounts[ m_ixSourceCounts ];
    }
  }
  
  FpsGenerator::signalFrame_t m_signal;
  
protected:
private:
  size_t m_rSourceCounts[ nSourceCounts ];
  size_t m_ixSourceCounts;
  size_t m_nCount;
  FpsGenerator::FPS m_fps;
};
  
FpsGenerator::FpsGenerator( ) 
: m_bStopThread( false ), m_bThreadRunning( false ), m_thread( boost::phoenix::bind( &FpsGenerator::Thread, this ) )
{
  const size_t cnt24fps[DownCount::nSourceCounts] = { 8, 8, 9 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps24, pDownCount_t( new DownCount( fps24, cnt24fps ) ) ) );

  const size_t cnt25fps[DownCount::nSourceCounts] = { 8, 8, 8 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps25, pDownCount_t( new DownCount( fps25, cnt25fps ) ) ) );

  const size_t cnt30fps[DownCount::nSourceCounts] = { 7, 7, 6 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps30, pDownCount_t( new DownCount( fps30, cnt30fps ) ) ) );

  const size_t cnt48fps[DownCount::nSourceCounts] = { 4, 4, 5 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps48, pDownCount_t( new DownCount( fps48, cnt48fps ) ) ) );

  const size_t cnt60fps[DownCount::nSourceCounts] = { 3, 3, 4 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps60, pDownCount_t( new DownCount( fps60, cnt60fps ) ) ) );
  
  const size_t cnt100fps[DownCount::nSourceCounts] = { 2, 2, 2 };
  m_mapDownCount.insert( mapDownCount_t::value_type( fps100, pDownCount_t( new DownCount( fps100, cnt100fps ) ) ) );
  
  }

FpsGenerator::~FpsGenerator( ) {
  m_bStopThread = true;
  m_thread.join();  // wait for thread to end
}

boost::signals2::connection FpsGenerator::Connect( FPS fps, const slotFrame_t& slot ) {
  mapDownCount_t::iterator iter = m_mapDownCount.find( fps );
  assert( m_mapDownCount.end() != iter );
  return iter->second->m_signal.connect( slot );
}

void FpsGenerator::Thread( void ) {
  
  typedef boost::chrono::high_resolution_clock::time_point tp;
   
  static boost::chrono::duration<int64_t, boost::milli> duration( 5 );  // 5 milliseconds per step
  
  tp tpBase = boost::chrono::high_resolution_clock::now();  // cycle relative to discrete calculations
  
  m_bThreadRunning = true;
  while ( ! m_bStopThread ) {
    // generate events
    for ( mapDownCount_t::iterator iter = m_mapDownCount.begin(); m_mapDownCount.end() != iter; ++iter ) {
      iter->second->Check();
    }
    // perform delay to next
    // could move this to prior to Check and provide current time to slots as well
    tp tpNow = boost::chrono::high_resolution_clock::now();
    tp tpNext = tpBase + duration;
    if ( tpNext < tpNow ) { //stutter step, not processing fast enough
      tpNext = tpNow + duration;
      std::cout << "FpsGenerator stutter step" << std::endl;
    }
    tpBase = tpNext;  // keep track for next cycle through
    boost::this_thread::sleep_until( tpNext );
  }
}
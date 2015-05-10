/* 
 * File:   FpsGenerator.cpp
 * Author: rpb
 * 
 * Created on April 18, 2015, 6:08 PM
 */

#include <vector>

#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/duration.hpp>

#include <boost/assign/std/vector.hpp>

#include "FpsGenerator.h"

//typedef boost::chrono::milliseconds ms;
//typedef boost::chrono::microseconds mu;
  
class DownCount {  // counters are based upon 1ms intervals
public:
  
  typedef std::vector<size_t> vCounts_t;
  
  DownCount( FpsGenerator::FPS fps, const vCounts_t vCounts ) 
  : m_fps( fps ), m_ixCounts( 0 ), m_nCount( 0 ), m_vCounts( vCounts )
  { // frame pull down rates
    m_nCount = m_vCounts[ m_ixCounts ];
  }
  ~DownCount( void ) {};
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
  
  FpsGenerator::signalFrame_t m_signal;
  
protected:
private:
  vCounts_t m_vCounts;
  size_t m_ixCounts;
  size_t m_nCount;
  FpsGenerator::FPS m_fps;
};
  
FpsGenerator::FpsGenerator( ) 
: m_bStopThread( false ), m_bThreadRunning( false ), m_thread( boost::phoenix::bind( &FpsGenerator::Thread, this ) )
{
  using namespace boost::assign;
  {
    DownCount::vCounts_t v24xfps;
    for ( unsigned int ix = 1; ix <= 7; ++ix ) {
      v24xfps.push_back( 42 ); v24xfps.push_back( 42 ); v24xfps.push_back( 41 );
    } // ( 7 * 3 ) entries above + 3 entries below  is 24 frames
    v24xfps.push_back( 42 ); v24xfps.push_back( 42 ); v24xfps.push_back( 42 ); // 24 frames in 1001 ms
    assert( 24 == v24xfps.size());
    m_mapDownCount.insert( mapDownCount_t::value_type( fps24x, pDownCount_t( new DownCount( fps24x, v24xfps ) ) ) );
  }

  {
    DownCount::vCounts_t v24fps;
    v24fps += 42, 42, 41;  // 3 frames in 125 ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps24, pDownCount_t( new DownCount( fps24, v24fps ) ) ) );
  }

  {
    DownCount::vCounts_t v25fps; 
    v25fps += 40;  // 1 frame every 40 ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps25, pDownCount_t( new DownCount( fps25, v25fps ) ) ) );
  }

  {
    DownCount::vCounts_t v30xfps;
    for ( unsigned int ix = 1; ix <= 9; ++ix ) {
      v30xfps.push_back( 33 ); v30xfps.push_back( 33 ); v30xfps.push_back( 34 );
    } // ( 9 * 3 ) entries above + 3 entries below  is 30 frames
    v30xfps.push_back( 33 ); v30xfps.push_back( 34 ); v30xfps.push_back( 34 ); // 30 frames in 1001 ms
    assert( 30 == v30xfps.size());
    m_mapDownCount.insert( mapDownCount_t::value_type( fps30x, pDownCount_t( new DownCount( fps30x, v30xfps ) ) ) );
  }

  {
    DownCount::vCounts_t v30fps; 
    v30fps += 33, 33, 34;  // 3 frames in 100ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps30, pDownCount_t( new DownCount( fps30, v30fps ) ) ) );
  }

  {
    DownCount::vCounts_t v48fps; 
    v48fps += 21, 21, 21, 21, 21, 20; // 6 frames in 125ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps48, pDownCount_t( new DownCount( fps48, v48fps ) ) ) );
  }

  {
    DownCount::vCounts_t v60fps; 
    v60fps += 17, 17, 16; // 3 frames in 50ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps60, pDownCount_t( new DownCount( fps60, v60fps ) ) ) );
  }
  
  {
    DownCount::vCounts_t v100fps; 
    v100fps += 10; // 1 frame in 10ms
    m_mapDownCount.insert( mapDownCount_t::value_type( fps100, pDownCount_t( new DownCount( fps100, v100fps ) ) ) );
  }
  
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
   
  static boost::chrono::duration<int64_t, boost::milli> duration( 1 );  // 1 milliseconds per step
  
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
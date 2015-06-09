/* 
 * File:   AudioQueue.h
 * Author: rpb
 *
 * Created on May 23, 2015, 12:37 PM
 */

#pragma once

#include <deque>
#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/strict_lock.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/shared_ptr.hpp>

// typically crossing two background threads:  file processing, sound output, so sync is necessary

template <typename SampleType>
class AudioQueue: public boost::basic_lockable_adapter<boost::mutex> {
public:
  
  typedef std::deque<SampleType> deque_t;
  typedef boost::shared_ptr<AudioQueue<SampleType> > pAudioQueue_t;
  
  AudioQueue( void );
  virtual ~AudioQueue( );
  
  // attenuator is range of -15 to 4 (buck / boost)
  void SetAttenuator( int st ) { 
    assert( -18 <= st );  assert( 4 >= st );  
    m_ixAttenuator = 4 - st; 
    m_attenuator = m_vAttenuator[ m_ixAttenuator ];
  }
  SampleType GetAttenuator( void ) const { return m_ixAttenuator; }
  
  void AddSamples( const uint64_t n, const SampleType*, boost::strict_lock<AudioQueue<SampleType> >& ); 
  void AddSamples( const uint64_t n, const SampleType* );
  
    // blocks when no data available
  bool RemoveSamplesSync( const uint64_t nRequested, uint64_t& nReturned, SampleType*, boost::strict_lock<AudioQueue<SampleType> >& );
  bool RemoveSamplesSync( const uint64_t nRequested, uint64_t& nReturned, SampleType* );
  
  typename deque_t::size_type GetSize( void ) const { return m_dequeSamples.size(); }
  void SetDone( void );  // no more queued data to be had
  
  inline SampleType RemoveSampleAsyncChecked( boost::strict_lock<AudioQueue<SampleType> >&guard ) { // externally locked
    assert( guard.owns_lock( this ) );
    SampleType sample {};
    if ( 0 != m_dequeSamples.size() ) {
      sample = RemoveSample();
    }
    return sample;
  }
  
  inline SampleType RemoveSampleAsyncUnChecked( boost::strict_lock<AudioQueue<SampleType> >&guard ) { // externally locked
    assert( guard.owns_lock( this ) );
    return RemoveSample();
  }
  
protected:
private:
  
  bool m_bDone;
  boost::condition_variable m_condQueueHasData;
  deque_t m_dequeSamples;

  size_t m_ixAttenuator;  
  std::vector<SampleType> m_vAttenuator;
  SampleType m_attenuator;
  
  inline SampleType RemoveSample( void ) {
    SampleType sample = m_dequeSamples.front();
    m_dequeSamples.pop_front();
    if ( 1 == m_attenuator ) {
      return sample;
    }
    else {
      if ( 0 < m_attenuator ) {
        return ( (int32_t)sample * (int32_t)m_attenuator ) / 4;
      }
      else {
        return ( (int32_t)sample * 2 ) / -m_attenuator;
      }
    }
    
  }
  

};

template <typename SampleType>
AudioQueue<SampleType>::AudioQueue( ): m_bDone( false ), m_ixAttenuator( 4 ), m_attenuator( (SampleType) 1 ) {
  using namespace boost::assign;
  m_vAttenuator += 8, 7, 6, 5, 1, -2, -3, -4, -5, -6, -7, -10, -12, -16, -22, -40, -80, -200, -480, -1024, -2048, -4096, -8192, -16384, -32768;
  m_attenuator = m_vAttenuator[ m_ixAttenuator ];
  assert( (SampleType) 1 == m_attenuator );
}

template <typename SampleType>
AudioQueue<SampleType>::~AudioQueue( ) {
}

template <typename SampleType>  
bool AudioQueue<SampleType>::RemoveSamplesSync( const uint64_t nRequested, uint64_t& nReturned, SampleType* pSamples, boost::strict_lock<AudioQueue<SampleType> >& guard) {
  assert( guard.owns_lock( *this ) );
  while ( !m_bDone && ( nRequested > m_dequeSamples.size() ) ) {  // how to break out of this when no more data??
    m_condQueueHasData.wait( lock );
  }
  
  nReturned = 0;
  typename deque_t::size_type nToGet( std::min<SampleType>( m_dequeSamples.size(), nRequested ) );
  for ( typename deque_t::size_type cnt = 1; cnt <= nToGet; ++cnt ) {
    *pSamples = RemoveSample();
    ++nReturned;
  }
}

template <typename SampleType>
bool AudioQueue<SampleType>::RemoveSamplesSync( const uint64_t nRequested, uint64_t& nReturned, SampleType* pSamples) {
  boost::strict_lock<AudioQueue<SampleType> > guard( this );  // internally locked
  RemoveSamplesSync( nRequested, nReturned, pSamples, guard );
}

template <typename SampleType>
void AudioQueue<SampleType>::AddSamples( const uint64_t n, const SampleType* pSamples, boost::strict_lock<AudioQueue<SampleType> >& guard ) { // externally locked
  assert( guard.owns_lock( this ) );
  for ( uint64_t ix = 0; ix < n; ++ix ) {
    m_dequeSamples.push_back( *pSamples );
    ++pSamples;
  }
  m_condQueueHasData.notify_one();
}

template <typename SampleType>
void AudioQueue<SampleType>::AddSamples( const uint64_t n, const SampleType* pSamples) {
  boost::strict_lock<AudioQueue<SampleType> > guard( *this );  // internally locked
  AddSamples( n, pSamples, guard );
}

template <typename SampleType>
void AudioQueue<SampleType>::SetDone( void ) {
  // is a lock required for this?
  m_bDone = true;
  m_condQueueHasData.notify_one();
}

// http://www.boost.org/doc/libs/1_58_0/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref
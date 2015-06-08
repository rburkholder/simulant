/* 
 * File:   Audio.h
 * Author: rpb
 *
 * Created on May 23, 2015, 12:33 AM
 */

#pragma once

#include <vector>
#include <algorithm> 

#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/strict_lock.hpp>
#include <boost/signals2.hpp>

#include <RtAudio.h>

#include "AudioQueue.h"

template <typename SampleType, typename OutputType>  // maybe sometime can mix types
class ChannelMixer: public boost::basic_lockable_adapter<boost::mutex> {
public:
  
  typedef typename AudioQueue<SampleType>::pAudioQueue_t pAudioQueue_t;
  typedef boost::shared_ptr<ChannelMixer<SampleType, OutputType> > pChannelMixer_t;
  
  ChannelMixer( void ) {};
  virtual ~ChannelMixer( void ) {};
  
  void Attach( pAudioQueue_t pAudioQueue, boost::strict_lock<ChannelMixer<SampleType,OutputType> >& guard ) {
    assert( guard.owns_lock( this ) );
    m_vpAudioQueue.push_back( pAudioQueue );
  }
  
  void Detach( pAudioQueue_t pAudioQueue, boost::strict_lock<ChannelMixer<SampleType,OutputType> >& guard ) {
    assert( guard.owns_lock( this ) );
    typename vpAudioQueue_t::const_iterator iter = std::find( m_vpAudioQueue.begin(), m_vpAudioQueue.end(), pAudioQueue );
    if ( m_vpAudioQueue.end() != iter ) {
      m_vpAudioQueue.erase( iter );
    }
  }
  
  template<typename Function>
  void Dequeue( unsigned int nSamples, Function function, boost::strict_lock<ChannelMixer<SampleType,OutputType> >& guard ) {
    //std::cout << "ChannelMixer::Dequeue" << std::endl;
    assert( guard.owns_lock( this ) );
    // lock each of the queues
    for ( typename vpAudioQueue_t::iterator iter = m_vpAudioQueue.begin(); iter != m_vpAudioQueue.end(); ++iter ) {
      pLock_t pLock( new holder( **iter ) );
      vpLocks.push_back( pLock );
    }
    // pull out the data
    typename vpAudioQueue_t::size_type nQueues( m_vpAudioQueue.size() );
    for ( unsigned int ix = 0; ix < nSamples; ++ix ) {
      // pretty heavy duty calls, can it be simplified?
      OutputType value {};
      if ( 0 == nQueues ) {
        function( value );
      }
      else {
        typename vpLock_t::iterator iterLocks = vpLocks.begin();
        for ( typename vpAudioQueue_t::iterator iterQueues = m_vpAudioQueue.begin(); iterQueues != m_vpAudioQueue.end(); ++iterQueues ) {
          value += (*iterQueues)->RemoveSampleAsyncChecked( (*iterLocks)->guard );
          ++iterLocks;
        }
        //function( value / (OutputType)m_vpAudioQueue.size() );
        function( value );  // don't average out, just pass the mix, individual components are adjusted instead
      }
    }
    // locks go away with clearing of vector
    vpLocks.clear();  // decided to keep around for a bit of improvement on memory management, maybe do array instead on stack
  }
  
protected:
private:
  
  typedef std::vector<pAudioQueue_t> vpAudioQueue_t;
  vpAudioQueue_t m_vpAudioQueue;
  
  struct holder {
    holder( AudioQueue<SampleType>& aq ): guard( aq ) {};
    boost::strict_lock<AudioQueue<SampleType> > guard;
  };
  
  typedef boost::shared_ptr<holder> pLock_t;
  typedef std::vector<pLock_t> vpLock_t;
  vpLock_t vpLocks;
  
  
};

// =================================

class Audio {
public:
  
  typedef int16_t SampleType;
  //typedef int32_t OutputType;
  typedef float OutputType;
  typedef ChannelMixer<SampleType,OutputType> ChannelMixer_t;  // needs to be more flexible when more types are available
  typedef ChannelMixer_t::pAudioQueue_t pAudioQueue_t;
  
  typedef boost::signals2::signal<void (unsigned int)> signalFramesProcessed_t;
  typedef signalFramesProcessed_t::slot_type slotFramesProcessed_t;
  
  signalFramesProcessed_t m_signalFramesProcessed; // invoked from callback HandleSampleRequest
  
  Audio( );
  virtual ~Audio( );
  
  void Attach( unsigned int n, pAudioQueue_t pAudioQueue );
  void Detach( unsigned int n, pAudioQueue_t pAudioQueue );
  
  void Play( void ) { m_audio.startStream(); }
  void Abort( void ) { m_audio.abortStream(); }
  void Stop( void ) { m_audio.stopStream(); }
  
protected:
private:
  
  unsigned int m_nActiveChannels;
  
  RtAudio m_audio;
  
  typedef ChannelMixer_t::pChannelMixer_t pChannelMixer_t;
  typedef std::vector<pChannelMixer_t> vChannelMixer_t;
  vChannelMixer_t m_vcm; // try basic channel mixer
  
  static int HandleSampleRequest( void* pOutput, void* pInput, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* pThis );
};


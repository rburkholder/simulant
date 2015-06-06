/* 
 * File:   Audio.cpp
 * Author: rpb
 * 
 * Created on May 23, 2015, 12:33 AM
 */

#include <cassert>

#include <iostream>
#include <string>
//#include <boost/phoenix/bind/bind_member_function.hpp>
//#include <boost/phoenix/bind/bind_function.hpp>
//#include <boost/phoenix/bind/bind_function_object.hpp>
//#include <boost/phoenix/core/argument.hpp>

#include "Audio.h"

struct UpdateOutputBuffer {
  void operator()( int32_t value ) {  // converts 32 back to 16
    **pOutput = (int16_t) value;
    ++(*pOutput);
  }
  UpdateOutputBuffer( int16_t** pOutput_ ): pOutput( pOutput_ ) {}
  int16_t** pOutput;
};

int Audio::HandleSampleRequest( void* pOutput, void* pInput, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* pThis ) {
  
  assert( 0 == pInput );
  assert( 0 != pOutput );
  
  //std::cout << "Audio::HandleSampleRequest" << std::endl;
  
  if ( RTAUDIO_INPUT_OVERFLOW == status ) std::cout << "audio stream overflow" << std::endl;
  if ( RTAUDIO_OUTPUT_UNDERFLOW == status ) std::cout << "audio stream underflow" << std::endl;
  
  Audio* pAudio( reinterpret_cast<Audio*>( pThis ) );
  int16_t* pSamples( reinterpret_cast<int16_t*>( pOutput ) );
  
  //for ( size_t n = 0; n < nFrames * pAudio->m_nActiveChannels; ++n ) {
    //*pSamples = 0;  // assign default of zero for now, no sound
    //++pSamples;
  //}
  
  for ( vChannelMixer_t::iterator iter = pAudio->m_vcm.begin(); iter != pAudio->m_vcm.end(); ++iter ) {
    //namespace args = boost::phoenix::arg_names;
    boost::strict_lock<ChannelMixer<int16_t,int32_t> > guard( **iter );
    //(*iter)->Dequeue( nFrames, boost::phoenix::bind( &UpdateOutputBuffer, args::arg1, args::arg2 ), guard );
    (*iter)->Dequeue( nFrames, UpdateOutputBuffer( &pSamples ), guard );
  }
  
  return 0; // normal stream operation
  //return 1; // stop and drain
  //return 2; // abort 
}

Audio::Audio( void ): m_nActiveChannels( 2 ) {
  
  unsigned int nDevices = m_audio.getDeviceCount();
  assert( 0 != nDevices );

  bool bDeviceAvailable( false );
  unsigned int ixFirstAvailableDevice( 0 );
  
  // Scan through devices for various capabilities
  RtAudio::DeviceInfo info;
  
  for ( unsigned int ix = 0; ix < nDevices; ++ix ) {
    info = m_audio.getDeviceInfo( ix );
    //if ( info.probed == true ) {
      // Print, for example, the maximum number of output channels for each device
      std::cout 
        << info.probed
        << " device " << ix
        << ": " << info.name
        << " maximum output channels = " << info.outputChannels 
        << ", is default = " << info.isDefaultOutput
        << ", output = " << info.nativeFormats
        ;
      if (!bDeviceAvailable && info.probed ) {
        bDeviceAvailable = true;
        ixFirstAvailableDevice = ix;
        std::cout << " << to be opened";
      }
      std::cout << std::endl;
    //}
  }
  
  // RTAUDIO_SINT16 | RTAUDIO_SINT32 | RTAUDIO_FLOAT32
  RtAudio::StreamParameters parameters;
  //parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.deviceId = ixFirstAvailableDevice; // based on text output above, may need to change
  parameters.nChannels = m_nActiveChannels;
  
  unsigned int sampleRate = 44100;
  unsigned int bufferSamples = 256; // 256 sample frames
  
  RtAudio::StreamOptions options;
  // so can lock/process/unlock channels individually
  // but leads to frame sync issues if not all frames installed at the same time
  // so maybe we need to run interleaved
  // not unless the underlying takes care of that
  // so to sync separate buffers will need to play with the stream start stop calls rather than let them run continuously
  // or pause the stream while starting the buffers, use the pause rather then the abort,to let things run in the background
  options.flags = RTAUDIO_NONINTERLEAVED;
  //options.flags = 0;  // default to interleaved

  if (!bDeviceAvailable) {
    std::cout << "no audio device available" << std::endl;
  }
  else {
    try {
      std::cout << "Audio Opening" << std::endl;
      m_audio.openStream(&parameters, 0, RTAUDIO_SINT16, sampleRate, &bufferSamples, &HandleSampleRequest, this, &options);
      std::cout << "Audio Opened" << std::endl;
    }
    catch (RtAudioError& e) {
      std::cout << "Audio Error on Open:  " << e.getMessage() << std::endl;
    }

    //m_vcm.resize( m_nActiveChannels );
    for (size_t ix = 0; ix < m_nActiveChannels; ++ix) {
      pChannelMixer_t p(new ChannelMixer_t);
      m_vcm.push_back(p);
    }
  }
  
  //m_audio.startStream(); // may need to change this to a global play/pause/stop/abort button
}

Audio::~Audio( ) {
  //m_audio.abortStream(); // may need to change this to a global play/pause/stop/abort button
  std::cout << "Audio Closed" << std::endl;
  m_audio.closeStream();
}

void Audio::Attach( unsigned int n, pAudioQueue_t pAudioQueue ) {
  assert( n < m_nActiveChannels );
  pChannelMixer_t pChannelMixer( m_vcm[ n ] );
  boost::strict_lock<ChannelMixer_t> guard( *pChannelMixer );
  pChannelMixer->Attach( pAudioQueue, guard );
}

void Audio::Detach( unsigned int n, pAudioQueue_t pAudioQueue ) {
  assert( n < m_nActiveChannels );
  pChannelMixer_t pChannelMixer( m_vcm[ n ] );
  boost::strict_lock<ChannelMixer_t> guard( *pChannelMixer );
  pChannelMixer->Detach( pAudioQueue, guard );
}

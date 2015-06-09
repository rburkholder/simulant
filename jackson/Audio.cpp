/* 
 * File:   Audio.cpp
 * Author: rpb
 * 
 * Created on May 23, 2015, 12:33 AM
 */

#include <cassert>

#include <iostream>
//#include <string>

#include "Audio.h"

struct UpdateOutputBuffer {
  void operator()( Audio::OutputType value ) {  // converts 32 back to 16
    **pOutput = (Audio::OutputType) value;
    ++(*pOutput);
  }
  UpdateOutputBuffer( Audio::OutputType** pOutput_ ): pOutput( pOutput_ ) {}
  Audio::OutputType** pOutput;
};

int Audio::HandleSampleRequest( void* pOutput, void* pInput, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* pThis ) {
  
  assert( 0 == pInput );
  assert( 0 != pOutput );
  
  //std::cout << "Audio::HandleSampleRequest" << std::endl;
  
  if ( RTAUDIO_INPUT_OVERFLOW == status ) std::cout << "audio stream overflow" << std::endl;
  if ( RTAUDIO_OUTPUT_UNDERFLOW == status ) std::cout << "audio stream underflow" << std::endl;
  
  Audio* pAudio( reinterpret_cast<Audio*>( pThis ) );
  OutputType* pSamples( reinterpret_cast<OutputType*>( pOutput ) );
  
  for ( vChannelMixer_t::iterator iter = pAudio->m_vcm.begin(); iter != pAudio->m_vcm.end(); ++iter ) {
    //namespace args = boost::phoenix::arg_names;
    boost::strict_lock<ChannelMixer<SampleType,OutputType> > guard( **iter );
    //(*iter)->Dequeue( nFrames, boost::phoenix::bind( &UpdateOutputBuffer, args::arg1, args::arg2 ), guard );
    (*iter)->Dequeue( nFrames, UpdateOutputBuffer( &pSamples ), guard );
  }
  
  pAudio->m_signalFramesProcessed( nFrames );
  
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

  std::vector<RtAudio::Api> vApi;
  RtAudio::getCompiledApi( vApi );

  int i1 = RtAudio::WINDOWS_ASIO;
  int i2 = RtAudio::WINDOWS_DS;
  
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
        << ", output = ";
      if ( RTAUDIO_SINT8 & info.nativeFormats ) std::cout << "SINT8 ";  // = 0x1;  // 8-bit signed integer.
      if ( RTAUDIO_SINT16 & info.nativeFormats ) std::cout << "SINT16 ";// = 0x2;  // 16-bit signed integer.
      if ( RTAUDIO_SINT24 & info.nativeFormats ) std::cout << "SINT24 ";// = 0x4;  // 24-bit signed integer.
      if ( RTAUDIO_SINT32 & info.nativeFormats ) std::cout << "SINT32 ";// = 0x8;  // 32-bit signed integer.
      if ( RTAUDIO_FLOAT32 & info.nativeFormats ) std::cout << "FLT32 ";// = 0x10; // Normalized between plus/minus 1.0.
      if ( RTAUDIO_FLOAT64 & info.nativeFormats ) std::cout << "FLT64 ";// = 0x20; // Normalized between plus/minus 1.0
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
  //parameters.deviceId = 1; // based on text output above, may need to change
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
      //m_audio.openStream(&parameters, 0, RTAUDIO_SINT32, sampleRate, &bufferSamples, &HandleSampleRequest, this, &options);
      m_audio.openStream(&parameters, 0, RTAUDIO_SINT16, sampleRate, &bufferSamples, &HandleSampleRequest, this, &options);
      //m_audio.openStream(&parameters, 0, RTAUDIO_FLOAT32, sampleRate, &bufferSamples, &HandleSampleRequest, this, &options);
      std::cout << "Audio Opened with buffer size: " << bufferSamples << std::endl;
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

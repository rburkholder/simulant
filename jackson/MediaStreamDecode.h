/* 
 * File:   DecodeStream.h
 * Author: rpb
 *
 * Created on May 9, 2015, 4:02 PM
 */

#pragma once

#include <list>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "RawImage.h"
#include "common.h"

class MediaStreamDecode {
public:
  
  struct FrameInfo {
    int64_t nAudioFrame; // current audio frame number
    int64_t ttlAudioFrames;
    int64_t nVideoFrame; // current video frame number, 1 ... n
    int64_t ttlVideoFrames;
    int64_t pts;
    int64_t pkt_pts;  // 1001, 2002, 3003, ... 
    int64_t pkt_dts;
    FrameInfo(): nAudioFrame( 0 ), ttlAudioFrames( 0 ), nVideoFrame( 0 ), ttlVideoFrames( 0 ), pts( 0 ), pkt_pts( 0 ), pkt_dts( 0 ) {}
  };
  
  typedef RawImage::pRawImage_t pRawImage_t;
  typedef boost::signals2::signal<void (pRawImage_t, const structTimeSteps&, const FrameInfo&)> signalImageReady_t;
  typedef signalImageReady_t::slot_type slotImageReady_t;
  
  typedef boost::signals2::signal<void ()> signalDecodeDone_t;
  typedef signalDecodeDone_t::slot_type slotDecodeDone_t;
  
  MediaStreamDecode( void );
  virtual ~MediaStreamDecode( void );
  
  boost::signals2::connection ConnectImageReady( const slotImageReady_t& slot ) {
    return m_signalImageReady.connect( slot );
  }
  
  boost::signals2::connection ConnectDecodeDone( const slotDecodeDone_t& slot ) {
    return m_signaDecodeDone.connect( slot );
  }
  
  AVRational GetAudioFrameRate( void ) const { return m_pFormatContext->streams[m_ixBestAudioStream]->avg_frame_rate; }
  AVRational GetVideoFrameRate( void ) const { return m_pFormatContext->streams[m_ixBestVideoStream]->avg_frame_rate; }
  
  uint64_t GetTotalVideoFrames( void ) const { return m_fi.ttlVideoFrames; }
  uint64_t GetTotalAudioFrames( void ) const { return m_fi.ttlAudioFrames; }
  
  bool Open( const std::string& sFile ); // return some initial info
  void EmitStats( void );
  void Play( void );
  void Rewind( void );
  //void Seek( ... );
  void Stop( void );
  void Pause( void );
  void Resume( void );
  
  void Close( void );  // need to ensure background thread is finished
  
private:
  
  struct StreamInfo {
    AVCodec* pCodec;
    AVCodecContext* pCodecContext;
    StreamInfo( void ) : pCodec( 0 ), pCodecContext( 0 ) {}
    ~StreamInfo( void ) {
      if ( 0 != pCodecContext ) {
        avcodec_close( pCodecContext );
        avcodec_free_context( &pCodecContext );
        pCodecContext = 0;
        pCodec = 0;
      }
    }
  };
  
  enum StateStream { 
    EClosed, 
    EError, 
    EOpenRequest, 
    EOpen, 
    EPlayRequest, 
    EPlaying, 
    EPauseRequest, 
    EPaused, 
    EResumeRequest, 
    EStopRequest, 
    EStopped  // implies start from beginning
  };
  
  typedef std::vector<StreamInfo> vStreamInfo_t;
  vStreamInfo_t m_vStreamInfo;
  
  FrameInfo m_fi;

  AVFormatContext* m_pFormatContext;
  
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  enum ThreadRequest { ThreadRequestFilled, ThreadRequestPlay, ThreadRequestPause, ThreadRequestStop };
  enum ThreadState { ThreadStatePlaying, ThreadStatePaused, ThreadStateStopping, ThreadStateStopped };
  
  boost::condition_variable m_cvDecode;
  boost::mutex m_mutexDecode;
  bool m_bProcessStreamWait;
  StateStream m_stateStream;
  ThreadRequest m_stateThreadRequest;
  ThreadState m_stateThread;
  
  signalImageReady_t m_signalImageReady;
  signalDecodeDone_t m_signaDecodeDone;
  
  int m_ixBestAudioStream;
  int m_ixBestVideoStream;
  
  structTimeSteps m_ts;
  
  void ProcessStream( size_t ixAudio, size_t ixVideo );  // background thread
  void ProcessVideoFrame( AVCodecContext* pContext, AVFrame* pFrame, structTimeSteps perf );
  void HandleFrameTransformToImage( AVFrame* pRgb, uint8_t* buf, structTimeSteps perf, int srcX, int srcY );
  
};


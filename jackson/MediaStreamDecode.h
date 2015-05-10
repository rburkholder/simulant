/* 
 * File:   DecodeStream.h
 * Author: rpb
 *
 * Created on May 9, 2015, 4:02 PM
 */

#pragma once

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <wx/wx.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "common.h"

class MediaStreamDecode {
public:
  
  typedef boost::shared_ptr<wxImage> pImage_t;
  typedef boost::signals2::signal<void (pImage_t, const structTimeSteps&)> signalImageReady_t;
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
  
  bool Open( const std::string& sFile );
  void EmitStats( void );
  void Play( void );
  void Rewind( void );
  //void Seek( ... );
  void Stop( void );
  void PauseResume( void );
  
  void Close( void );
  
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
  
  typedef std::vector<StreamInfo> vStreamInfo_t;
  vStreamInfo_t m_vStreamInfo;

  AVFormatContext* m_pFormatContext;
  
  bool m_bInUse;
  
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  signalImageReady_t m_signalImageReady;
  signalDecodeDone_t m_signaDecodeDone;
  
  int m_ixBestAudioStream;
  int m_ixBestVideoStream;
  
  structTimeSteps m_ts;
  
  void ProcessStream( size_t ixAudio, size_t ixVideo );  // background thread
  void HandleOnFrame( AVCodecContext* pContext, AVFrame* pFrame, structTimeSteps perf );
  void HandleFrameTransformToImage( AVFrame* pRgb, uint8_t* buf, structTimeSteps perf, int srcX, int srcY );
  
  void Workers( void );  // background processing of video
  

};


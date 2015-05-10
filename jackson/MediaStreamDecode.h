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

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "common.h"

class MediaStreamDecode {
public:
  MediaStreamDecode( );
  virtual ~MediaStreamDecode( );
  void EmitStats( const std::string& sFile );
  void Play( const std::string& sFile );
private:
  
  bool m_bInUse;
  
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  boost::signals2::connection m_connectionFrameTrigger;
  
  AVFormatContext* m_pFormatContext;
  
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

  int m_ixBestAudioStream;
  int m_ixBestVideoStream;
  
//  void ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder );  // background thread?
  void ProcessStream( size_t ixAudio, size_t ixVideo );  // background thread
  void HandleOnFrame( AVCodecContext* pContext, AVFrame* pFrame, void* user, structTimeSteps perf );
  void HandleFrameTransformToImage( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY );
  
  void Workers( void );  // background processing of video
  
  bool Open( const std::string& sFile );
  void EmitStats( void );
  void Close( void );

};


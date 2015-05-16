/* 
 * File:   DecodeStream.cpp
 * Author: rpb
 * 
 * Created on May 9, 2015, 4:02 PM
 */

#include <stdexcept>

//#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <boost/chrono/chrono_io.hpp>

#include <boost/foreach.hpp>

extern "C" {
#include <libavutil/dict.h>
#include <libswscale/swscale.h>
//#include <libavutil/log.h>
}

#include <wx/rawbmp.h>

#include "MediaStreamDecode.h"

//void log(void* ptr, int level, const char* fmt, va_list vl) {
//  std::array<char,256> line;
//  int printBuffer = 1;
//  av_log_format_line(ptr, level, fmt, vl, line.data(), line.size(), &printBuffer);
  //OutputDebugStringA(line.data());
//  std::cout << line.data() << std::endl;
//}

MediaStreamDecode::MediaStreamDecode( )
: m_pFormatContext( 0 ), m_ixBestVideoStream( 0 ), m_ixBestAudioStream( 0 ),
  m_bProcessStreamWait( false ), m_stateStream( EClosed ),
  m_stateThreadRequest( ThreadRequestFilled ), m_stateThread( ThreadStateStopped )
{
  
    // workers for the movie action
  m_pWork = new boost::asio::io_service::work(m_Srvc);  // keep the asio service running 
  for ( std::size_t ix = 0; ix < 2; ix++ ) {  // one for each of ProcessStream, HandleFrameTransformToImage
    m_threadsWorkers.create_thread( boost::phoenix::bind( &boost::asio::io_service::run, &m_Srvc ) );
  }
  
  av_log_set_level(AV_LOG_DEBUG);  //AV_LOG_VERBOSE AV_LOG_INFO AV_LOG_WARNING AV_LOG_ERROR AV_LOG_FATAL
  
}

MediaStreamDecode::~MediaStreamDecode( ) {
  Stop();
  Close();
  delete m_pWork;
  m_pWork = 0;
  m_threadsWorkers.join_all();  // wait for processing to end
}

bool MediaStreamDecode::Open( const std::string& sFile ) {
  
  switch ( m_stateStream ) {
    case EStopped:
      Close();
      break;
    case EError:
      m_stateStream = EClosed;
      break;
  }
  assert( EClosed == m_stateStream );
  
  m_stateStream = EOpenRequest;
  
  bool bOk( true );
  
  avcodec_register_all();
  av_register_all();

  int status;

  // https://libav.org/doxygen/master/group__lavf__decoding.html
  // https://libav.org/doxygen/master/transcode_aac_8c-example.html#a7

  // open file, and fill related context
  status = avformat_open_input( &m_pFormatContext, sFile.c_str(), NULL, NULL);
  if ( 0 != status ) {
    std::cout << "info: error on open: " << status << std::endl;
    m_stateStream = EError;
    bOk = false;
    m_pFormatContext = 0;
  }
  else {
    
    // check that there are streams available
    status = avformat_find_stream_info( m_pFormatContext, 0 );  // https://libav.org/doxygen/master/group__lavf__decoding.html
    if ( 0 > status ) {
      std::cout << "info strm: none" << std::endl;
      avformat_close_input( &m_pFormatContext );
      m_pFormatContext = 0;
      m_stateStream = EError;
      bOk = false;
    }
    else {
      // when found, iterate the streams
      unsigned int nbStreams = m_pFormatContext->nb_streams;
      if ( 0 == nbStreams ) {
        std::cout << "info strm:  no streams found to decode " << std::endl;
        avformat_close_input( &m_pFormatContext );
        m_pFormatContext = 0;
        m_stateStream = EError;
        bOk = false;
      }
      else {
        assert( 0 == m_vStreamInfo.size() );
        m_vStreamInfo.resize( nbStreams );
        std::cout << "info dtl: " << "#streams=" << nbStreams << std::endl;
        for ( unsigned int ix = 0; ix < nbStreams; ++ix ) {
          
          m_vStreamInfo[ix].pCodec = avcodec_find_decoder( m_pFormatContext->streams[ix]->codec->codec_id );
          if ( 0 == m_vStreamInfo[ix].pCodec ) {
            std::cout << "info strm " << ix << " no codec" << std::endl;
          }
          else {
            
            // when a stream with codec found, prepare to use it in decode activities
            std::cout << "info strm " << ix << " " << m_vStreamInfo[ix].pCodec->name << std::endl;
            m_vStreamInfo[ix].pCodecContext = avcodec_alloc_context3( m_vStreamInfo[ix].pCodec );
            
            if ( m_vStreamInfo[ix].pCodec->capabilities & CODEC_CAP_TRUNCATED ) {
              m_vStreamInfo[ix].pCodecContext->flags |= CODEC_FLAG_TRUNCATED;
            }
            
            // http://stackoverflow.com/questions/12778978/libav-ffmpeg-avcodec-decode-video2-returns-1-when-separating-demultiplexing
            status = avcodec_copy_context( m_vStreamInfo[ix].pCodecContext, m_pFormatContext->streams[ix]->codec );
            if ( 0 != status ) {
              std::cout << "error on stream " << ix << ": avcodec_copy_context" << std::endl;
            }
      
            status = avcodec_open2( m_vStreamInfo[ix].pCodecContext, m_vStreamInfo[ix].pCodec, 0 ); // https://libav.org/doxygen/master/group__lavc__core.html
            if ( 0 != status ) {
              // need to close anything for this codec?
              //throw std::runtime_error( "couldn't set codec");
              // lots of uncleaned variables here
              //bOk = false;
            }
          }
        }  // iterate streams
        
        m_stateStream = EOpen; // do a default
        // find the best stream for decoding
        AVCodec* pBestCodecForVideo( 0 );  // should be able to discard this, may need to match addresses
        m_ixBestVideoStream = av_find_best_stream( m_pFormatContext, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &pBestCodecForVideo, 0 );
        if ( 0 > m_ixBestVideoStream ) {
          if ( AVERROR_STREAM_NOT_FOUND == m_ixBestVideoStream ) std::cout << "no best video stream found" << std::endl;
          if ( AVERROR_DECODER_NOT_FOUND == m_ixBestVideoStream ) std::cout << "no decoder found for best video stream found" << std::endl;
          m_stateStream = EError;
        }
        else {
          m_fi.ttlVideoFrames = m_pFormatContext->streams[m_ixBestVideoStream]->nb_frames;
          m_fi.duration = m_pFormatContext->duration;
        }

        AVCodec* pBestCodecForAudio( 0 );  // should be able to discard this, may need to match addresses 
        m_ixBestAudioStream = av_find_best_stream( m_pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1, &pBestCodecForAudio, 0 );
        if ( 0 > m_ixBestAudioStream ) {
          if ( AVERROR_STREAM_NOT_FOUND == m_ixBestAudioStream ) std::cout << "no best audio stream found" << std::endl;
          if ( AVERROR_DECODER_NOT_FOUND == m_ixBestAudioStream ) std::cout << "no decoder found for best audio stream found" << std::endl;
          //m_stateStream = EError;  // ignore audio for now
        }
        else {
          m_fi.ttlAudioFrames = m_pFormatContext->streams[m_ixBestAudioStream]->nb_frames;
        }
        
        if ( EError == m_stateStream ) {
          avformat_close_input( &m_pFormatContext );
          m_pFormatContext = 0;  
          bOk = false;
        }

      } // done finding main audio and video streams
      
    }
    
  }

  return bOk;
  
}

void MediaStreamDecode::EmitStats( void ) {
  
  std::cout << "==== Media File Stats ==== " << std::endl;
  
  switch ( m_stateStream ) {
    case EOpen:
    case EPlaying:
    case EPaused:
    case EStopped:
      break;
    default:
      assert( false );
  }
  
  AVDictionaryEntry* pDictEntry = 0;
  
  std::cout << "basics: " << AV_TIME_BASE 
    << ", start: " << m_pFormatContext->start_time
    << ", dur: " << m_pFormatContext->duration << ", " << (double) m_pFormatContext->duration / AV_TIME_BASE / 60.0 << " minutes"
    //<< ", " << m_pFormatContext->
    << std::endl;
  
  // emit dictionary
  while ( ( pDictEntry = av_dict_get( m_pFormatContext->metadata, "", pDictEntry, AV_DICT_IGNORE_SUFFIX) ) ) {
    std::cout << "info dict: " << pDictEntry->key << "=" << pDictEntry->value << std::endl;
  }
  
  for ( unsigned int ix = 0; ix < m_vStreamInfo.size(); ++ix ) {
    std::cout << "info times " 
      << "curdts: " << m_pFormatContext->streams[ix]->cur_dts
      << ", frstdts: " << m_pFormatContext->streams[ix]->first_dts
      << ", start: " << m_pFormatContext->streams[ix]->start_time
      << std::endl;
      
    AVRational fr1( m_pFormatContext->streams[ix]->codec->framerate );
    std::cout << "info strm " << ix << " frame rate " << fr1.num << "/" << fr1.den << std::endl;
    AVRational tb1( m_pFormatContext->streams[ix]->time_base );
    std::cout << "info strm " << ix << " time_base " << tb1.num << "/" << tb1.den << " (use frame rate instead)" << std::endl;
    AVRational afr( m_pFormatContext->streams[ix]->avg_frame_rate );
    std::cout << "info strm " << ix << " afr " << afr.num << "/" << afr.den << std::endl;
    int64_t nbFrames( m_pFormatContext->streams[ix]->nb_frames );
    std::cout << "info strm " << ix << " frames " << nbFrames << std::endl;
    std::cout << "info strm " << ix << " buffer size " << m_pFormatContext->max_picture_buffer << std::endl;
    std::cout << "info strm " << ix << " h/w " 
      << m_pFormatContext->streams[ix]->codec->height << ", " << m_pFormatContext->streams[ix]->codec->coded_height << ", "
      << m_pFormatContext->streams[ix]->codec->width  << ", " << m_pFormatContext->streams[ix]->codec->coded_width << std::endl;
    AVRational ar( m_pFormatContext->streams[ix]->codec->sample_aspect_ratio );
    std::cout << "info strm " << ix << " ar " << ar.num << "/" << ar.den << std::endl;
    std::cout << "info strm " << ix << " channels " << m_pFormatContext->streams[ix]->codec->channels << std::endl; // audio channels
    AVPixelFormat pf( m_pFormatContext->streams[ix]->codec->pix_fmt );
    std::cout << "info strm " << ix << " pixfmt " << pf << std::endl;
    AVMediaType mt( m_pFormatContext->streams[ix]->codec->codec_type );
    std::cout << "info strm " << ix << " type " << mt << std::endl;
    if ( 0 != m_vStreamInfo[ix].pCodecContext ) {
      std::cout << "refcounted frames " << m_vStreamInfo[ix].pCodecContext->refcounted_frames << std::endl;
    }
    
    // AVCodecContext::frame_number
  }
    
  std::cout << "best video stream: " << m_ixBestVideoStream << ", best audio stream: " << m_ixBestAudioStream << std::endl;
}

void MediaStreamDecode::Close( void ) {

  switch ( m_stateStream ) {
    case EClosed:
      break;
    case EOpen:
    case EStopped:
      m_vStreamInfo.clear();
      avformat_close_input( &m_pFormatContext );
      m_pFormatContext = 0;
      m_stateStream = EClosed;
      break;
    default:
      assert( false );
  }
  

}

void MediaStreamDecode::Play( void ) {

  assert( ( m_stateStream == EOpen ) || ( m_stateStream == EStopped ) || ( m_stateStream == EPlayRequest ) );
  
  m_stateStream = EPlaying;
  m_Srvc.post( boost::phoenix::bind( &MediaStreamDecode::ProcessStream, this, m_ixBestAudioStream, m_ixBestVideoStream ) );
  
}

  // may want to use seek at some point (when looping) av_seek_frame : https://libav.org/doxygen/master/group__lavf__decoding.html)
  

void MediaStreamDecode::Pause( void ) {
  // todo put in lock and wait for ThreadRequestFilled
  boost::lock_guard<boost::mutex> lock(m_mutexDecode);
  m_stateThreadRequest = ThreadRequestPause;
  m_bProcessStreamWait = true;
}

void MediaStreamDecode::Resume( void ) {
  // todo put in lock and wait for ThreadRequestFilled
  boost::lock_guard<boost::mutex> lock(m_mutexDecode);
  m_stateThreadRequest = ThreadRequestPlay;
  m_bProcessStreamWait = false;
  m_cvDecode.notify_one();
}

void MediaStreamDecode::Stop( void ) {
  // todo put in lock and wait for ThreadRequestFilled
  boost::lock_guard<boost::mutex> lock(m_mutexDecode);
  m_stateThreadRequest = ThreadRequestStop;
  m_bProcessStreamWait = false;
  m_cvDecode.notify_one();  // just in case is paused
}

void MediaStreamDecode::Rewind( void ) {
  assert( ( ThreadStatePaused == m_stateThread ) || ( ThreadStateStopped == m_stateThread ) );
}

void MediaStreamDecode::ProcessStream( size_t ixAudio, size_t ixVideo ) {  // background thread
  
  // need asserts to verify indexes are for valid codecs of the type specified
  
  assert( ThreadStateStopped == m_stateThread );
  
  m_stateThread = ThreadStatePlaying;
  
  AVPacket packet;
  av_init_packet( &packet );
  packet.data = 0;
  packet.size = 0;
  AVFrame* pFrame; // https://libav.org/doxygen/master/group__lavu__frame.html
  pFrame = av_frame_alloc();
  assert( 0 != pFrame );
  int status( 0 );
  
  // need to protect with atomics?
  while ( ThreadRequestStop != m_stateThreadRequest ) {
    {
      boost::unique_lock<boost::mutex> lock(m_mutexDecode);
      switch ( m_stateThreadRequest ) {
        case ThreadRequestPlay:
          m_stateThreadRequest = ThreadRequestFilled;
          m_stateThread = ThreadStatePlaying;
          // continue with frame processing
          break;
        case ThreadRequestPause:
          m_stateThreadRequest = ThreadRequestFilled;
          m_stateThread = ThreadStatePaused;
          while ( m_bProcessStreamWait ) {
            m_cvDecode.wait( lock );
          }
          m_stateThread = ThreadStatePlaying;
          break;
        case ThreadRequestFilled:
          // continue
          break;
      }
    }
    
    m_ts.start = boost::chrono::high_resolution_clock::now();
  
    if ( 0 > ( status = av_read_frame( m_pFormatContext, &packet ) ) ) {  // https://libav.org/doxygen/master/group__lavf__decoding.html
      m_stateThreadRequest = ThreadRequestStop;
    }
    else {
      //std::cout << "packet " << packet.stream_index << ": " << packet.size << ", " << std::endl;
      m_ts.parse = boost::chrono::high_resolution_clock::now();
      int gotFrame( 0 );
      // https://libav.org/doxygen/master/group__lavc__decoding.html
      if ( ixAudio == packet.stream_index ) {
        status = avcodec_decode_audio4( m_vStreamInfo[ixAudio].pCodecContext, pFrame, &gotFrame, &packet );
        if ( 0 > status ) {
          std::cout << "audio decode error: " << status << std::endl;
        }
        else {
          // emit the audio
          m_ts.decoded = boost::chrono::high_resolution_clock::now();
          m_fi.nAudioFrame = m_vStreamInfo[ixAudio].pCodecContext->frame_number;
        }
      }
      if ( ixVideo == packet.stream_index ) {
        status = avcodec_decode_video2( m_vStreamInfo[ixVideo].pCodecContext , pFrame, &gotFrame, &packet );
        if ( 0 > status ) {
          std::cout << "video decode error: " << status << std::endl;
        }
        else {
          if ( 0 != gotFrame ) {  // emit picture
            m_ts.decoded = boost::chrono::high_resolution_clock::now();
            //std::cout << "packet " << packet.stream_index << ": " << packet.pts << ", " << packet.dts << ", " << packet.duration << std::endl;
            // can't run in service queue as we need to return the pFrame, and is not queueable, unless we refcount it
            m_fi.nVideoFrame = m_vStreamInfo[ixVideo].pCodecContext->frame_number;  // 1 .. n
            m_fi.pts = pFrame->pts;
            m_fi.pkt_pts = pFrame->pkt_pts;
            m_fi.pkt_dts = pFrame->pkt_dts;
            ProcessVideoFrame( m_vStreamInfo[ixVideo].pCodecContext, pFrame, m_ts );
            //std::cout << "width: " << pFrame->width << " height: " << pFrame->height << " format: " << pFrame->format;
            //if ( 1 == pFrame->key_frame ) std::cout << " key frame " << pFrame->display_picture_number;
            //std::cout << std::endl;
            m_ts.start = boost::chrono::high_resolution_clock::now();  // reset for next portion of read
            m_ts.clear();
          }
        }
      }
    }
  }  // while m_stateThreadRequest
  
  m_stateThread = ThreadStateStopping;
  
  m_signaDecodeDone();  // but there may be HandleFrameTransformToImage in the queue

  av_frame_free( &pFrame );
  av_free_packet( &packet );
  
  m_stateThreadRequest = ThreadRequestFilled;
  m_stateThread = ThreadStateStopped;
  m_stateStream = EStopped;
  
  std::cout << "ProcessStream exited" << std::endl;
}

void MediaStreamDecode::ProcessVideoFrame( AVCodecContext* pContext, AVFrame* pFrame, structTimeSteps perf ) {
  
#define FMT PIX_FMT_RGB32
//#define FMT PIX_FMT_RGB24
  
  //int srcX = pFrame->width;  // these are actually 0, so can't use
  //int srcY = pFrame->height;
  
  int srcX = pContext->width;
  int srcY = pContext->height;
  
  size_t nBytes = avpicture_get_size( FMT, srcX, srcY );
  
  struct SwsContext* swsContext = sws_getContext(srcX, srcY, pContext->pix_fmt, srcX, srcY, FMT, SWS_BILINEAR, NULL, NULL, NULL);
  //struct SwsContext* swsContext = sws_getContext(srcX, srcY, context->pix_fmt, srcX, srcY, PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
  //struct SwsContext* swsContext = sws_getContext(srcX, srcY, context->pix_fmt, srcX, srcY, PIX_FMT_RGB32_1, SWS_BICUBIC, NULL, NULL, NULL);
  
  AVFrame* pRGB;
  pRGB = av_frame_alloc();
  assert( 0 != pRGB );
  
  RawImage::pRawImage_t pRawImage;
  pRawImage.reset( new RawImage( RawImage::FormatBGRA, nBytes, srcX, srcY, m_fi.nVideoFrame ) );
  
  //uint8_t* buf  = (uint8_t*)av_malloc( nBytes * sizeof( uint8_t ) );  // *** todo:  keep from call to call, be aware of frame size changes in test material
  avpicture_fill( ( AVPicture*)pRGB, pRawImage->GetBuffer(), FMT, srcX, srcY );  // effectively assigns buf ptr to pRGB entry 0, linesize is # bytes in line
  
  perf.filled = boost::chrono::high_resolution_clock::now();
  
  sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, srcY, pRGB->data, pRGB->linesize );
  
  sws_freeContext( swsContext );
  
  perf.scaled = boost::chrono::high_resolution_clock::now();

  //m_signalImageReady( pRawImage, perf, m_fi );
  m_signalImageReady( pRawImage, perf );
  av_free( pRGB );  
  pRGB = 0;
  
  // ** note decode currently works faster than the transform, so transform work will queue up.
  //    be aware that if using multiple threads, that processing needs to be sync'd so frames stay in order
  //      or do frame ordering in final presentation to screen, buffer loop
}


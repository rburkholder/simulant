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
}

#include <wx/image.h>
#include <wx/rawbmp.h>
//#include <wx/app.h>
#include <wx/event.h>

//#include "EventImage.h"

#include "MediaStreamDecode.h"

// 2015/05/09  need to use fifo to queue processing

MediaStreamDecode::MediaStreamDecode( )
: m_pFormatContext( 0 ), m_bInUse( false ), m_ixBestVideoStream( 0 ), m_ixBestAudioStream( 0 )
{
  
    // workers for the movie action
  m_pWork = new boost::asio::io_service::work(m_Srvc);  // keep the asio service running 
  //m_thrdWorkers = boost::thread( boost::phoenix::bind( &AppProjection::Workers, this ) );
  for ( std::size_t ix = 0; ix < 2; ix++ ) {  // HandleFrame, TransformToImage
    m_threadsWorkers.create_thread( boost::phoenix::bind( &boost::asio::io_service::run, &m_Srvc ) );
  }

}

MediaStreamDecode::~MediaStreamDecode( ) {
  Close();
  delete m_pWork;
  m_pWork = 0;
  m_threadsWorkers.join_all();  // wait for processing to end
}

void MediaStreamDecode::Workers( void ) {
  m_Srvc.run(); 
}

void MediaStreamDecode::Play( void ) {
  
  if ( !m_bInUse ) {
    std::runtime_error( "instance of MediaStreamDecode not in use" );
  }
  
//  namespace args = boost::phoenix::arg_names;
//  pDecoder->m_OnFrame.connect( 
//    boost::phoenix::bind( &TreeItemVideo::HandleOnFrame, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );

  // need to check not already running
  m_Srvc.post( boost::phoenix::bind( &MediaStreamDecode::ProcessStream, this, m_ixBestAudioStream, m_ixBestVideoStream ) );
  
  // need to auto close, or manually close stream
  // also need to set flag to interrupt frame generation
    
}

bool MediaStreamDecode::Open( const std::string& sFile ) {
  
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
      bOk = false;
    }
    else {
      // when found, iterate the streams
      unsigned int nbStreams = m_pFormatContext->nb_streams;
      if ( 0 == nbStreams ) {
        std::cout << "info strm:  no streams found to decode " << std::endl;
        avformat_close_input( &m_pFormatContext );
        m_pFormatContext = 0;
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
        }
        
        // find the best stream for decoding
        AVCodec* pBestCodecForVideo( 0 );  // should be able to discard this, may need to match addresses
        m_ixBestVideoStream = av_find_best_stream( m_pFormatContext, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &pBestCodecForVideo, 0 );
        if ( 0 > m_ixBestVideoStream ) {
          if ( AVERROR_STREAM_NOT_FOUND == m_ixBestVideoStream ) std::cout << "no best video stream found" << std::endl;
          if ( AVERROR_DECODER_NOT_FOUND == m_ixBestVideoStream ) std::cout << "no decoder found for best video stream found" << std::endl;
        }

        AVCodec* pBestCodecForAudio( 0 );  // should be able to discard this, may need to match addresses 
        m_ixBestAudioStream = av_find_best_stream( m_pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1, &pBestCodecForAudio, 0 );
        if ( 0 > m_ixBestAudioStream ) {
          if ( AVERROR_STREAM_NOT_FOUND == m_ixBestAudioStream ) std::cout << "no best audio stream found" << std::endl;
          if ( AVERROR_DECODER_NOT_FOUND == m_ixBestAudioStream ) std::cout << "no decoder found for best audio stream found" << std::endl;
        }

        m_bInUse = true;

      } // done iterating streams
      
    }
    
  }

  return bOk;
  
}

void MediaStreamDecode::EmitStats( void ) {
  
  assert( m_bInUse );
  
  std::cout << "==== Media File Stats ==== " << std::endl;
  
  AVDictionaryEntry* pDictEntry = 0;
  
  // emit dictionary
  while ( ( pDictEntry = av_dict_get( m_pFormatContext->metadata, "", pDictEntry, AV_DICT_IGNORE_SUFFIX) ) ) {
    std::cout << "info dict: " << pDictEntry->key << "=" << pDictEntry->value << std::endl;
  }
  
  for ( unsigned int ix = 0; ix < m_vStreamInfo.size(); ++ix ) {
    AVRational fr( m_pFormatContext->streams[ix]->avg_frame_rate );
    std::cout << "info strm " << ix << " fr " << fr.num << "/" << fr.den << std::endl;
    int64_t nbFrames( m_pFormatContext->streams[ix]->nb_frames );
    std::cout << "info strm " << ix << " frames " << nbFrames << std::endl;
    std::cout << "info strm " << ix << " buffer size " << m_pFormatContext->max_picture_buffer << std::endl;
    std::cout << "info strm " << ix << " w/h " 
      << m_pFormatContext->streams[ix]->codec->width  << ", " << m_pFormatContext->streams[ix]->codec->coded_width << ", "
      << m_pFormatContext->streams[ix]->codec->height << ", " << m_pFormatContext->streams[ix]->codec->coded_height << std::endl;
    AVRational ar( m_pFormatContext->streams[ix]->codec->sample_aspect_ratio );
    std::cout << "info strm " << ix << " ar " << ar.num << "/" << ar.den << std::endl;
    std::cout << "info strm " << ix << " channels " << m_pFormatContext->streams[ix]->codec->channels << std::endl; // audio channels
    AVPixelFormat pf( m_pFormatContext->streams[ix]->codec->pix_fmt );
    std::cout << "info strm " << ix << " pixfmt " << pf << std::endl;
    AVMediaType mt( m_pFormatContext->streams[ix]->codec->codec_type );
    std::cout << "info strm " << ix << " type " << mt << std::endl;
    AVRational tb( m_pFormatContext->streams[ix]->time_base );
    std::cout << "info strm " << ix << " tb " << tb.num << "/" << tb.den << std::endl;
    std::cout << "refcounted frames " << m_vStreamInfo[ix].pCodecContext->refcounted_frames << std::endl;
  }
    
  std::cout << "best video stream: " << m_ixBestVideoStream << ", best audio stream: " << m_ixBestAudioStream << std::endl;
}

void MediaStreamDecode::ProcessStream( size_t ixAudio, size_t ixVideo ) {  // should be in background thread
  
  // need asserts to verify indexes are for valid codecs of the type specified
  
  //AV_TIME_BASE
  //AVFormatContext *pFormatCtx;
  //double duration = double(pFormatCtx->duration) / AV_TIME_BASE;

  AVPacket packet;
  av_init_packet( &packet );
  packet.data = 0;
  packet.size = 0;
  AVFrame* pFrame; // https://libav.org/doxygen/master/group__lavu__frame.html
  pFrame = av_frame_alloc();
  //pFrame = avcodec_alloc_frame(); // http://pastebin.com/ByYzePa3  deprecated
  assert( 0 != pFrame );
  int status( 0 );
  m_ts.start = boost::chrono::high_resolution_clock::now();
  while ( 0 == ( status = av_read_frame( m_pFormatContext, &packet ) ) ) {  // https://libav.org/doxygen/master/group__lavf__decoding.html
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
          // could put into asio queue
          //HandleOnFrame( m_vStreamInfo[ixVideo].pCodecContext, pFrame, 0, m_ts );
          // can't run in service queue as we need to return the pFrame, and is not queueable
          //m_Srvc.post( boost::phoenix::bind( &MediaStreamDecode::HandleOnFrame, this, m_vStreamInfo[ixVideo].pCodecContext, pFrame, m_ts ) );
          HandleOnFrame( m_vStreamInfo[ixVideo].pCodecContext, pFrame, m_ts );
          //std::cout << "width: " << pFrame->width << " height: " << pFrame->height << " format: " << pFrame->format;
          //if ( 1 == pFrame->key_frame ) std::cout << " key frame " << pFrame->display_picture_number;
          //std::cout << std::endl;
          m_ts.start = boost::chrono::high_resolution_clock::now();  // reset for next portion of read
          m_ts.clear();
        }
      }
    }
  }
  
  m_signaDecodeDone();

  // may want to use seek at some point (when looping) av_seek_frame : https://libav.org/doxygen/master/group__lavf__decoding.html)

  av_frame_free( &pFrame );
  av_free_packet( &packet );
  
}

void MediaStreamDecode::Close( void ) {

  // todo: need to ensure stream is stopped
  
  if ( m_bInUse ) {
    m_vStreamInfo.clear();
    avformat_close_input( &m_pFormatContext );
    m_pFormatContext = 0;
    m_bInUse = false;
  }

}

void MediaStreamDecode::HandleOnFrame( AVCodecContext* pContext, AVFrame* pFrame, structTimeSteps perf ) {
  
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
  
  uint8_t* buf  = (uint8_t*)av_malloc( nBytes * sizeof( uint8_t ) );  // *** todo:  keep from call to call, be aware of frame size changes in test material
  avpicture_fill( ( AVPicture*)pRGB, buf, FMT, srcX, srcY );
  
  perf.filled = boost::chrono::high_resolution_clock::now();
  
  sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, srcY, pRGB->data, pRGB->linesize );
  
  perf.scaled = boost::chrono::high_resolution_clock::now();

  m_Srvc.post( boost::phoenix::bind( &MediaStreamDecode::HandleFrameTransformToImage, this, pRGB, buf, perf, srcX, srcY ) );
  
  // ** note decode currently works faster than the transform, so transform work will queue up.
  //    need to deal with proper timing of frames.
  //    need to base frame timing on what is in the file
  //    need sync so stops at high water mark, resumes decode at low water mark
  //    be aware that if using multiple threads, that processing needs to be sync'd so frames stay in order
  //      or do frame ordering in final presentation to screen, buffer loop
}

void MediaStreamDecode::HandleFrameTransformToImage( AVFrame* pRgb, uint8_t* buf, structTimeSteps perf, int srcX, int srcY ) {
  
  perf.queue1 = boost::chrono::high_resolution_clock::now();
  
  uint8_t* pSrcFrame( *pRgb->data );
  
  pImage_t pImage( new wxImage( srcX, srcY, false ) );
  wxImagePixelData data( *pImage );
  wxImagePixelData::Iterator pDestImage( data );
  
  for ( int iy = 0; iy < srcY; ++iy ) {
    for ( int ix = 0; ix < srcX; ++ix ) {
      //++pSrc;  // skip A
      pDestImage.Blue() =  *pSrcFrame; ++pSrcFrame;
      pDestImage.Green() = *pSrcFrame; ++pSrcFrame;
      pDestImage.Red() =   *pSrcFrame; ++pSrcFrame;
      ++pDestImage;
      ++pSrcFrame; // skip alpha?
    }
  }

  perf.copied = boost::chrono::high_resolution_clock::now();
  
  m_signalImageReady( pImage, perf );

  av_free( buf );
  av_free( pRgb );  
  
}


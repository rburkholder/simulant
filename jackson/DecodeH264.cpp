
// from http://roxlu.com/2014/039/decoding-h264-and-yuv420p-playback

#include <cassert>
#include <array>
#include <iostream>

extern "C" {
#include <libavutil/log.h>
}

#include "DecodeH264.h"

//void log(void* ptr, int level, const char* fmt, va_list vl) {
//  std::array<char,256> line;
//  int printBuffer = 1;
//  av_log_format_line(ptr, level, fmt, vl, line.data(), line.size(), &printBuffer);
  //OutputDebugStringA(line.data());
//  std::cout << line.data() << std::endl;
//}
 
DecodeH264::DecodeH264( void* user ) 
  :m_codec(NULL)
  ,m_codecContext(NULL)
  ,m_parser(NULL)
  ,fp(NULL)
  ,frame(0)
  ,frame_timeout(0)
  ,frame_delay(0)
  ,cb_user( user )

{
  avcodec_register_all();
  av_log_set_level(AV_LOG_DEBUG);  //AV_LOG_VERBOSE AV_LOG_INFO AV_LOG_WARNING AV_LOG_ERROR AV_LOG_FATAL
}
 
DecodeH264::~DecodeH264() {
 
  if(m_parser) {
    av_parser_close(m_parser);
    m_parser = NULL;
  }
 
  if(m_codecContext) {
    avcodec_close(m_codecContext);
    av_free(m_codecContext);
    m_codecContext = NULL;
  }
 
  if(m_picture) {
    av_free(m_picture);
    m_picture = NULL;
  }
 
  if(fp) {
    fclose(fp);
    fp = NULL;
  }
 
  cb_user = NULL;
  frame = 0;
  frame_timeout = 0;
  
  std::cout << "DecodeH264 destructor" << std::endl;
}
 
bool DecodeH264::load( const std::string filepath, float fps) {
 
  m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if(!m_codec) {
    printf("Error: cannot find the h264 codec: %s\n", filepath.c_str());
    return false;
  }
 
  m_codecContext = avcodec_alloc_context3(m_codec);
 
  if(m_codec->capabilities & CODEC_CAP_TRUNCATED) {
    m_codecContext->flags |= CODEC_FLAG_TRUNCATED;
  }
 
  if(avcodec_open2(m_codecContext, m_codec, NULL) < 0) {
    printf("Error: could not open codec.\n");
    return false;
  }
 
  fp = fopen(filepath.c_str(), "rb");
 
  if(!fp) {
    printf("Error: cannot open: %s\n", filepath.c_str());
    return false;
  }
 
  m_picture = av_frame_alloc();
  m_parser = av_parser_init(AV_CODEC_ID_H264);
 
  if(!m_parser) {
    printf("Error: cannot create H264 parser.\n");
    return false;
  }
 
//  if(fps > 0.0001f) {
//    frame_delay = (1.0f/fps) * 1000ull * 1000ull * 1000ull;
//    frame_timeout = rx_hrtime() + frame_delay;
//  }
 
  return true;
}

bool DecodeH264::ProcessFile() {
  
  if(!fp) {
    printf("Cannot update .. file not opened...\n");
    return false;
  }
  
  //int frame = 0;
  
  size_t nBytes( 0 );
  uint8_t* data = NULL;
  int size( 0 );
  m_ts.start = boost::chrono::high_resolution_clock::now();
  while ( 0 != ( nBytes = fread(chBuf, 1, H264_INBUF_SIZE, fp) ) ) {  // http://www.cplusplus.com/reference/cstdio/fread/
    uint8_t* pchBuf( chBuf );
    while ( 0 != nBytes ) {
      int len = av_parser_parse2(m_parser, m_codecContext, &data, &size, 
                                 pchBuf, nBytes, 0, 0, AV_NOPTS_VALUE );
      assert( len <= nBytes );
      if ( 0 != size ) {
        m_ts.parse = boost::chrono::high_resolution_clock::now();
        int val = decodeFrame(data, size);
        m_ts.start = boost::chrono::high_resolution_clock::now();
      }
      // need an inter-frame delay at some point
      nBytes -= len;
      pchBuf += len;
    }
  }
  // no more data:
  //   process end of file in parser and decoder
  // close file out, or reset pointer to beginning
  return true;
}
 
int DecodeH264::decodeFrame(uint8_t* data, int size) {
 
  int val = 0;
  
  AVPacket pkt;
  int got_picture = 0;
  int len = 0;
 
  av_init_packet(&pkt);
 
  pkt.data = data;
  pkt.size = size;
 
  len = avcodec_decode_video2(m_codecContext, m_picture, &got_picture, &pkt);
  m_ts.decoded = boost::chrono::high_resolution_clock::now();
  if(len < 0) {
    printf("Error while decoding a frame.\n");
    val = -1;
  }
    else {
      if( 0 == got_picture ) {
        val = 0;
      }
    else {
      ++frame;
      m_OnFrame(m_codecContext, m_picture, &pkt, cb_user, m_ts);
      val = 1;
    }
  }
 
  return val;
}
 

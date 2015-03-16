/*
 
  H264_Decoder
  ---------------------------------------
 
  Example that shows how to use the libav parser system. This class forces a 
  H264 parser and codec. You use it by opening a file that is encoded with x264 
  using the `load()` function. You can pass the framerate you want to use for playback.
  If you don't pass the framerate, we will detect it as soon as the parser found 
  the correct information.
 
  After calling load(), you can call readFrame() which will read a new frame when
  necessary. It will also make sure that it will read enough data from the buffer/file
  when there is not enough data in the buffer.
 
  `readFrame()` will trigger calls to the given `h264_decoder_callback` that you pass
  to the constructor. 
 * 
 * from http://roxlu.com/2014/039/decoding-h264-and-yuv420p-playback
 
 */

#ifndef DecodeH264_H
#define DecodeH264_H
 
#define H264_INBUF_SIZE 16384                                                           /* number of bytes we read per chunk */
 
#include <stdio.h>
//#include <stdlib.h>
#include <string>
//#include <vector>

//#include <tinylib.h>

#include <boost/signals2/signal.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
 
#include "common.h"
 
class DecodeH264 {
 
 public:
   
  DecodeH264( void* user );                         /* pass in a callback function that is called whenever we decoded a video frame, make sure to call `readFrame()` repeatedly */
  ~DecodeH264();                                                                       /* d'tor, cleans up the allocated objects and closes the codec context */
  bool load(const std::string filepath, float fps = 0.0f);                                     /* load a video file which is encoded with x264 */
  bool readFrame();                                                                      /* read a frame if necessary */
  
  bool ProcessFile();
  
 typedef boost::signals2::signal<void (AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf)> OnFrame_t;  // make copy instead of reference
 
  OnFrame_t m_OnFrame;
  
 private:
  int decodeFrame(uint8_t* data, int size);                                             /* decode a frame we read from the buffer */
  structTimeSteps m_ts;
 
 public:
  AVCodec* m_codec;                                                                        /* the AVCodec* which represents the H264 decoder */
  AVCodecContext* m_codecContext;                                                         /* the context; keeps generic state */
  AVCodecParserContext* m_parser;                                                          /* parser that is used to decode the h264 bitstream */
  AVFrame* m_picture;                                                                      /* will contain a decoded picture */
  uint8_t chBuf[H264_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];                         /* used to read chunks from the file */
  FILE* fp;                                                                              /* file pointer to the file from which we read the h264 data */
  int frame;                                                                             /* the number of decoded frames */
  void* cb_user;                                                                         /* the void* with user data that is passed into the set callback */
  uint64_t frame_timeout;                                                                /* timeout when we need to parse a new frame */
  uint64_t frame_delay;                                                                  /* delay between frames (in ns) */
};
 
#endif

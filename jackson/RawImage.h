/* 
 * File:   RawImage.h
 * Author: rpb
 *
 * Created on May 12, 2015, 9:43 PM
 */

#pragma once

#include <boost/shared_ptr.hpp>

class RawImage {
public:
  
  typedef boost::shared_ptr<RawImage> pRawImage_t;
  
  enum Format { FormatRGB, FormatBGR, FormatRGBA, FormatBGRA };  //first two are three bytes, second are four bytes
  RawImage( Format format, size_t width, size_t height );
  RawImage( Format format, size_t width, size_t height, size_t size );
  virtual ~RawImage( void );
  
  uint8_t* GetBuffer( void ) const { return m_pImageData; }
  size_t GetHeight( void ) const { return m_height; }
  size_t GetWidth( void ) const { return m_width; }
  Format GetFormat( void ) const { return m_format; }
  //int64_t GetImageNumber( void ) const { return m_nImage; }
  
//  int64_t nAudioFrame; // current audio frame number
//  int64_t ttlAudioFrames;
  int64_t nVideoFrame; // current video frame number, 1 ... n
//  int64_t ttlVideoFrames;
//  int64_t pts;
  int64_t pkt_pts;  // 1001, 2002, 3003, ...  or AV_NOPTS_VALUE
  int64_t pkt_dts;
  
protected:
private:
  Format m_format;
  size_t m_height;
  size_t m_width;
  uint8_t* m_pImageData;
  RawImage( void );
  RawImage( const RawImage& rhs ) {};
};


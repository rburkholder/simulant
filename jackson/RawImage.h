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
  RawImage( Format format, size_t size, size_t width, size_t height );
  virtual ~RawImage( void );
  
  uint8_t* GetBuffer( void ) const { return m_pImageData; }
  size_t GetHeight( void ) const { return m_height; }
  size_t GetWidth( void ) const { return m_width; }
  Format GetFormat( void ) const { return m_format; }
protected:
private:
  Format m_format;
  size_t m_height;
  size_t m_width;
  uint8_t* m_pImageData;
  RawImage( void );
  RawImage( const RawImage& rhs ) {};
};
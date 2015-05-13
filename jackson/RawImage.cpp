/* 
 * File:   RawImage.cpp
 * Author: rpb
 *
 * Created on May 12, 2015, 9:43 PM
 */

#include <stdexcept>

extern "C" {
#include "libavutil/common.h"
}

#include "RawImage.h"


RawImage::RawImage( void ): m_format( FormatRGBA ), m_pImageData( 0 ) {
}

RawImage::RawImage( Format format, size_t size, size_t width, size_t height )
: m_format( format ), m_height( height ), m_width( width ), m_pImageData( 0 )
{
  m_pImageData = (uint8_t*)av_malloc( size );
}

RawImage::RawImage( Format format, size_t width, size_t height )
: m_format( format ), m_height( height ), m_width( width ), m_pImageData( 0 )
{
  switch ( format ) {
    case FormatRGB:
    case FormatBGR:
      //m_pImageData = new uint8_t[ height * width * 3 ];  // will this work for various things?
      m_pImageData = (uint8_t*)av_malloc( height * width * 3 );  // will this work for various things?
      break;
    case FormatRGBA:
    case FormatBGRA:
      m_pImageData = (uint8_t*)av_malloc( height * width * 4 );
      break;
  }
  if ( 0 == m_pImageData ) 
    throw std::runtime_error( "can't allocate in RawImage" );
}

RawImage::~RawImage( void ) {
  assert( 0 != m_pImageData );
  av_free( m_pImageData );
  m_pImageData = 0;
}
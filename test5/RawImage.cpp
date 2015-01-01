/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

#include "stdafx.h"

#include <iostream>
#include <iomanip>

#include <boost/timer/timer.hpp>
//#include <boost/chrono/system_clocks.hpp>

#include "RawImage.h"

RawImage::RawImage( ) : m_bImageLoaded(false) {
}

RawImage::~RawImage() {
  //LibRaw::free_image( )
}

int my_progress_callback( void *data, enum LibRaw_progress state, int iter, int expected ) {
  if (iter == 0)
    //assert( 0 != data );
    // use  LibRaw::strprogress to decode stage  http://www.libraw.org/docs/API-CXX-eng.html#unpack
    std::cout << "CB: state=" << std::hex << state << ", expected " << std::dec << expected << " iterations" << std::endl;
  //std::cout << "CB: state=" << libraw_strprogress( state ) << ", expected " << expected << " iterations, " << (char*) data << std::endl;
  //printf( "CB: state=%x, expected %d iterations\n", state, expected );
  return 0;  // return 1; // cancel processing immediately
}

void RawImage::LoadImage( const std::string& sFileName ) {

  //int i;
  int ret;
  //int verbose = 0;

  //LibRaw m_raw;
  //m_raw.recycle( );  // automatically

  m_sFileName = sFileName;

  m_raw.set_progress_handler( &::my_progress_callback, NULL );

  boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now( );
  boost::timer::auto_cpu_timer t( std::cout );

  if (LIBRAW_SUCCESS != (ret = m_raw.open_file( sFileName.c_str( ) ))) {
    throw std::runtime_error( "Cannot open " + sFileName + " " + libraw_strerror( ret ) );
  }

  if (LIBRAW_SUCCESS != (ret = m_raw.unpack( ))) {
    throw std::runtime_error( "Cannot unpack " + sFileName + " " + libraw_strerror( ret ) );
  }

  boost::chrono::nanoseconds nsec = boost::chrono::system_clock::now( ) - start;
  std::cout << "File load took " << nsec.count() << " nanoseconds" << std::endl;

  m_bImageLoaded = true;

}

libraw_processed_image_t* RawImage::CalcImage( void ) {

  //int i;
  int ret;
  //int verbose = 0;

  m_raw.recycle( );  

  /*
  printf(
    "simple_dcraw - LibRaw %s sample. Emulates dcraw [-D] [-T] [-v] [-e] [-E]\n"
    " %d cameras supported\n"
    "Usage:  [-D] [-T] [-v] [-e] raw-files....\n"
    "\t-4 - 16-bit mode\n"
    "\t-v - verbose output\n"
    "\t-T - output TIFF files instead of .pgm/ppm\n"
    "\t-e - extract thumbnails (same as dcraw -e in separate run)\n",
    LibRaw::version( ),
    LibRaw::cameraCount( )
    );
  */

  putenv( (char*)"TZ=UTC" ); // dcraw compatibility, affects TIFF datestamp field

  //verbose++;
  //        OUT.output_bps = 16;  // bits per colour
  m_raw.set_progress_handler( &::my_progress_callback, NULL );

  boost::timer::auto_cpu_timer t(std::cout);
  boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now( );

  // customize conversion parameters
  if (0 != m_OnLibRawOutputParams) m_OnLibRawOutputParams( m_raw.imgdata.params );

  ret = m_raw.dcraw_process( );
  if (LIBRAW_SUCCESS != ret) {
    std::string sError( "Cannot do postprocessing: " );
    sError += libraw_strerror( ret );
    throw std::runtime_error( sError );
    // if (LIBRAW_FATAL_ERROR( ret )) ... ?
  }

 // if (verbose) printf( "Writing file %s\n", outfn );
//  std::string sOut( sFileName + ".tiff" );
//  if (LIBRAW_SUCCESS != (ret = m_raw.dcraw_ppm_tiff_writer( sOut.c_str() )))
//    throw std::runtime_error( "Cannot write " + sOut + libraw_strerror( ret )  );

  libraw_processed_image_t* img = m_raw.dcraw_make_mem_image( &ret );
  if (LIBRAW_SUCCESS != ret) {
    std::string sError( "Cannot make memory image: " );
    sError += libraw_strerror( ret );
    throw std::runtime_error( sError );
    // if (LIBRAW_FATAL_ERROR( ret )) ... ?
  }

  boost::chrono::nanoseconds nsec = boost::chrono::system_clock::now( ) - start;
  std::cout << "CalcImage took " << nsec.count() << " nanoseconds" << std::endl;

  return img;
}

void RawImage::FreeImage( libraw_processed_image_t* pImage ) {
  LibRaw::dcraw_clear_mem( pImage );
  //m_raw.free_image();
}

void RawImage::FileInfo( void ) {

  if (m_bImageLoaded) {
    int verbose = 0, ret, print_unpack = 0, print_frame = 0;

    //for (int i = 1; i<ac; i++) {
//        if (av[i][1] == 'v' && av[i][2] == 0) verbose++;
//        if (av[i][1] == 'u' && av[i][2] == 0) print_unpack++;
//        if (av[i][1] == 'f' && av[i][2] == 0) print_frame++;
//        if (av[i][1] == 'x' && av[i][2] == 0) O.force_foveon_x3f = 1;

//    if ((ret = MyCoolRawProcessor.adjust_sizes_info_only( )))
//        {
//          printf( "Cannot decode %s: %s\n", av[i], libraw_strerror( ret ) );
//          continue; // no recycle, open_file will recycle
//        }

#define P1 m_raw.imgdata.idata
#define P2 m_raw.imgdata.other
#define C m_raw.imgdata.color
#define S m_raw.imgdata.sizes
#define T m_raw.imgdata.thumbnail

    // http://stackoverflow.com/questions/11989374/floating-point-format-for-stdostream

    std::cout << "Filename: " << m_sFileName << std::endl;
    std::cout << "Timestamp: " << ctime( &(P2.timestamp) );
    std::cout << "Camera: " << P1.make << " " << P1.model << std::endl;
    if (P2.artist[0])
      std::cout << "Owner: " << P2.artist << std::endl;
//        if (P1.dng_version) {
//          std::cout << "DNG Version: ";
//          for (int i = 24; i >= 0; i -= 8)
//            printf( "%d%c", P1.dng_version >> i & 255, i ? '.' : '\n' );
//        }
    std::cout << "ISO speed: " << (int)P2.iso_speed << std::endl;
    std::cout << "Shutter: ";
    if ( P2.shutter>0.0) 
      if (1.0 > P2.shutter) std::cout << "1/" << 1.0 / P2.shutter;
      else std::cout << P2.shutter;
    std::cout << " sec" << std::endl;
    std::cout << "Aperture: f/" << P2.aperture << std::endl;
    std::cout << "Focal length: " << P2.focal_len << "mm" << std::endl;
    if (C.profile)
      std::cout << "Embedded ICC profile: yes, " << C.profile_length << " bytes" << std::endl;
    else
      std::cout << "Embedded ICC profile: no" << std::endl;

    std::cout << "Number of raw images: " << P1.raw_count << std::endl;
    if (S.pixel_aspect != 1)
      std::cout << "Pixel Aspect Ratio: " << S.pixel_aspect << std::endl;
    if (T.tlength)
      std::cout << "Thumb size:  " << T.twidth << " x " << T.theight << std::endl;
    std::cout << "Full size:   " << S.raw_width << " x " << S.raw_height << std::endl;

    std::cout << "Image size:  " << S.width << " x " << S.height << std::endl;
    std::cout << "Output size: " << S.iwidth << " x " << S.iheight << std::endl;
    std::cout << "Raw colors: " << P1.colors << std::endl;
    if (P1.filters) {
      std::cout << "Filter pattern: ";
      if (!P1.cdesc[3]) P1.cdesc[3] = 'G';
      for (int i = 0; i < 16; i++)
        std::cout << P1.cdesc[m_raw.fcol( i >> 1, i & 1 )];
      std::cout << std::endl;
    }
    std::cout << "Daylight multipliers:";
    for (int c = 0; c<P1.colors; c++) 
      std::cout 
      << " "
      << std::fixed << std::setprecision( 6 ) << std::setw( 11 ) << std::setfill( ' ' )
      << C.pre_mul[c];
    std::cout << std::endl;

    if (C.cam_mul[0] > 0) {
      std::cout << "Camera multipliers: ";
      for (int c = 0; c<4; c++) 
        std::cout 
        << " "
        << std::fixed << std::setprecision( 6 ) << std::setw( 11 ) << std::setfill( ' ' )
        << C.cam_mul[c];
      std::cout << std::endl;
    }

    std::cout << "Cam->XYZ matrix:" << std::endl;
    for (int i = 0; i < 4; i++)
      std::cout 
      << std::fixed << std::setprecision( 4 ) << std::setw( 8 ) << std::setfill( ' ' )
      << C.cam_xyz[i][0] << " " << C.cam_xyz[i][1] << " " << C.cam_xyz[i][2] << std::endl;
        /*
        }
      else
      {
        if (print_unpack)
        {
          char frame[48] = "";
          if (print_frame)
          {
            ushort right_margin = S.raw_width - S.width - S.left_margin;
            ushort bottom_margin = S.raw_height - S.height - S.top_margin;
            snprintf( frame, 48, "F=%dx%dx%dx%d RS=%dx%d",
              S.left_margin, S.top_margin, right_margin, bottom_margin,
              S.raw_width, S.raw_height );
            printf( "%s\t%s\t%s\t%s/%s\n",
              av[i],
              MyCoolRawProcessor.unpack_function_name( ),
              frame,
              P1.make, P1.model );
          }
        }
        else
          printf( "%s is a %s %s image.\n", av[i], P1.make, P1.model );
      }
      */
      //MyCoolRawProcessor.recycle( );
    //}// endfor
  }
}
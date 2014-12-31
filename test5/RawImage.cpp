/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

#include "stdafx.h"

#include <boost/timer/timer.hpp>
//#include <boost/chrono/system_clocks.hpp>

#include "RawImage.h"

RawImage::RawImage() {
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

libraw_processed_image_t* RawImage::ObtainImage( const std::string& sFileName ) {

  int i;
  int ret;
  int verbose = 0;

  LibRaw m_raw;

  // don't use fixed size buffers in real apps!
  //  char outfn[1024], thumbfn[1024];

  boost::timer::auto_cpu_timer t;

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

  //#define P1  RawProcessor.imgdata.idata
  //#define S   RawProcessor.imgdata.sizes
  //#define C   RawProcessor.imgdata.color
  //#define T   RawProcessor.imgdata.thumbnail
  //#define P2  RawProcessor.imgdata.other
  //#define OUT RawProcessor.imgdata.params

  m_raw.imgdata.params.output_tiff = 1;
  verbose++;
  //        OUT.output_bps = 16;  // bits per colour
  m_raw.set_progress_handler( &::my_progress_callback, NULL );

  //if (verbose) printf( "Processing file %s\n", av[i] );

  if (LIBRAW_SUCCESS != (ret = m_raw.open_file( sFileName.c_str( ) ))) {
    throw std::runtime_error( "Cannot open " + sFileName + " " + libraw_strerror( ret ) );
  }

  boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now( );

  if (LIBRAW_SUCCESS != (ret = m_raw.unpack( ))) {
    throw std::runtime_error( "Cannot unpack " + sFileName + " " + libraw_strerror( ret ) );
  }

  if (0 != m_OnLibRawOutputParams) m_OnLibRawOutputParams( m_raw.imgdata.params );

  ret = m_raw.dcraw_process( );
  if (LIBRAW_SUCCESS != ret) {
    throw std::runtime_error( "Cannot do postprocessing on " + sFileName + libraw_strerror( ret ) );
    // if (LIBRAW_FATAL_ERROR( ret )) ... ?
  }

  /*
  snprintf( outfn, sizeof(outfn),
  "%s.%s",
  av[i], OUT.output_tiff ? "tiff" : (P1.colors>1 ? "ppm" : "pgm") );

  if (verbose) printf( "Writing file %s\n", outfn );

  if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer( outfn )))
  fprintf( stderr, "Cannot write %s: %s\n", outfn, libraw_strerror( ret ) );
  */

  libraw_processed_image_t* img = m_raw.dcraw_make_mem_image( &ret );
  if (LIBRAW_SUCCESS != ret) {
    throw std::runtime_error( "Cannot make memory image " + sFileName + libraw_strerror( ret ) );
    // if (LIBRAW_FATAL_ERROR( ret )) ... ?
  }

  boost::chrono::nanoseconds nsec = boost::chrono::system_clock::now( ) - start;
  std::cout << "RawProcessor was alive for\n " << nsec.count( ) << " nanoseconds\n";

  m_raw.recycle( );

  return img;
}

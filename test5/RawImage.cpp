#include "stdafx.h"

#include <boost/timer/timer.hpp>
//#include <boost/chrono/system_clocks.hpp>

#include "RawImage.h"

RawImage::RawImage() {
}

RawImage::~RawImage() {
}

int my_progress_callback( void *unused_data, enum LibRaw_progress state, int iter, int expected ) {
  if (iter == 0)
    printf( "CB: state=%x, expected %d iterations\n", state, expected );
  return 0;
}

libraw_processed_image_t* RawImage::ObtainImage( const std::string& sFileName ) {

  int i;
  int ret;
  int verbose = 0;

  // don't use fixed size buffers in real apps!
  //  char outfn[1024], thumbfn[1024];

  LibRaw RawProcessor;

  boost::timer::auto_cpu_timer t;

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

  putenv( (char*)"TZ=UTC" ); // dcraw compatibility, affects TIFF datestamp field

  //#define P1  RawProcessor.imgdata.idata
  //#define S   RawProcessor.imgdata.sizes
  //#define C   RawProcessor.imgdata.color
  //#define T   RawProcessor.imgdata.thumbnail
  //#define P2  RawProcessor.imgdata.other
  //#define OUT RawProcessor.imgdata.params

  RawProcessor.imgdata.params.output_tiff = 1;
  verbose++;
  //        OUT.output_bps = 16;  // bits per colour
  RawProcessor.set_progress_handler( &::my_progress_callback, NULL );

  //if (verbose) printf( "Processing file %s\n", av[i] );

  if (LIBRAW_SUCCESS != (ret = RawProcessor.open_file( sFileName.c_str( ) ))) {
    throw std::runtime_error( "Cannot open " + sFileName + " " + libraw_strerror( ret ) );
  }

  //RawProcessor.imgdata.params.no_auto_scale = 1; //disables scaling from camera maximum to 64k
  RawProcessor.imgdata.params.no_auto_bright = 1; //disables auto brighten
  RawProcessor.imgdata.params.use_auto_wb = 0;
  RawProcessor.imgdata.params.use_camera_wb = 1;
  RawProcessor.imgdata.params.output_color = 1;  // sRGB  [0-5] Output colorspace (raw, sRGB, Adobe, Wide, ProPhoto, XYZ). 
  //    RawProcessor.
  //      LibRaw_image_formats

  /*
  int user_qual;
  dcraw keys : -q
  0 - 10 : interpolation quality :

  0 - linear interpolation
  1 - VNG interpolation
  2 - PPG interpolation
  3 - AHD interpolation
  4 - DCB interpolation
  5 - Modified AHD interpolation by Paul Lee
  6 - AFD interpolation( 5 - pass )
  7 - VCD interpolation
  8 - Mixed VCD / Modified AHD interpolation
  9 - LMMSE interpolation
  10 - AMaZE intepolation

  Values 5 - 9 are useful only if "LibRaw demosaic pack GPL2" compiled in.
  Value 10 is useful only if LibRaw compiled with "LibRaw demosaic pack GPL3".
  If some interpolation method is unsupported because LibRaw compiled without corresponding demosaic pack, AHD interpolation will be used without any notice.
  */

  boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now( );

  if (LIBRAW_SUCCESS != (ret = RawProcessor.unpack( ))) {
    throw std::runtime_error( "Cannot unpack " + sFileName + " " + libraw_strerror( ret ) );
  }

  ret = RawProcessor.dcraw_process( );
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

  libraw_processed_image_t* img = RawProcessor.dcraw_make_mem_image( &ret );
  if (LIBRAW_SUCCESS != ret) {
    throw std::runtime_error( "Cannot make memory image " + sFileName + libraw_strerror( ret ) );
    // if (LIBRAW_FATAL_ERROR( ret )) ... ?
  }

  boost::chrono::nanoseconds sec = boost::chrono::system_clock::now( ) - start;
  std::cout << "RawProcessor was alive for\n " << sec.count( ) << " nanoseconds\n";

  RawProcessor.recycle( ); // just for show this call

  return img;
}

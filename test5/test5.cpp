/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

#include "stdafx.h"

/*
  2014/12/29
    * currently loads a Nikon RAW image file with a hard coded file name
    * the image is demosaiced with mostly default settings
    * the image is then sent over to FramePicture for interactive viewing
*/

#include <string>
#include <iostream>

#include <wx/panel.h>

#include "test5.h"

IMPLEMENT_APP( AppTest5 )

std::string sFileName( "D:\\Data\\Projects\\VC++\\Graphics\\_DSC4256.NEF" );

bool AppTest5::OnInit() {

  if (!wxApp::OnInit( ))
    return false;

  framePicture = new FramePicture( (wxFrame *)NULL, wxID_ANY, wxT( "Test5" ), wxPoint( 300, 50 ), wxSize( 1000, 800 ), wxDEFAULT_FRAME_STYLE );  // could use wxDefaultPosition
  frameLibRawOptions = new FrameLibRawOptions( framePicture, wxID_ANY, wxT( "Demosaic Options" ), wxPoint( 50, 50 ), wxDefaultSize, wxDEFAULT_FRAME_STYLE );  // could use wxDefaultPosition )

  m_user_qual = FrameLibRawOptions::Unknown;

  frameLibRawOptions->SetSelectInteropolationHandler( 
    fastdelegate::MakeDelegate( this, &AppTest5::HandleDemosaicSelection ) );
  m_ri.SetLibRawOutputParams( fastdelegate::MakeDelegate( this, &AppTest5::SetLibRawOutputParams ) );

  //DemosaicImage();

//  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
//  wxPanel* panel = new wxPanel( frame );
//  sizer->Add( panel, 1, wxEXPAND );

//  frame->SetSizer( sizer );
//  frame->SetAutoLayout( true );

  wxApp::SetTopWindow( framePicture );
  framePicture->Show();
  frameLibRawOptions->Show();

  return 1;
}

void AppTest5::SetLibRawOutputParams( libraw_output_params_t& params ) {

  //RawProcessor.imgdata.params.no_auto_scale = 1; //disables scaling from camera maximum to 64k
  params.no_auto_bright = 1; //disables auto brighten
  params.use_auto_wb = 0;
  params.use_camera_wb = 1;
  params.output_color = 1;  // sRGB  [0-5] Output colorspace (raw, sRGB, Adobe, Wide, ProPhoto, XYZ). 
  params.user_qual = m_user_qual;

}

void AppTest5::DemosaicImage( void ) {

  //libraw_output_params_t& params( m_ri.Params() );

  try {
    libraw_processed_image_t* image = m_ri.ObtainImage( sFileName );   // todo: cache the image when possible
    if ((LIBRAW_IMAGE_BITMAP == image->type) && (3 == image->colors) && (8 == image->bits)) {
      wxImage* pImage( new wxImage( image->width, image->height, image->data, true ) );
      //wxBitmap* pBitmap( new wxBitmap( *pImage ) );
      framePicture->SetPicture( pImage );
      // todo: need to do significant memory clean up here
    }
  }
  catch (...) {
    std::cout << "failed image" << std::endl;
  }
}

void AppTest5::HandleDemosaicSelection( FrameLibRawOptions::eInterpolation_t interp ) {

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

  //libraw_output_params_t& params( m_ri.Params( ) );
  //params.user_qual = interp;
  m_user_qual = interp;
  DemosaicImage();
}

int AppTest5::OnExit( void ) {
  return 0;
}

void AppTest5::OnClose( wxCloseEvent& event ) {
  frameLibRawOptions->SetSelectInteropolationHandler( 0 );
  event.Skip( true );  // auto followed by Destroy();
}



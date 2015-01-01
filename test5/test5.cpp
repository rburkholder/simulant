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
    * the image is then sent over to PanelPicture for interactive viewing
*/

#include <string>
#include <iostream>

#include "test5.h"

IMPLEMENT_APP( AppTest5 )

std::string sFileName( "D:\\Data\\Projects\\VC++\\Graphics\\_DSC4256.NEF" );

bool AppTest5::OnInit() {

  if (!wxApp::OnInit( ))
    return false;

  frameMain = new wxFrame( (wxFrame *)NULL, wxID_HIGHEST + 1000, wxT( "Test5" ), wxPoint( 300, 50 ), wxSize( 1000, 800 ), wxDEFAULT_FRAME_STYLE );
  wxApp::SetTopWindow( frameMain );

  Bind( wxEVT_MOUSEWHEEL, &AppTest5::OnMouseWheel1, this );
  //frameMain->Bind( wxEVT_MOUSEWHEEL, &AppTest5::OnMouseWheel2, this );

  panelLibRawOptions = new PanelLibRawOptions( frameMain, wxID_HIGHEST + 1001, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE );
  panelPicture = new PanelPicture( frameMain, wxID_HIGHEST + 1002, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE );
  panelLogging = new ou::PanelLogging( frameMain, wxID_HIGHEST + 1003, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE ); 

  //m_user_qual = PanelLibRawOptions::Unknown;

  panelLibRawOptions->SetOptionHandler(
    fastdelegate::MakeDelegate( this, &AppTest5::HandleDemosaicSelection ) );
  m_ri.SetLibRawOutputParams( fastdelegate::MakeDelegate( this, &AppTest5::SetLibRawOutputParams ) );

  wxBoxSizer* sizerVertical = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* sizerPictureStuff = new wxBoxSizer( wxHORIZONTAL );
  sizerVertical->Add( sizerPictureStuff, 6, wxEXPAND, 0 );

  //sizer->Add( panelLibRawOptions, 1, wxEXPAND | wxALL, 2 );
  sizerPictureStuff->Add( panelLibRawOptions, 1, wxALIGN_LEFT | wxALIGN_TOP | wxALL, 2 );
  sizerPictureStuff->Add( panelPicture, 4, wxEXPAND | wxALL, 2 );

  sizerVertical->Add( panelLogging, 1, wxEXPAND, 0 );

  frameMain->SetSizer( sizerVertical );
  frameMain->SetAutoLayout( true );

  frameMain->Show();

  return 1;
}

void AppTest5::OnMouseWheel1( wxMouseEvent& event ) {
  if (panelPicture->IsMouseInWindow()) {
    panelPicture->OnMouseWheel( event );
  }
  else {
    event.Skip( true );
  }
}

//void AppTest5::OnMouseWheel2( wxMouseEvent& event ) {
//  event.Skip( true );
//}

void AppTest5::SetLibRawOutputParams( libraw_output_params_t& params ) {

  //RawProcessor.imgdata.params.no_auto_scale = 1; //disables scaling from camera maximum to 64k
  params.no_auto_bright = 1; //disables auto brighten
  params.no_auto_scale = 1;
  params.use_auto_wb = 0;
  params.use_camera_wb = 1;
  params.user_qual = m_options.eInterpolation;
  //params.output_color = 1;  // sRGB  [0-5] Output colorspace (raw, sRGB, Adobe, Wide, ProPhoto, XYZ). 
  params.output_color = m_options.eColourSpace;
  params.threshold = (float)m_options.intNoiseThreshold;
  params.med_passes = m_options.nMedianFilterPasses;

}

void AppTest5::LoadImage( void ) {

  //libraw_output_params_t& params( m_ri.Params() );

  try {
    libraw_processed_image_t* image = m_ri.ObtainImage( sFileName );   // todo: cache the image when possible
    if ((LIBRAW_IMAGE_BITMAP == image->type) && (3 == image->colors) && (8 == image->bits)) {
      wxImage* pImage( new wxImage( image->width, image->height, image->data, true ) );
      //wxBitmap* pBitmap( new wxBitmap( *pImage ) );
      panelPicture->SetPicture( pImage );
      // todo: need to do significant memory clean up here
    }
  }
  catch (...) {
    std::cout << "failed image" << std::endl;
  }
}

void AppTest5::HandleDemosaicSelection( const PanelLibRawOptions::options_t& options ) {

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
  //m_user_qual = options.eInterpolation;
  m_options = options;
  LoadImage( );
}

int AppTest5::OnExit( void ) {
  return 0;
}

void AppTest5::OnClose( wxCloseEvent& event ) {
  panelLibRawOptions->SetOptionHandler( 0 );
  event.Skip( true );  // auto followed by Destroy();
}



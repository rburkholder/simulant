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

  panelLibRawOptions->SetOptionHandler(
    fastdelegate::MakeDelegate( this, &AppTest5::HandleDemosaicSelection ) );
  m_ri.SetLibRawOutputParams( fastdelegate::MakeDelegate( this, &AppTest5::SetLibRawOutputParams ) );

  wxBoxSizer* sizerVertical = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* sizerPictureStuff = new wxBoxSizer( wxHORIZONTAL );
  sizerVertical->Add( sizerPictureStuff, 8, wxEXPAND, 0 );

  sizerPictureStuff->Add( panelLibRawOptions, 1, wxALIGN_LEFT | wxALIGN_TOP | wxALL, 2 );
  sizerPictureStuff->Add( panelPicture, 4, wxEXPAND | wxALL, 2 );

  sizerVertical->Add( panelLogging, 1, wxEXPAND, 0 );

  frameMain->SetSizer( sizerVertical );
  frameMain->SetAutoLayout( true );

  frameMain->Show();

  try {
    m_ri.LoadImage( sFileName );
    std::cout << std::endl;  // clears the auto timer
    m_ri.FileInfo();
  }
  catch ( std::runtime_error& e ) {
    std::cout << e.what() << std::endl;
  }

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
  params.output_tiff = 1;
  params.no_auto_bright = !m_options.bAutoBrighten; //disables auto brighten
  //params.no_auto_scale = 1;  // enables green cast, disables scaling from camera maximum to 64k
  params.use_auto_wb = m_options.bAutoWhiteBalance;
  params.use_camera_wb = m_options.bUseCameraWhiteBalance;
  params.user_qual = m_options.eInterpolation;
  params.output_color = m_options.eColourSpace;
  params.threshold = (float)m_options.intNoiseThreshold;
  params.med_passes = m_options.nMedianFilterPasses;
}

void AppTest5::LoadImage( void ) {

  try {
    libraw_processed_image_t* image = m_ri.CalcImage(); 
    if ((LIBRAW_IMAGE_BITMAP == image->type) && (3 == image->colors) && (8 == image->bits)) {
      wxImage imageTemp( image->width, image->height, image->data, true );  // static data is true
      boost::shared_ptr<wxImage> pImage( new wxImage( imageTemp.GetWidth(), imageTemp.GetHeight(), false ) );
      pImage->Paste( imageTemp, 0, 0 );   // create a copy of the image
      panelPicture->SetPicture( pImage );
    }
    m_ri.FreeImage( image );
  }
  catch (std::runtime_error& e) {
    std::cout << "failed image: " << e.what() << std::endl;
  }
}

void AppTest5::HandleDemosaicSelection( const PanelLibRawOptions::options_t& options ) {

  // options are supplied through callback to SetLibRawOutputParams
  m_options = options;
  LoadImage( );
  std::cout << std::endl;  // clears the auto timer

}

int AppTest5::OnExit( void ) {
  return 0;
}

void AppTest5::OnClose( wxCloseEvent& event ) {
  panelLibRawOptions->SetOptionHandler( 0 );
  event.Skip( true );  // auto followed by Destroy();
}



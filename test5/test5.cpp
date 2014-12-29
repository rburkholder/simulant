/*
  Copywrite (2014) Raymond Burkholder
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

#include "RawImage.h"
#include "FramePicture.h"
#include "test5.h"

IMPLEMENT_APP( AppTest5 )

std::string sFileName( "D:\\Data\\Projects\\VC++\\Graphics\\_DSC4256.NEF" );

bool AppTest5::OnInit() {

  if (!wxApp::OnInit( ))
    return false;

  FramePicture* frame = new FramePicture( (wxFrame *)NULL, wxID_ANY, wxT( "Test5" ), wxPoint( 50, 50 ), wxSize( 1000, 800 ), wxDEFAULT_FRAME_STYLE );  // could use wxDefaultPosition

  try {
    libraw_processed_image_t* image = RawImage::ObtainImage( sFileName );
    if ((LIBRAW_IMAGE_BITMAP == image->type) && (3 == image->colors) && (8 == image->bits)) {
      wxImage* pImage( new wxImage( image->width, image->height, image->data, true ) );
      //wxBitmap* pBitmap( new wxBitmap( *pImage ) );
      frame->SetPicture( pImage );
      // todo: need to do significant memory clean up here
    }
  }
  catch (...) {
    std::cout << "failed image" << std::endl;
  }


//  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
//  wxPanel* panel = new wxPanel( frame );
//  sizer->Add( panel, 1, wxEXPAND );

//  frame->SetSizer( sizer );
//  frame->SetAutoLayout( true );

  frame->Show( );

  return 1;
}

int AppTest5::OnExit() {
  return 0;
}

void AppTest5::OnClose( wxCloseEvent& event ) {
  event.Skip( true );  // auto followed by Destroy();
}



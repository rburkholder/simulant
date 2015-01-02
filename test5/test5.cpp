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

#include <wx/splitter.h>

#include "test5.h"

IMPLEMENT_APP( AppTest5 )

std::string sPath( "D:\\Data\\Projects\\VC++\\Graphics" );
std::string sFile( "_DSC4256.NEF" );
std::string sFileName( sPath + "\\" + sFile );

bool AppTest5::OnInit() {

  if (!wxApp::OnInit( ))
    return false;

  m_frameMain = new wxFrame( (wxFrame *)NULL, ID_frameMain, wxT( "Test5" ), wxPoint( 300, 50 ), wxSize( 1000, 800 ), wxDEFAULT_FRAME_STYLE );
  wxApp::SetTopWindow( m_frameMain );

  m_mbMain = new wxMenuBar;
  m_menuFile = new wxMenu;
  m_menuFile->Append( ID_menuFileOpen, _( "&Open" ), wxEmptyString, wxITEM_NORMAL );
  m_menuFile->Append( ID_menuFileExit, _( "e&Xit" ), wxEmptyString, wxITEM_NORMAL );
  m_mbMain->Append( m_menuFile, _( "&File" ) );
  m_frameMain->SetMenuBar( m_mbMain );

  wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
  m_frameMain->SetSizer( sizerMain );

  wxSplitterWindow* swMain = new wxSplitterWindow( m_frameMain, ID_swMain, wxDefaultPosition, wxSize( 100, 100 ), wxSP_3DSASH | wxNO_BORDER );
  // wxSP_3DBORDER | wxSP_3DSASH | wxSUNKEN_BORDER
  swMain->SetMinimumPaneSize( 20 );
  swMain->SetSashGravity( 0.9 );

  wxPanel* panelSplitterUpper = new wxPanel( swMain, ID_panelSplitterTop, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );
  wxBoxSizer* sizerSplitterUpper = new wxBoxSizer( wxHORIZONTAL );
  panelSplitterUpper->SetSizer( sizerSplitterUpper );

  m_panelLibRawOptions = new PanelLibRawOptions( panelSplitterUpper, ID_panelLibRawOptions, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );
  sizerSplitterUpper->Add( m_panelLibRawOptions, 0, wxGROW |wxALIGN_LEFT | wxALIGN_TOP | wxALL, 2 );

  m_panelPicture = new PanelPicture( panelSplitterUpper, ID_panelPicture, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  sizerSplitterUpper->Add( m_panelPicture, 1, wxGROW|wxEXPAND | wxALL, 0 );

  m_panelLogging = new ou::PanelLogging( swMain, ID_panelLogging, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );

  swMain->SplitHorizontally( panelSplitterUpper, m_panelLogging, 600 );
  sizerMain->Add( swMain, 1, wxGROW | wxALL | wxEXPAND, 0 );
  
  m_sbMain = new wxStatusBar( m_frameMain, ID_sbMain, wxST_SIZEGRIP | wxNO_BORDER );
  m_sbMain->SetFieldsCount( 2 );
  sizerMain->Add( m_sbMain, 0, wxGROW | wxALL, 0 );

  m_panelLibRawOptions->SetOptionHandler(
    fastdelegate::MakeDelegate( this, &AppTest5::HandleDemosaicSelection ) );
  m_ri.SetLibRawOutputParams( fastdelegate::MakeDelegate( this, &AppTest5::SetLibRawOutputParams ) );

  m_frameMain->SetAutoLayout( true );

  Bind( wxEVT_CLOSE_WINDOW, &AppTest5::OnClose, this );

  Bind( wxEVT_MENU, &AppTest5::OnMenuFileOpenClick, this, ID_menuFileOpen );
  Bind( wxEVT_MENU, &AppTest5::OnMenuFileExitClick, this, ID_menuFileExit );
  Bind( wxEVT_MOUSEWHEEL, &AppTest5::OnMouseWheel1, this );
  //frameMain->Bind( wxEVT_MOUSEWHEEL, &AppTest5::OnMouseWheel2, this );

  m_frameMain->Show( );

  return 1;
}

void AppTest5::OnMenuFileOpenClick( wxCommandEvent& event ) {
  wxFileDialog openFileDialog( m_frameMain, _( "Open File ..." ), sPath, sFile, "", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
  if (wxID_CANCEL == openFileDialog.ShowModal()) return;
  wxString wxsFilePath( openFileDialog.GetPath() );
  std::string sFilePath( wxsFilePath );
  try {
    m_ri.LoadImage( sFilePath );
    std::cout << std::endl;  // clears the auto timer
    m_ri.FileInfo( );
  }
  catch (std::runtime_error& e) {
    std::cout << e.what( ) << std::endl;
  }

}

void AppTest5::OnMenuFileExitClick( wxCommandEvent& event ) {
  // Exit Steps:  #1 -> Appxxx::OnClose
  m_frameMain->Close( );
}

void AppTest5::OnMouseWheel1( wxMouseEvent& event ) {
  if (m_panelPicture->IsMouseInWindow( )) {
    m_panelPicture->OnMouseWheel( event );
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
      m_panelPicture->SetPicture( pImage );
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

  // isn't called, needs to be in main frame
  m_panelLibRawOptions->SetOptionHandler( 0 );

  // Exit Steps: #3 -> Appxxx::OnExit
  Unbind( wxEVT_CLOSE_WINDOW, &AppTest5::OnClose, this );
  //  Unbind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );  // causes crash
  // http://docs.wxwidgets.org/trunk/classwx_close_event.html
  event.Skip( );  // continue with base class stuff
}



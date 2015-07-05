

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <boost/filesystem/operations.hpp>

#include <wx/wx.h>
#include <wx/display.h>
#include <wx/sizer.h>

#include "MediaStreamDecode.h"

#include "PanelSurfaceSources.h"
#include "PanelLogging.h"

#include "AppProjection.h"

IMPLEMENT_APP( AppProjection )

bool AppProjection::OnInit( ) {
  
  wxImage::AddHandler( new wxPNGHandler );
  wxImage::AddHandler( new wxJPEGHandler );
  
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;
  
  m_sWorkingDirectory = boost::filesystem::current_path().string();
  
  m_sMediaDirectory = "~/";
  
  m_pFrameMain = new FrameMain( (wxFrame *)NULL, -1, wxT( "Projection Demo" ), wxPoint( 10, 10 ), wxSize( 500, 500 ) );
  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppProjection::OnClose, this );  // start close of windows and controls

  namespace args = boost::phoenix::arg_names;
  m_MenuActionLoad = m_pFrameMain->m_MenuItemLoad.connect( boost::phoenix::bind( &AppProjection::HandleLoad, this ) );
  m_MenuActionSave = m_pFrameMain->m_MenuItemSave.connect( boost::phoenix::bind( &AppProjection::HandleSave, this ) );

  FrameMain::vpMenuItems_t vMenuItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vMenuItems.push_back( new mi( "Video File Stats", boost::phoenix::bind( &AppProjection::MediaFileStats, this ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vMenuItems );
  
//  vMenuItems.clear();
//  vMenuItems.push_back( new mi( "Canvas", boost::phoenix::bind( &AppProjection::CreateCanvas, this ) ) );
//  vMenuItems.push_back( new mi( "Movie Screen", boost::phoenix::bind( &AppProjection::CreateMovieScreen, this ) ) );
//  m_pFrameMain->AddDynamicMenu( "Create", vMenuItems );
  
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  
  m_pSurfaceSources = new PanelSurfaceSources( m_pFrameMain, -1 );
  
  unsigned int nDisplays = wxDisplay::GetCount();
  std::cout << "#connected displays: " << nDisplays << std::endl;
  
  for ( unsigned int ix = 0; ix < nDisplays; ++ix ) {
    
    wxDisplay display( ix );
    wxRect rectClientArea = display.GetClientArea();

    std::cout << "building on " << rectClientArea.x << ", " << rectClientArea.y << std::endl;
    
    // need to keep track of projection frames, so can iconize them sometime for visual reference in the gui
    // force frame size for the time being
#ifdef __WXMSW__
    m_pSurfaceSources->Append( pPhysicalDisplay_t( new PhysicalDisplay( ix, m_pFrameMain, wxPoint( rectClientArea.x, rectClientArea.y ), wxSize( 1000, 720 ) ) ) );
#else
    m_pSurfaceSources->Append( pPhysicalDisplay_t( new PhysicalDisplay( ix, m_pFrameMain, wxPoint( rectClientArea.x, rectClientArea.y ), wxSize( 1920, 1080 ) ) ) );
#endif
  }
  
  m_pFrameMain->SetSizer( sizer );
  m_pFrameMain->SetAutoLayout( true );

  sizer->Add( m_pSurfaceSources, 3, wxEXPAND|wxGROW|wxALL );

  ou::tf::PanelLogging* p = new ou::tf::PanelLogging(m_pFrameMain, wxID_ANY );
  sizer->Add( p, 1, wxEXPAND | wxGROW | wxALL );

  m_pFrameMain->Show( );
  // serialize the following for session to session persistence
#ifdef __WXMSW__
  m_pFrameMain->Move( wxPoint( 1078, 224 ) );  // testing on laptop
#else
  m_pFrameMain->Move( wxPoint( 1950, 150 ) );  // testing on desktop
#endif
  
  // templates for windows requiring them
//  wxApp::Bind( EVENT_IMAGE, &AppProjection::HandleEventImage, this );
//  wxApp::Bind( wxEVT_MOTION, &AppProjection::HandleMouseMoved, this );
//  wxApp::Bind( wxEVT_MOUSEWHEEL, &AppProjection::HandleMouseWheel, this );
//  wxApp::Bind( wxEVT_LEFT_DOWN, &AppProjection::HandleMouseLeftDown, this );
//  wxApp::Bind( wxEVT_LEFT_UP, &AppProjection::HandleMouseLeftUp, this );
//  wxApp::Bind( wxEVT_RIGHT_DOWN, &AppProjection::HandleMouseRightDown, this );
  //wxApp::Bind( wxEVT_KEY_DOWN, &AppProjection::HandleKey, this );
  //wxApp::Bind( wxEVT_KEY_UP, &AppProjection::HandleKey, this );
  wxApp::Bind( wxEVT_CHAR, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_CHAR_HOOK, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_ENTER_WINDOW, &AppProjection::HandleEnterWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_LEAVE_WINDOW, &AppProjection::HandleLeaveWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_SIZE, &AppProjection::HandleMouseMoved, this );  // window specific, not in app
  
  m_pAudio.reset( new Audio );
  m_pSurfaceSources->Add( m_pAudio );
  
  return true;
}

void AppProjection::HandleSave( void ) {
  std::cout << "Saving ..." << std::endl;
  std::ofstream ofs( m_sWorkingDirectory + "/jackson.show" );
  boost::archive::text_oarchive oa(ofs);
  oa << *this;
  m_pSurfaceSources->Save( oa );
  std::cout << "  done." << std::endl;
}

void AppProjection::HandleLoad( void ) {
  try {
    std::cout << "Loading ..." << std::endl;
    std::ifstream ifs( m_sWorkingDirectory + "/jackson.show" );
    boost::archive::text_iarchive ia(ifs);
    ia >> *this;
    m_pSurfaceSources->Load( ia );
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppProjection::MediaFileStats( void ) {
  
  wxFileDialog dialogOpenFile( 
    m_pFrameMain, wxT("Select Media" ), m_sMediaDirectory, "Media Stats", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sMediaDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    //std::cout << "dir " << m_sMediaDirectory << std::endl;
    //assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
    
    std::string s( dialogOpenFile.GetPath() );
    
    MediaStreamDecode media;
    if ( media.Open( s ) ) {
      media.EmitStats();
      media.Close();
    }
    
    
  }
  else {
    
  }

}

// http://docs.wxwidgets.org/trunk/classwx_key_event.html
void AppProjection::HandleKey( wxKeyEvent& event ) {
  //if ( event.GetKeyCode() = )
  // interacts with TreeDisplayManager::HandleItemActivated, may need to adjust skip on certain things
  // ctrl, alt keys do pass through
  //std::cout << "key='" << event.GetKeyCode() << "'" << std::endl;
  event.Skip();
}

void AppProjection::OnClose( wxCloseEvent& event ) {
  m_MenuActionLoad.disconnect();
  m_MenuActionSave.disconnect();
  // stuff to do on window close
  std::cout << "OnClose" << std::endl;
  // need to put this elsewhere so queues and gui can finish off?
  //m_timerGuiRefresh.Stop();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

int AppProjection::OnExit( void ) {
  // stuff to do on program exit
  std::cout << "OnExit" << std::endl;
  return 0;
}


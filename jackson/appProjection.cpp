

#include <cstdlib>
#include <iostream>

#include <wx/wx.h>
#include <wx/display.h>
#include <wx/sizer.h>


#include "panelSurfaceSources.h"
#include "appProjection.h"

IMPLEMENT_APP( AppProjection )

bool AppProjection::OnInit( ) {
  
  m_pFrameMain = new FrameMain( (wxFrame *)NULL, -1, wxT( "Projection Demo" ), wxPoint( 10, 10 ), wxSize( 500, 250 ) );
  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppProjection::OnClose, this );  // start close of windows and controls

//  FrameMain::vpMenuItems_t vMenuItems;
//  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vMenuItems.push_back( new mi( "select picture", boost::phoenix::bind( &AppProjection::LoadPicture, this ) ) );
//  vMenuItems.push_back( new mi( "select video", boost::phoenix::bind( &AppProjection::LoadVideo, this ) ) );
//  vMenuItems.push_back( new mi( "image->opengl", boost::phoenix::bind( &AppProjection::Image2OpenGL, this ) ) ) ;
//  m_pFrameMain->AddDynamicMenu( "Actions", vMenuItems );
  
//  vMenuItems.clear();
//  vMenuItems.push_back( new mi( "Canvas", boost::phoenix::bind( &AppProjection::CreateCanvas, this ) ) );
//  vMenuItems.push_back( new mi( "Movie Screen", boost::phoenix::bind( &AppProjection::CreateMovieScreen, this ) ) );
//  m_pFrameMain->AddDynamicMenu( "Create", vMenuItems );
  
  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
  m_pSurfaceSources = new panelSurfaceSources( m_pFrameMain, -1 );
  sizer->Add( m_pSurfaceSources, 1, wxEXPAND );

  unsigned int nDisplays = wxDisplay::GetCount();
  std::cout << "#connected displays: " << nDisplays << std::endl;
  
  for ( int ix = 0; ix < nDisplays; ++ix ) {
    
    wxDisplay display( ix );
    wxRect rectClientArea = display.GetClientArea();

    std::cout << "building on " << rectClientArea.x << ", " << rectClientArea.y << std::endl;
    
    // need to keep track of projection frames, so can iconize them sometime for visual reference in the gui
    // force frame size for the time being
    m_pSurfaceSources->Append( pScreenFrame_t( new ScreenFrame( ix, m_pFrameMain, wxPoint( rectClientArea.x, rectClientArea.y ), wxSize( 1920, 1080 ) ) ) );
    
  }
  
  m_pFrameMain->SetSizer( sizer );
  m_pFrameMain->SetAutoLayout( true );

  m_pFrameMain->Show( );
  // serialize the following for session to session persistence
  m_pFrameMain->Move( wxPoint( 1950, 150 ) );
  
  // WX_GL_CORE_PROFILE limits to older 3.x version
//  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_CORE_PROFILE, WX_GL_DEPTH_SIZE, 16, 0 };
//  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
//  m_pTut1 = new tut1( m_vScreenFrame[1]->GetFrame(), argsCanvas );
//  m_pTut1->SetSize( 400, 400 );
//  m_pTut1->Move( 100, 100 );
  
  // templates for windows requiring them
//  wxApp::Bind( EVENT_IMAGE, &AppProjection::HandleEventImage, this );
//  wxApp::Bind( wxEVT_MOTION, &AppProjection::HandleMouseMoved, this );
//  wxApp::Bind( wxEVT_MOUSEWHEEL, &AppProjection::HandleMouseWheel, this );
//  wxApp::Bind( wxEVT_LEFT_DOWN, &AppProjection::HandleMouseLeftDown, this );
//  wxApp::Bind( wxEVT_LEFT_UP, &AppProjection::HandleMouseLeftUp, this );
//  wxApp::Bind( wxEVT_RIGHT_DOWN, &AppProjection::HandleMouseRightDown, this );
  //wxApp::Bind( wxEVT_KEY_DOWN, &AppProjection::HandleKey, this );
  //wxApp::Bind( wxEVT_KEY_UP, &AppProjection::HandleKey, this );
  //wxApp::Bind( wxEVT_CHAR, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_CHAR_HOOK, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_ENTER_WINDOW, &AppProjection::HandleEnterWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_LEAVE_WINDOW, &AppProjection::HandleLeaveWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_SIZE, &AppProjection::HandleMouseMoved, this );  // window specific, not in app
  
//  m_pSurfaceSources->Bind( wxEVT_TOGGLEBUTTON, &AppProjection::HandleToggleEditMode, this, panelSurfaceSources::ID_BTN_EDITMODE );
//  m_pSurfaceSources->Bind( wxEVT_BUTTON, &AppProjection::HandleUndo, this, panelSurfaceSources::ID_BTN_UNDO );
  
  return true;
}

// http://docs.wxwidgets.org/trunk/classwx_key_event.html
void AppProjection::HandleKey( wxKeyEvent& event ) {
  //if ( event.GetKeyCode() = )
  std::cout << event.GetKeyCode() << std::endl;
  event.Skip();
}

void AppProjection::OnClose( wxCloseEvent& event ) {
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


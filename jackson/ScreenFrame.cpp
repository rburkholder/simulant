/* 
 * File:   ScreenFrame.cpp
 * Author: rpb
 * 
 * Created on April 4, 2015, 8:40 PM
 */

#include "ScreenFrame.h"

ScreenFrame::ScreenFrame( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ ): 
  m_nDisplay( nDisplay ), m_display( nDisplay ), m_point( point_ ), m_size( size_ ) 
{
  // for debugging purposes
  wxVideoMode vmDisplay = m_display.GetCurrentMode();
  wxRect rectGeometry = m_display.GetGeometry();
  wxArrayVideoModes modes = m_display.GetModes();
  wxString sName = m_display.GetName();
  bool bPrimary = m_display.IsPrimary();

  // do the frame for projection display
  m_pFrame = new FrameProjection( parent, -1, "", wxPoint( 10, 10 ), wxSize( 10, 10 ) );  // use default point and size to for redraw to bypass bug
  m_pFrame->SetSize( size_ );
  m_pFrame->Move( point_ );
  m_pFrame->Show();
}
  
ScreenFrame::~ScreenFrame( ) {
}

//void ScreenFrame::Append( pCanvas_t pCanvas ) {
//  
//}
/* 
 * File:   ScreenFrame.cpp
 * Author: rpb
 * 
 * Created on April 4, 2015, 8:40 PM
 */

#include "ScreenFrame.h"

ScreenFrame::ScreenFrame( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ ): 
  display( nDisplay ), point( point_ ), size( size_ ) 
{
  // for debugging purposes
  wxVideoMode vmDisplay = display.GetCurrentMode();
  wxRect rectGeometry = display.GetGeometry();
  wxArrayVideoModes modes = display.GetModes();
  wxString sName = display.GetName();
  bool bPrimary = display.IsPrimary();

  // do the frame for projection display
  pFrame = new FrameProjection( parent, -1, "", wxPoint( 10, 10 ), wxSize( 10, 10 ) );  // use default point and size to for redraw to bypass bug
  pFrame->SetSize( size );
  pFrame->Move( point );
  pFrame->Show();
}
  
ScreenFrame::~ScreenFrame( ) {
}

void ScreenFrame::Append( pCanvas_t pCanvas ) {
  
}
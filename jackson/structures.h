/* 
 * File:   structures.h
 * Author: rpb
 *
 * Created on April 3, 2015, 12:01 PM
 */

#ifndef STRUCTURES_H
#define	STRUCTURES_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/display.h>

#include "frameProjection.h"

class Canvas {
public:
protected:
private:
  std::string sDescription;
};
typedef std::vector<Canvas> vCanvas_t;

class VisualPresentation {  // maybe subset of multi-monitor setup, or an individual remote monitor
public:
  VisualPresentation( unsigned int nDisplay, wxWindow* parent ): 
      VisualPresentation( nDisplay, parent, wxPoint( 0, 0 ), wxSize( 100, 200 ) ) {
  }
  VisualPresentation( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ ): 
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
  VisualPresentation( void ): display( 0 ), point( 0, 0 ), size( 100, 200 ) {
  }
  FrameProjection* GetFrame( void ) { return pFrame; }
protected:
private:
  std::string sDescription;
  wxDisplay display;  // non copyable
  wxPoint point;     // location in client area, bug work around in gtk3, chooses different location on primary display
  wxSize size;  // use default, but may override
  FrameProjection* pFrame;  // windowing system takes care of destruction
  vCanvas_t vCanvas;  
};

typedef boost::shared_ptr<VisualPresentation> pVisualPresentation_t;
typedef std::vector<pVisualPresentation_t> vVisualPresentation_t;



#endif	/* STRUCTURES_H */


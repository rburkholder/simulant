/* 
 * File:   ScreenFrame.h
 * Author: rpb
 *
 * Created on April 4, 2015, 8:40 PM
 */

#ifndef SCREENFRAME_H
#define	SCREENFRAME_H

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/display.h>

#include "frameProjection.h"
#include "CanvasBase.h"

//#include "structures.h"

class ScreenFrame {  
  // maybe subset of multi-monitor setup, or an individual remote monitor
  // maps to the pixels emitted by a single projector
  // remote controller may be used to populate the resolution here
  // will need to add remote machine information here to represent remote displays
  // map to local screen when performing edit mode functions,
  //   and then transmit to remote monitor
public:
  
  typedef CanvasBase::pCanvas_t pCanvas_t;
  
  typedef boost::shared_ptr<ScreenFrame> pScreenFrame_t;
  //typedef std::vector<pScreenFrame_t> vScreenFrame_t;
  
  ScreenFrame( unsigned int nDisplay, wxWindow* parent ): 
      ScreenFrame( nDisplay, parent, wxPoint( 0, 0 ), wxSize( 100, 200 ) ) {
  }
  ScreenFrame( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ );
      
  virtual ~ScreenFrame( void );
  
  FrameProjection* GetFrame( void ) { return pFrame; }
  
  void Append( pCanvas_t pCanvas );
  
protected:
private:

  // disallow default constructor  
  ScreenFrame( void ): display( 0 ), point( 0, 0 ), size( 100, 200 ) {  };
  
  typedef std::vector<pCanvas_t> vCanvas_t;
  
  std::string m_sDescription;
  wxDisplay display;  // non copyable
  wxPoint point;     // location in client area, bug work around in gtk3, chooses different location on primary display
  wxSize size;  // use default, but may override
  FrameProjection* pFrame;  // windowing system takes care of destruction
  //vCanvas_t vCanvas;  
};



#endif	/* SCREENFRAME_H */


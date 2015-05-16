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

#include "FrameProjection.h"
#include "CanvasBase.h"

//#include "structures.h"

class PhysicalDisplay {  
  // maybe subset of multi-monitor setup, or an individual remote monitor
  // maps to the pixels emitted by a single projector
  // remote controller may be used to populate the resolution here
  // will need to add remote machine information here to represent remote displays
  // map to local screen when performing edit mode functions,
  //   and then transmit to remote monitor
public:
  
  //typedef CanvasBase::pCanvas_t pCanvas_t;
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  
  PhysicalDisplay( unsigned int nDisplay, wxWindow* parent ): 
      PhysicalDisplay( nDisplay, parent, wxPoint( 0, 0 ), wxSize( 100, 200 ) ) {
  }
  PhysicalDisplay( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ );
      
  virtual ~PhysicalDisplay( void );
  
  FrameProjection* GetFrame( void ) { return m_pFrame; }
  
  //void Append( pCanvas_t pCanvas );
  int GetId( void ) { return m_nDisplay; }
  
protected:
private:

  // disallow default constructor  
  PhysicalDisplay( void ): m_display( 0 ), m_point( 0, 0 ), m_size( 100, 200 ) {  };
  
  //typedef std::vector<pCanvas_t> vCanvas_t;
  
  unsigned int m_nDisplay;
  std::string m_sDescription;
  wxDisplay m_display;  // non copyable
  wxPoint m_point;     // location in client area, bug work around in gtk3, chooses different location on primary display
  wxSize m_size;  // use default, but may override
  FrameProjection* m_pFrame;  // windowing system takes care of destruction
  //vCanvas_t m_vCanvas;  
};



#endif	/* SCREENFRAME_H */


/* 
 * File:   EventGenerateFrame.h
 * Author: rpb
 *
 * Created on April 18, 2015, 8:38 PM
 */

#pragma once

#include <wx/event.h>

#include "FpsGenerator.h"

class EventGenerateFrame: public wxEvent {
public:
  typedef FpsGenerator::FPS FPS;
  
  EventGenerateFrame( wxEventType eventType, int winid, FPS fps );
  virtual ~EventGenerateFrame( void );
  
  virtual EventGenerateFrame* Clone( void ) const { 
    //std::cout << "EventGenerateFrame Cloned" << std::endl;
    return new EventGenerateFrame( *this ); 
  };

protected:  
private:
  FPS m_fps;
};

wxDECLARE_EVENT( EVENT_GENERATEFRAME, EventGenerateFrame );

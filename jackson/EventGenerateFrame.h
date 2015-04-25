/* 
 * File:   EventGenerateFrame.h
 * Author: rpb
 *
 * Created on April 18, 2015, 8:38 PM
 */

#pragma once

#include <wx/event.h>

class EventGenerateFrame: public wxEvent {
public:
  EventGenerateFrame( wxEventType eventType, int winid );
  virtual ~EventGenerateFrame( void );
  
  virtual EventGenerateFrame* Clone( void ) const { 
    //std::cout << "EventGenerateFrame Cloned" << std::endl;
    return new EventGenerateFrame( *this ); 
  };

protected:  
private:

};

wxDECLARE_EVENT( EVENT_GENERATEFRAME, EventGenerateFrame );

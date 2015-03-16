/* 
 * File:   EventImage.h
 * Author: rpb
 *
 * Created on March 14, 2015, 11:57 PM
 */

#ifndef EVENTIMAGE_H
#define	EVENTIMAGE_H

#include <boost/shared_ptr.hpp>

#include <wx/image.h>
#include <wx/event.h>

#include "common.h"

class EventImage: public wxEvent {
public:
  typedef boost::shared_ptr<wxImage> pImage_t;
  EventImage( wxEventType eventType, int winid, pImage_t p, void* user, structTimeSteps );
  virtual ~EventImage( void );
  
  pImage_t GetImage( void ) { return m_pImage; };
  void* GetVoid( void ) { return m_pVoid; };
  structTimeSteps& GetTimeSteps( void ) { return m_ts; };
  
  virtual EventImage* Clone( void ) const { 
    std::cout << "EventImage Cloned" << std::endl;
    return new EventImage( *this ); 
  };

protected:  
private:

  pImage_t m_pImage;
  void* m_pVoid;  
  structTimeSteps m_ts;
};

wxDECLARE_EVENT( EVENT_IMAGE, EventImage );

#endif	/* EVENTIMAGE_H */


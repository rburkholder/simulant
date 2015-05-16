/* 
 * File:   EventImage.h
 * Author: rpb
 *
 * Created on March 14, 2015, 11:57 PM
 */

#ifndef EVENTIMAGE_H
#define	EVENTIMAGE_H

#include <wx/event.h>

#include "RawImage.h"
#include "common.h"

class EventImage: public wxEvent {
public:
  typedef RawImage::pRawImage_t pRawImage_t;
  EventImage( wxEventType eventType, int winid, pRawImage_t p, void* user, structTimeSteps );
  virtual ~EventImage( void );
  
  pRawImage_t GetRawImage( void ) { return m_pRawImage; };
  void* GetUser( void ) { return m_pVoid; };
  structTimeSteps& GetTimeSteps( void ) { return m_ts; };
  
  virtual EventImage* Clone( void ) const { 
    std::cout << "EventImage Cloned" << std::endl;
    return new EventImage( *this ); 
  };

  int64_t nAudioFrame; // current audio frame number
  int64_t ttlAudioFrames;
  int64_t nVideoFrame; // current video frame number, 1 ... n
  int64_t ttlVideoFrames;
  int64_t pts;
  int64_t pkt_pts;  // 1001, 2002, 3003, ... 
  int64_t pkt_dts;
  
protected:  
private:

  pRawImage_t m_pRawImage;
  void* m_pVoid;  
  structTimeSteps m_ts;
  
};

wxDECLARE_EVENT( EVENT_IMAGE, EventImage );

#endif	/* EVENTIMAGE_H */


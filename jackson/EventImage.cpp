/* 
 * File:   EventImage.cpp
 * Author: rpb
 * 
 * Created on March 14, 2015, 11:57 PM
 */

//#include <iostream>

#include "EventImage.h"

EventImage::EventImage( wxEventType eventType, int winid, pRawImage_t p, void* user, structTimeSteps ts ) 
  : wxEvent( winid, eventType ), m_pRawImage( p ), m_pVoid( user ), m_ts( ts )
{
}

EventImage::~EventImage( ) {
}

wxDEFINE_EVENT( EVENT_IMAGE, EventImage );


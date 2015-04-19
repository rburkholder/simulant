/* 
 * File:   EventGenerateFrame.cpp
 * Author: rpb
 * 
 * Created on April 18, 2015, 8:38 PM
 */

#include "EventGenerateFrame.h"

EventGenerateFrame::EventGenerateFrame( wxEventType eventType, int winid ) 
  : wxEvent( winid, eventType )
{
}

EventGenerateFrame::~EventGenerateFrame( ) {
}

wxDEFINE_EVENT( EVENT_GENERATEFRAME, EventGenerateFrame );


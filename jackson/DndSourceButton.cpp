/* 
 * File:   DndSourceButton.cpp
 * Author: rpb
 * 
 * Created on March 1, 2015, 2:53 PM
 */

#include <iostream>

#include <wx/dataobj.h>
#include <wx/dnd.h>
//#include <wx/window.h>

#include "DndSourceButton.h"

class DndDataObjectSource: public wxDataObjectSimple {
public:
  DndDataObjectSource( ) {};
  DndDataObjectSource( const wxString &format ): wxDataObjectSimple( format ) {};
  DndDataObjectSource( wxDataFormatId format=wxDF_INVALID ): wxDataObjectSimple( format ) {};
  
  virtual bool 	GetDataHere (void *buf) const {
    std::cout << "dataobject GetDataHere" << std::endl;
    char* p( (char*)buf );
    p[ 0 ] = 'a';
    p[ 1 ] = 0;
    return true;
  }
  virtual size_t 	GetDataSize () const {
    std::cout << "dataobject GetDataSize" << std::endl;
    return 2;
  }
  virtual bool 	SetData (size_t len, const void *buf) {
    std::cout << "dataobject SetData" << len << std::endl;
    return true;
  }
};

DndSourceButton::DndSourceButton( ): wxStaticText( ), m_bLeftDown( false ) {
  
} 

DndSourceButton::DndSourceButton( 
        wxWindow* parent, 
        wxWindowID id, 
        const wxString& text,
        const wxPoint& pos, 
        const wxSize& size, 
        long style )
: wxStaticText( parent, id, text, pos, size, style ), m_bLeftDown( false )
{
  Init();
}

bool DndSourceButton::Create( 
  wxWindow* parent, 
        wxWindowID id, 
        const wxString& text,
        const wxPoint& pos, 
        const wxSize& size, 
        long style ) {
  
  wxStaticText::Create( parent, id, text, pos, size, style );
  Init();
  return true;
}

DndSourceButton::~DndSourceButton() {
  
}

void DndSourceButton::Init() {
  //_winChart->Bind( wxEVT_MOUSEWHEEL, &AppHedgedBollinger::HandleMouse, this, idChart );
  // m_winChart->Bind( wxEVT_MOTION, &AppHedgedBollinger::HandleMouse, this, idChart );
  Bind( wxEVT_MOTION, &DndSourceButton::HandleMouseMotion, this, this->GetId() );
  Bind( wxEVT_LEFT_DOWN, &DndSourceButton::HandleMouseLeftDown, this, this->GetId() );
  Bind( wxEVT_LEFT_UP, &DndSourceButton::HandleMouseLeftUp, this, this->GetId() );
  // can use event.Dragging())
}

void DndSourceButton::CreateControls() {
  
}

void DndSourceButton::HandleMouseMotion( wxMouseEvent& event ) {
  wxPoint point( event.GetPosition() );
  bool bDragging = event.Dragging();
  if ( !bDragging ) {
    m_bLeftDown = false;  // MouseLeftUp doesn't get called when button released outside of control
  }
  if ( m_bLeftDown && bDragging ) {
    // http://docs.wxwidgets.org/3.0/overview_dnd.html
    std::cout << "dragging" << std::endl;
    //wxTextDataObject my_data("This text will be dragged.");
    DndDataObjectSource my_data("custom");
    wxDropSource dragSource( this );
    dragSource.SetData( my_data );
    wxDragResult result = dragSource.DoDragDrop( wxDrag_CopyOnly );
    switch (result) {
      case wxDragCopy:
        std::cout << "drag copy" << std::endl;
        break;
      case wxDragMove:
        std::cout << "drag move" << std::endl;
        break;
      case wxDragCancel:
        std::cout << "drag cancel" << std::endl;
        break;
      case wxDragError:
        std::cout << "drag error" << std::endl;
        break;
      case wxDragLink:
        std::cout << "drag link" << std::endl;
        break;
      case wxDragNone:
        std::cout << "drag none" << std::endl;
        break;
      default:
        break;
    }

  }
  std::cout << "Mouse Motion" << " " << bDragging << " " << point.x << "," << point.y << std::endl;  
}

void DndSourceButton::HandleMouseLeftDown( wxMouseEvent& event ) {
  m_bLeftDown = true;
  std::cout << "Mouse Left Down" << std::endl;
}

void DndSourceButton::HandleMouseLeftUp( wxMouseEvent& event ) {
  m_bLeftDown = false;
  std::cout << "Mouse Left Up" << std::endl;
}



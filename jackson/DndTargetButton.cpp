/* 
 * File:   DndTargetButton.cpp
 * Author: rpb
 * 
 * Created on March 1, 2015, 5:40 PM
 */

#include "DndTargetButton.h"

DndTargetButton::DndTargetButton( ): wxStaticText( ), 
  m_pDndDropTarget( 0 ), m_pDndDropTextData( 0 ) {
  
} 

DndTargetButton::DndTargetButton( 
        wxWindow* parent, 
        wxWindowID id, 
        const wxString& text,
        const wxPoint& pos, 
        const wxSize& size, 
        long style )
: wxStaticText( parent, id, text, pos, size, style ), 
  m_pDndDropTarget( 0 ), m_pDndDropTextData( 0 )
{
  Init();
}

bool DndTargetButton::Create( 
  wxWindow* parent, 
        wxWindowID id, 
        const wxString& text,
        const wxPoint& pos, 
        const wxSize& size, 
        long style ) {
  
  wxStaticText::Create( parent, id, text, pos, size, style );
  Init();
  return true;  // not sure to return true or false
}

DndTargetButton::~DndTargetButton() {
  
}

void DndTargetButton::Init() {
  m_pDndDropTarget = new DndDropTarget;
  //m_pDndDropTextData = new wxTextDataObject;
  m_pDndDropTextData = new DndDataObjectTarget( "custom" );
  m_pDndDropTarget->SetDataObject( m_pDndDropTextData );
  this->SetDropTarget( m_pDndDropTarget );
}

void DndTargetButton::CreateControls() {
  
}


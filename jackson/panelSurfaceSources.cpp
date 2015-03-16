/////////////////////////////////////////////////////////////////////////////
// Name:        panelsurfacesources.cpp
// Purpose:     
// Author:      contact:  raymond@burkholder.net
// Modified by: 
// Created:     Sat 28 Feb 2015 09:20:20 PM AST
// RCS-ID:      
// Copyright:   (c) 2014 Raymond Burkholder
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "DndSourceButton.h"
#include "DndTargetButton.h"
#include "panelSurfaceSources.h"

IMPLEMENT_DYNAMIC_CLASS( panelSurfaceSources, wxPanel )

panelSurfaceSources::panelSurfaceSources() {
  Init();
}

panelSurfaceSources::panelSurfaceSources( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool panelSurfaceSources::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

panelSurfaceSources::~panelSurfaceSources() {
}

void panelSurfaceSources::Init() {

  m_btnPoly4Side = NULL;
  m_btnDisplay = NULL;

}

void panelSurfaceSources::CreateControls() {    

  panelSurfaceSources* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
  itemPanel1->SetSizer(itemBoxSizer2);

  wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer3, 0, 0, 0);

  m_btnPoly4Side = new wxButton( itemPanel1, ID_BTN_Poly4Side, _("Button Maybe"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(m_btnPoly4Side, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  DndSourceButton* itemButton5 = new DndSourceButton( itemPanel1, ID_BTN_DNDSOURCE, _("4Poly Source"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(itemButton5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  wxButton* itemButton6 = new wxButton( itemPanel1, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(itemButton6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer7, 0, wxGROW|wxALL, 0);

  m_btnDisplay = new wxButton( itemPanel1, ID_BTN_DISPLAY, _("Display"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(m_btnDisplay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  DndTargetButton* itemButton10 = new DndTargetButton( itemPanel1, ID_BTN_DNDTARGET, _("4Poly Target"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(itemButton10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

  wxButton* itemButton9 = new wxButton( itemPanel1, ID_BUTTON3, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(itemButton9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

}

wxBitmap panelSurfaceSources::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon panelSurfaceSources::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

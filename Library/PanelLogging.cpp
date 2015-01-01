/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "PanelLogging.h"

namespace ou { // One Unified

wxDEFINE_EVENT( EVT_ConsoleString, ConsoleStringEvent );

PanelLogging::PanelLogging(void) {
  Init();
}

PanelLogging::PanelLogging( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelLogging::~PanelLogging(void) {
  if ( 0 != m_pOldStreamBuf ) {
    std::cout.rdbuf(m_pOldStreamBuf);
    m_pOldStreamBuf = 0;
  }
}

void PanelLogging::Init() {
  m_txtLogging = 0;
  m_pOldStreamBuf = 0;
}

bool PanelLogging::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelLogging::CreateControls() {    

    PanelLogging* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    //m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(m_txtLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 5);

    m_pOldStreamBuf = std::cout.rdbuf();
    //std::cout.rdbuf(m_txtLogging);
    std::cout.rdbuf( &m_csb );

    Bind( EVT_ConsoleString, &PanelLogging::HandleConsoleLine1, this );
    m_csb.SetOnEmitString( MakeDelegate( this, &PanelLogging::HandleConsoleLine0 ) );
}


wxBitmap PanelLogging::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelLogging::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

void PanelLogging::HandleConsoleLine0( csb_t::Buf* pBuf ) {
  this->QueueEvent( new ConsoleStringEvent( EVT_ConsoleString, pBuf ) );
}

void PanelLogging::HandleConsoleLine1( ConsoleStringEvent& event ) {
  std::string s( event.GetBuf()->buf, event.GetBuf()->n );
  //m_txtLogging->SetValue( s );
  m_txtLogging->AppendText( s );
  m_csb.ReturnBuffer( event.GetBuf() );
}

} // namespace ou

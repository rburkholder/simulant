/*
Copyright (2014) Raymond Burkholder
GPL2 License
Created 2014/12/29
Contact:  raymond@burkholder.net
*/

#include <boost/assign/list_inserter.hpp>

#include "stdafx.h"

#include "PanelLibRawOptions.h"

FrameLibRawOptions::FrameLibRawOptions( void ) {
  Init( );
}

FrameLibRawOptions::FrameLibRawOptions( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init( );
  Create( parent, id, caption, pos, size, style );
}

FrameLibRawOptions::~FrameLibRawOptions( void ) {
}

bool FrameLibRawOptions::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls( );
  if (GetSizer()) {
    GetSizer( )->SetSizeHints( this );
  }
  return true;
}

void FrameLibRawOptions::Init( ) {
  m_rbInterpLinear = NULL;
  m_rbInterpVNG = NULL;
  m_rbInterpPPG = NULL;
  m_rbInterpAHD = NULL;
  m_rbInterpDCB = NULL;
  m_rbInterpModAHD = NULL;
  m_rbInterpAFD = NULL;
  m_rbInterpVCD = NULL;
  m_rbInterpVCDAHD = NULL;
  m_rbInterpLMMSE = NULL;
  m_rbInterpAMaZE = NULL;

  boost::assign::insert( m_mapLUInterpolation )
    (ID_rbInterpLinear, Linear)
    (ID_rbInterpVNG, VNG)
    (ID_rbInterpPPG, PPG)
    (ID_rbInterpAHD, AHD)
    (ID_rbInterpDCB, DCB)
    (ID_rbInterpModAHD, ModAHD)
    (ID_rbInterpAFD, AFD)
    (ID_rbInterpVCD, VCD)
    (ID_rbInterpVCDAHD, VCDAHD)
    (ID_rbInterpLMMSE, LMMSE)
    (ID_rbInterpAMaZE, AMaZE)
    ;
}

void FrameLibRawOptions::CreateControls( void ) {

  FrameLibRawOptions* itemFrame1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer( wxHORIZONTAL );
  itemFrame1->SetSizer( itemBoxSizer2 );

  wxPanel* itemPanel3 = new wxPanel( itemFrame1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
  itemBoxSizer2->Add( itemPanel3, 1, wxGROW | wxALL, 5 );

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer( wxVERTICAL );
  itemPanel3->SetSizer( itemBoxSizer4 );

  wxStaticText* itemStaticText5 = new wxStaticText( itemPanel3, wxID_staticInterpLabel, _( "Demosaic Type" ), wxDefaultPosition, wxDefaultSize, 0 );
  itemStaticText5->SetFont( wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Tahoma" ) ) );
  itemBoxSizer4->Add( itemStaticText5, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5 );

  m_rbInterpLinear = new wxRadioButton( itemPanel3, ID_rbInterpLinear, _( "Linear" ), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  m_rbInterpLinear->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpLinear, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpVNG = new wxRadioButton( itemPanel3, ID_rbInterpVNG, _( "VNG" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpVNG->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpVNG, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpPPG = new wxRadioButton( itemPanel3, ID_rbInterpPPG, _( "PPG" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpPPG->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpPPG, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpAHD = new wxRadioButton( itemPanel3, ID_rbInterpAHD, _( "AHD" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpAHD->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpAHD, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpDCB = new wxRadioButton( itemPanel3, ID_rbInterpDCB, _( "DCB" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpDCB->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpDCB, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpModAHD = new wxRadioButton( itemPanel3, ID_rbInterpModAHD, _( "Modified AHD" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpModAHD->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpModAHD, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpAFD = new wxRadioButton( itemPanel3, ID_rbInterpAFD, _( "AFD" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpAFD->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpAFD, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpVCD = new wxRadioButton( itemPanel3, ID_rbInterpVCD, _( "VCD" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpVCD->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpVCD, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpVCDAHD = new wxRadioButton( itemPanel3, ID_rbInterpVCDAHD, _( "VCD / AHD" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpVCDAHD->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpVCDAHD, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpLMMSE = new wxRadioButton( itemPanel3, ID_rbInterpLMMSE, _( "LMMSE" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpLMMSE->SetValue( false );
  itemBoxSizer4->Add( m_rbInterpLMMSE, 0, wxALIGN_LEFT | wxALL, 5 );

  m_rbInterpAMaZE = new wxRadioButton( itemPanel3, ID_rbInterpAMaZE, _( "AMaZE" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_rbInterpAMaZE->SetValue( true );
  itemBoxSizer4->Add( m_rbInterpAMaZE, 0, wxALIGN_LEFT | wxALL, 5 );


  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpLinear );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpVNG );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpPPG );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpAHD );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpDCB );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpModAHD );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpAFD );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpVCD );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpVCDAHD );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpLMMSE );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameLibRawOptions::OnRbInterpSelected, this, ID_rbInterpAMaZE );

  Bind( wxEVT_CLOSE_WINDOW, &FrameLibRawOptions::OnClose, this );
}

void FrameLibRawOptions::OnRbInterpSelected( wxCommandEvent& event ) {
  int id = event.GetId();
  eInterpolation_t interp = m_mapLUInterpolation[id];
  try {
    if (0 != m_OnSelectInterpolation) m_OnSelectInterpolation( interp );
  }
  catch (...) {

  }
  event.Skip( false );
}

void FrameLibRawOptions::OnClose( wxCloseEvent& event ) {
  event.Skip();
}

wxBitmap FrameLibRawOptions::GetBitmapResource( const wxString& name ) {
  wxUnusedVar( name );
  return wxNullBitmap;
}

wxIcon FrameLibRawOptions::GetIconResource( const wxString& name ) {
  wxUnusedVar( name );
  return wxNullIcon;
}

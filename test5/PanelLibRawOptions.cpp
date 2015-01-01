/*
Copyright (2014) Raymond Burkholder
GPL2 License
Created 2014/12/29
Contact:  raymond@burkholder.net
*/

#include "stdafx.h"

//#include <boost/assign/list_inserter.hpp>

// fix with issue with interference with <limits>
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <wx/valnum.h>
//#include <wx/valgen.h>

#include "PanelLibRawOptions.h"

PanelLibRawOptions::PanelLibRawOptions( void ) {
  Init( );
}

PanelLibRawOptions::PanelLibRawOptions( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init( );
  Create( parent, id, pos, size, style );
}

PanelLibRawOptions::~PanelLibRawOptions( void ) {
}

bool PanelLibRawOptions::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls( );
  if (GetSizer()) {
    GetSizer( )->SetSizeHints( this );
  }
  return true;
}

void PanelLibRawOptions::Init( ) {

  m_rbDemosaicType = NULL;
  m_sliderNoiseThreshold = NULL;
  m_choiceColourSpace = NULL;
  m_txtMedianFilterPasses = NULL;

  wxPanel::InitDialog( );


/*  boost::assign::insert( m_mapLUInterpolation )
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
    ; */
}

void PanelLibRawOptions::CreateControls( void ) {

  PanelLibRawOptions* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer( wxHORIZONTAL );
  itemPanel1->SetSizer( itemBoxSizer2 );

  wxArrayString m_rbDemosaicTypeStrings;
  m_rbDemosaicTypeStrings.Add( _( "Linear" ) );
  m_rbDemosaicTypeStrings.Add( _( "VNG" ) );
  m_rbDemosaicTypeStrings.Add( _( "PPG" ) );
  m_rbDemosaicTypeStrings.Add( _( "AHD" ) );
  m_rbDemosaicTypeStrings.Add( _( "DCB" ) );
  m_rbDemosaicTypeStrings.Add( _( "Mod. AHD" ) );
  m_rbDemosaicTypeStrings.Add( _( "AFD" ) );
  m_rbDemosaicTypeStrings.Add( _( "VCD" ) );
  m_rbDemosaicTypeStrings.Add( _( "VCD/AHD" ) );
  m_rbDemosaicTypeStrings.Add( _( "LMMSE" ) );
  m_rbDemosaicTypeStrings.Add( _( "AMaZE" ) );
  m_rbDemosaicType = new wxRadioBox( itemPanel1, ID_rbDemosaicType, _( "Demosaic Type" ), wxDefaultPosition, wxDefaultSize, m_rbDemosaicTypeStrings, 1, wxRA_SPECIFY_COLS );
  m_rbDemosaicType->SetSelection( 0 );
  itemBoxSizer2->Add( m_rbDemosaicType, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 );

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer( wxVERTICAL );
  itemBoxSizer2->Add( itemBoxSizer4, 0, wxALIGN_TOP | wxALL, 0 );

  wxStaticBox* itemStaticBoxSizer5Static = new wxStaticBox( itemPanel1, wxID_ANY, _( "Noise Threshold" ) );
  wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer( itemStaticBoxSizer5Static, wxVERTICAL );
  itemBoxSizer4->Add( itemStaticBoxSizer5, 0, wxALIGN_LEFT | wxALL, 2 );

  m_sliderNoiseThreshold = new wxSlider( itemStaticBoxSizer5->GetStaticBox( ), ID_SliderNoiseThreshold, 0, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS );
  itemStaticBoxSizer5->Add( m_sliderNoiseThreshold, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );

  wxStaticBox* itemStaticBoxSizer7Static = new wxStaticBox( itemPanel1, wxID_ANY, _( "Colour Space" ) );
  wxStaticBoxSizer* itemStaticBoxSizer7 = new wxStaticBoxSizer( itemStaticBoxSizer7Static, wxVERTICAL );
  itemBoxSizer4->Add( itemStaticBoxSizer7, 0, wxALIGN_LEFT | wxALL, 0 );

  wxArrayString m_choiceColourSpaceStrings;
  m_choiceColourSpaceStrings.Add( _( "Raw Colour" ) );
  m_choiceColourSpaceStrings.Add( _( "sRGB D65" ) );
  m_choiceColourSpaceStrings.Add( _( "Adobe RGB (1998) D65" ) );
  m_choiceColourSpaceStrings.Add( _( "Wide Gamut RGB D65" ) );
  m_choiceColourSpaceStrings.Add( _( "Kodak ProPhoto RGB D65" ) );
  m_choiceColourSpaceStrings.Add( _( "XYZ" ) );
  m_choiceColourSpace = new wxChoice( itemStaticBoxSizer7->GetStaticBox( ), ID_choiceColourSpace, wxDefaultPosition, wxDefaultSize, m_choiceColourSpaceStrings, 0 );
  m_choiceColourSpace->SetStringSelection( _( "sRGB D65" ) );
  itemStaticBoxSizer7->Add( m_choiceColourSpace, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );

  m_boxMedianFilterPasses = new wxStaticBox( itemPanel1, wxID_ANY, _( "Median Filter Passes" ) );
  m_sizerMedianFilterPasses = new wxStaticBoxSizer( m_boxMedianFilterPasses, wxVERTICAL );
  itemBoxSizer4->Add( m_sizerMedianFilterPasses, 0, wxALIGN_LEFT | wxALL, 5 );

  m_txtMedianFilterPasses = new wxTextCtrl( m_sizerMedianFilterPasses->GetStaticBox( ), ID_txtMedianFilterPasses, _( "0" ), wxDefaultPosition, wxDefaultSize, 0 );
  m_txtMedianFilterPasses->SetMaxLength( 1 );
  m_sizerMedianFilterPasses->Add( m_txtMedianFilterPasses, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );

  m_btnProcess = new wxButton( itemPanel1, ID_btnProcess, _( "Process" ), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer4->Add( m_btnProcess, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10 );


  // validators
  m_txtMedianFilterPasses->SetValidator( wxIntegerValidator<unsigned int>( &m_options.nMedianFilterPasses ) );
  //m_txtMedianFilterPasses->TransferDataToWindow();
  //m_sliderNoiseThreshold->SetValidator( wxIntegerValidator<unsigned int>( &m_options.intNoiseThreshold ) );
  //unsigned int n = std::numeric_limits<unsigned int>::min( );


  // events
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelLibRawOptions::OnBtnProcessClick, this );
  Bind( wxEVT_CLOSE_WINDOW, &PanelLibRawOptions::OnClose, this );
}

void PanelLibRawOptions::OnBtnProcessClick( wxCommandEvent& event ) {
  bool bValidate( true );

  bValidate &= m_sizerMedianFilterPasses->GetStaticBox( )->Validate( );
  //bValidate &= m_sliderNoiseThreshold->Validate();
  if (bValidate){
  //if (wxPanel::Validate() ) {
    //wxPanel::TransferDataFromWindow();
    m_options.eInterpolation = static_cast<eInterpolation_t>(m_rbDemosaicType->GetSelection( ));
    m_options.eColourSpace = static_cast<eColourSpace_t>(m_choiceColourSpace->GetSelection());
    // http://docs.wxwidgets.org/trunk/classwx_slider.html
    m_options.intNoiseThreshold = m_sliderNoiseThreshold->GetValue();
    assert( true == m_sizerMedianFilterPasses->GetStaticBox( )->TransferDataFromWindow( ) );
    //assert( true==m_sliderNoiseThreshold->TransferDataFromWindow() );
    if (0 != m_OnOption) m_OnOption( m_options );
  }
  event.Skip( );
}

void PanelLibRawOptions::OnClose( wxCloseEvent& event ) {
  event.Skip();
}

wxBitmap PanelLibRawOptions::GetBitmapResource( const wxString& name ) {
  wxUnusedVar( name );
  return wxNullBitmap;
}

wxIcon PanelLibRawOptions::GetIconResource( const wxString& name ) {
  wxUnusedVar( name );
  return wxNullIcon;
}

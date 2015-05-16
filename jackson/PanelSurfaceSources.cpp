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

#include <wx/splitter.h>

#include "DndSourceButton.h"
#include "DndTargetButton.h"
#include "PanelSurfaceSources.h"

IMPLEMENT_DYNAMIC_CLASS( PanelSurfaceSources, wxPanel )

PanelSurfaceSources::PanelSurfaceSources() {
  Init();
}

PanelSurfaceSources::PanelSurfaceSources( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool PanelSurfaceSources::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
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

PanelSurfaceSources::~PanelSurfaceSources() {
}

void PanelSurfaceSources::Init() {

  m_btnPoly4Side = NULL;
  m_btnDisplay = NULL;
  m_btnEditMode = NULL;
  m_btnUndo = NULL;

  m_bInEditMode = false;
  m_sliderHorizontal = 0;
}

void PanelSurfaceSources::Append( pPhysicalDisplay_t pPhysicalDisplay ) {
  m_treeDisplays->Append( pPhysicalDisplay );
}

void PanelSurfaceSources::CreateControls() {    
  
  // notice that the relative scale factor is important:  0 no scale, 1 scales with window size change

  PanelSurfaceSources* itemPanel1 = this;
  
  wxBoxSizer* itemBoxSizerForPanel = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer(itemBoxSizerForPanel);
  
  m_sliderHorizontal = new wxSlider( itemPanel1, ID_SLIDER_HORIZONTAL, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
  m_sliderHorizontal->Enable( false );
  itemBoxSizerForPanel->Add( m_sliderHorizontal, 0, wxALIGN_TOP|wxGROW|wxALL, 1 );
  
  wxBoxSizer* itemBoxSizerHorizontal = new wxBoxSizer( wxHORIZONTAL );
  itemBoxSizerForPanel->Add( itemBoxSizerHorizontal, 1, wxALIGN_LEFT|wxGROW|wxALL, 0 );
  
  wxSplitterWindow* itemSplitterWindow = new wxSplitterWindow( itemPanel1, ID_SPLITTER_HORIZONTAL, wxDefaultPosition, wxSize(100, 100), wxNO_BORDER|wxSP_LIVE_UPDATE ); // wxSP_3DSASH|wxNO_BORDER|wxSP_BORDER
  itemSplitterWindow->SetMinimumPaneSize(10);
  itemSplitterWindow->SetSashGravity(1.0);
  
  m_treeDisplays = new TreeDisplayManager( itemSplitterWindow, ID_TREE_DISPLAYS, wxDefaultPosition, wxSize( 150, 10 ), wxTR_HAS_BUTTONS | wxTR_SINGLE );
  
  wxPanel* m_panelRight = new wxPanel( itemSplitterWindow, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_panelRight->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  
  wxBoxSizer* itemBoxSizerPanelVertical = new wxBoxSizer( wxVERTICAL );
  m_panelRight->SetSizer( itemBoxSizerPanelVertical );
  
  wxBoxSizer* itemBoxSizerPanelStatus = new wxBoxSizer( wxHORIZONTAL );
  itemBoxSizerPanelVertical->Add( itemBoxSizerPanelStatus, 0, wxGROW|wxALL, 0 );
  
  m_stInfo = new wxStaticText( m_panelRight, ID_ST_FRAMECOUNTER, _("_"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  itemBoxSizerPanelStatus->Add( m_stInfo, 0, wxGROW|wxALL, 2 );

  wxBoxSizer* itemBoxSizerPanelButtonColumns = new wxBoxSizer( wxHORIZONTAL );
  itemBoxSizerPanelVertical->Add( itemBoxSizerPanelButtonColumns, 1, wxGROW|wxALL, 0 );
  
  itemSplitterWindow->SplitVertically(m_treeDisplays, m_panelRight, 40);
  itemBoxSizerHorizontal->Add(itemSplitterWindow, 1, wxGROW|wxALL, 0);

  wxBoxSizer* itemBoxSizerVertColumn1 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn1, 0, wxALIGN_TOP|wxALL, 0);

  m_btnPoly4Side = new wxButton( m_panelRight, ID_BTN_Poly4Side, _("Button Maybe"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(m_btnPoly4Side, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  DndSourceButton* itemButton5 = new DndSourceButton( m_panelRight, ID_BTN_DNDSOURCE, _("4Poly Source"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(itemButton5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxButton* itemButton6 = new wxButton( m_panelRight, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(itemButton6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxBoxSizer* itemBoxSizerVertColumn2 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn2, 0, wxALIGN_TOP|wxALL, 0);

  m_btnDisplay = new wxButton( m_panelRight, ID_BTN_DISPLAY, _("Display"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(m_btnDisplay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  DndTargetButton* itemButton10 = new DndTargetButton( m_panelRight, ID_BTN_DNDTARGET, _("4Poly Target"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(itemButton10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxButton* itemButton9 = new wxButton( m_panelRight, ID_BUTTON3, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(itemButton9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxBoxSizer* itemBoxSizerVertColumn3 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn3, 0, wxALIGN_TOP|wxALL, 0);
  
  m_btnEditMode = new wxToggleButton( m_panelRight, ID_BTN_EDITMODE, _("Editing Off"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn3->Add(m_btnEditMode, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  m_btnUndo = new wxButton( m_panelRight, ID_BTN_UNDO, _("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn3->Add(m_btnUndo, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);
  
  m_treeDisplays->SetStaticTextInfo( m_stInfo );
  m_treeDisplays->SetSlider( m_sliderHorizontal );

  Bind( wxEVT_TOGGLEBUTTON, &PanelSurfaceSources::HandleToggleEditMode, this, ID_BTN_EDITMODE );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandleUndo, this, ID_BTN_UNDO );
  //Bind( wxEVT_SCROLL_THUMBTRACK, &PanelSurfaceSources::HandleScrollThumbTrack, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_LINEUP, &PanelSurfaceSources::HandleScrollLineChange, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_LINEDOWN, &PanelSurfaceSources::HandleScrollLineChange, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_THUMBRELEASE, &PanelSurfaceSources::HandleScrollThumbRelease, this, ID_SLIDER_HORIZONTAL );
}

void PanelSurfaceSources::HandleUndo( wxCommandEvent& event ) {
  event.Skip();
}

void PanelSurfaceSources::HandleScrollThumbTrack( wxScrollEvent& event ) { // could pause playing while seeking, then on release resume playing
  std::cout << "ThumbTrack " << event.GetPosition() << std::endl;
}

void PanelSurfaceSources::HandleScrollLineChange( wxScrollEvent& event ) {  // cursor key related changes
  std::cout << "LineChange " << event.GetPosition() << std::endl;
}

void PanelSurfaceSources::HandleScrollThumbRelease( wxScrollEvent& event ) { // on mouse release
  std::cout << "ThumbRelease " << event.GetPosition() << std::endl;
}

void PanelSurfaceSources::HandleToggleEditMode( wxCommandEvent& event ) {
  if ( m_bInEditMode ) {
    m_bInEditMode = false;
    m_btnEditMode->SetLabel( "Editing Off" );
  }
  else {
    m_bInEditMode = true;
    m_btnEditMode->SetLabel( "Editing On" );
  }
}

wxBitmap PanelSurfaceSources::GetBitmapResource( const wxString& name ) {
  // Bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelSurfaceSources::GetIconResource( const wxString& name ) {
  // Icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
}

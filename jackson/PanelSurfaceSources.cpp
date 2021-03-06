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

//#include <boost/phoenix/bind/bind_member_function.hpp>
//#include <boost/phoenix/core/argument.hpp>

#include <wx/splitter.h>

#include "DndSourceButton.h"
#include "DndTargetButton.h"

#include "SceneViewCommon.h"

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
  if (GetSizer()) {
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
  
  m_sliderSeek = 0;
  m_sliderFader = NULL;
  m_sliderZ = NULL;
  
  m_panelScene = 0;
  
  //m_pWaveFormFrontLeft = 0;
  //m_pWaveFormFrontRight = 0;
  //m_pWaveFormBackLeft = 0;
  //m_pWaveFormBackRight = 0;
  
  m_BtnEvent = BtnEvent::BtnPlay;
}

void PanelSurfaceSources::HandlePlay( wxCommandEvent& event ) {
  m_btnPlay->Enable( false );
  m_btnPause->Enable( false );  // no ability to pause yet
  m_btnStop->Enable( true );
  m_btnAbort->Enable( true );
  m_signalBtnEvent( BtnEvent::BtnPlay );
}

void PanelSurfaceSources::HandlePause( wxCommandEvent& event ) {
  m_btnPlay->Enable( true );
  m_btnPause->Enable( false );
  m_btnStop->Enable( true );
  m_btnAbort->Enable( true );
  m_signalBtnEvent( BtnEvent::BtnPause );
}

void PanelSurfaceSources::HandleStop( wxCommandEvent& event ) {
  m_btnPlay->Enable( true );
  m_btnPause->Enable( false );
  m_btnStop->Enable( false );
  m_btnAbort->Enable( false );
  m_signalBtnEvent( BtnEvent::BtnStop );
}

void PanelSurfaceSources::HandleAbort( wxCommandEvent& event ) {
  m_btnPlay->Enable( true );
  m_btnPause->Enable( false );
  m_btnStop->Enable( false );
  m_btnAbort->Enable( false );
  m_signalBtnEvent( BtnEvent::BtnAbort );
}

void PanelSurfaceSources::ResetButtons( void ) {
  m_btnPlay->Enable( true );
  m_btnPause->Enable( false );
  m_btnStop->Enable( false );
  m_btnAbort->Enable( false );
}

void PanelSurfaceSources::Append( pPhysicalDisplay_t pPhysicalDisplay ) {
  m_treeDisplays->Append( pPhysicalDisplay );
}

void PanelSurfaceSources::Save( boost::archive::text_oarchive& oa) {
  m_treeDisplays->Save( oa );
}

void PanelSurfaceSources::Load( boost::archive::text_iarchive& ia) {
  m_treeDisplays->Load( ia );
}

void PanelSurfaceSources::CreateControls() {    
  
  // notice that the relative scale factor is important:  0 no scale, 1 scales with window size change

  PanelSurfaceSources* itemPanel1 = this;
  
  wxBoxSizer* itemBoxSizerForPanel = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer(itemBoxSizerForPanel);
  
  m_sliderSeek = new wxSlider( itemPanel1, ID_SLIDER_SEEK, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
  m_sliderSeek->Enable( false );
  itemBoxSizerForPanel->Add( m_sliderSeek, 0, wxALIGN_TOP|wxGROW|wxALL, 1 );
  
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

  // Static Status line
  wxBoxSizer* itemBoxSizerPanelStatus = new wxBoxSizer( wxHORIZONTAL );
  itemBoxSizerPanelVertical->Add( itemBoxSizerPanelStatus, 0, wxGROW|wxALL, 0 );
  
  m_stInfo = new wxStaticText( m_panelRight, ID_ST_FRAMECOUNTER, _("_"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  itemBoxSizerPanelStatus->Add( m_stInfo, 0, wxGROW|wxALL, 2 );

  // Media Buttons
  wxBoxSizer* itemBoxSizerMediaButtons = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizerPanelVertical->Add(itemBoxSizerMediaButtons, 0, wxALIGN_LEFT|wxALL, 3);
  m_btnPlay = new wxButton( m_panelRight, ID_BTN_PLAY, _("Play"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  itemBoxSizerMediaButtons->Add(m_btnPlay, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_btnPause = new wxButton( m_panelRight, ID_BTN_PAUSE, _("Pause"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_btnPause->Enable(false);
  itemBoxSizerMediaButtons->Add(m_btnPause, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_btnStop = new wxButton( m_panelRight, ID_BTN_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_btnStop->Enable(false);
  itemBoxSizerMediaButtons->Add(m_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_btnAbort = new wxButton( m_panelRight, ID_BTN_ABORT, _("Abort"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_btnAbort->Enable(false);
  itemBoxSizerMediaButtons->Add(m_btnAbort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  // Button Columns, are any of the buttons useful?
  wxBoxSizer* itemBoxSizerPanelButtonColumns = new wxBoxSizer( wxHORIZONTAL );
  itemBoxSizerPanelVertical->Add( itemBoxSizerPanelButtonColumns, 0, wxGROW|wxALL, 0 );
  
  itemSplitterWindow->SplitVertically(m_treeDisplays, m_panelRight, 40);
  itemBoxSizerHorizontal->Add(itemSplitterWindow, 1, wxGROW|wxALL, 0);

  // Button Column 1
  wxBoxSizer* itemBoxSizerVertColumn1 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn1, 0, wxALIGN_TOP|wxALL, 0);

  m_btnPoly4Side = new wxButton( m_panelRight, ID_BTN_Poly4Side, _("Button Maybe"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(m_btnPoly4Side, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  DndSourceButton* itemButton5 = new DndSourceButton( m_panelRight, ID_BTN_DNDSOURCE, _("4Poly Source"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(itemButton5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxButton* itemButton6 = new wxButton( m_panelRight, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn1->Add(itemButton6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  // Button Column 2
  wxBoxSizer* itemBoxSizerVertColumn2 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn2, 0, wxALIGN_TOP|wxALL, 0);

  m_btnDisplay = new wxButton( m_panelRight, ID_BTN_DISPLAY, _("Display"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(m_btnDisplay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  DndTargetButton* itemButton10 = new DndTargetButton( m_panelRight, ID_BTN_DNDTARGET, _("4Poly Target"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(itemButton10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  wxButton* itemButton9 = new wxButton( m_panelRight, ID_BUTTON3, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn2->Add(itemButton9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  // Button Column 3
  wxBoxSizer* itemBoxSizerVertColumn3 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizerPanelButtonColumns->Add(itemBoxSizerVertColumn3, 0, wxALIGN_TOP|wxALL, 0);
  
  m_btnEditMode = new wxToggleButton( m_panelRight, ID_BTN_EDITMODE, _("Editing Off"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn3->Add(m_btnEditMode, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  m_btnUndo = new wxButton( m_panelRight, ID_BTN_UNDO, _("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizerVertColumn3->Add(m_btnUndo, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

  // Waveform Panels
  wxBoxSizer* itemBoxSizerScenePanelH = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizerPanelVertical->Add(itemBoxSizerScenePanelH, 1, wxGROW|wxALL, 2);
  m_panelScene = new wxPanel( m_panelRight, ID_PANEL_SCENE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_panelScene->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  itemBoxSizerScenePanelH->Add(m_panelScene, 1, wxGROW|wxALL, 1);
  m_itemBoxSizerSceneElements = new wxBoxSizer(wxVERTICAL);
  m_panelScene->SetSizer(m_itemBoxSizerSceneElements);

  m_sliderZ = new wxSlider( itemPanel1, ID_SLIDER_Z, 0, -100, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE|wxNO_BORDER );
  if (PanelSurfaceSources::ShowToolTips())
    m_sliderZ->SetToolTip(_("z"));
  m_sliderZ->SetForegroundColour(wxColour(165, 42, 42));
  m_sliderZ->SetBackgroundColour(wxColour(189, 146, 53));
  m_sliderZ->Enable(false);
  itemBoxSizerHorizontal->Add(m_sliderZ, 0, wxGROW|wxALL, 1);

  m_sliderVolume = new wxSlider( itemPanel1, ID_SLIDER_VOLUME, 0, -18, 4, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE|wxNO_BORDER );
  if (PanelSurfaceSources::ShowToolTips())
    m_sliderVolume->SetToolTip(_("volume"));
  m_sliderVolume->SetBackgroundColour(wxColour(203, 86, 57));
  m_sliderVolume->Enable(false);
  itemBoxSizerHorizontal->Add(m_sliderVolume, 0, wxGROW|wxALL, 1);

  m_sliderFader = new wxSlider( itemPanel1, ID_SLIDER_FADER, 100, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE|wxNO_BORDER );
  if (PanelSurfaceSources::ShowToolTips())
    m_sliderFader->SetToolTip(_("fader"));
  m_sliderFader->SetBackgroundColour(wxColour(218, 58, 37));
  m_sliderFader->Enable(false);
  itemBoxSizerHorizontal->Add(m_sliderFader, 0, wxGROW|wxALL, 1);

  m_sliderMaster = new wxSlider( itemPanel1, ID_SLIDER_MASTER, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE|wxNO_BORDER );
  if (PanelSurfaceSources::ShowToolTips())
    m_sliderMaster->SetToolTip(_("master"));
  m_sliderMaster->SetBackgroundColour(wxColour(237, 22, 22));
  itemBoxSizerHorizontal->Add(m_sliderMaster, 0, wxGROW|wxALL, 1);

  m_treeDisplays->SetStaticTextInfo( m_stInfo );
  m_treeDisplays->SetSliders( m_sliderSeek, m_sliderZ, m_sliderVolume, m_sliderFader, m_sliderMaster );
  
  //m_connectionClearScenePanel
  //  = m_treeDisplays->m_signalClearScenePanel.connect( boost::phoenix::bind( &PanelSurfaceSources::ClearScenePanel, this ) );
  //m_connectionAppendKeyFrameView 
  //  = m_treeDisplays->m_signalAppendKeyframeView.connect( boost::phoenix::bind( &PanelSurfaceSources::AppendKeyFrameView, this ) );
  namespace args = boost::phoenix::arg_names;
  m_connectionAppendView
    = m_treeDisplays->m_signalAppendView.connect( boost::phoenix::bind( &PanelSurfaceSources::AppendView, this, args::arg1, args::arg2 ) );
  m_treeDisplays->SetScenePanel( m_panelScene );
  
  m_treeDisplays->SetButtonEvent( &m_signalBtnEvent );
  
  Bind( wxEVT_TOGGLEBUTTON, &PanelSurfaceSources::HandleToggleEditMode, this, ID_BTN_EDITMODE );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandleUndo, this, ID_BTN_UNDO );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandlePlay, this, ID_BTN_PLAY );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandlePause, this, ID_BTN_PAUSE );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandleStop, this, ID_BTN_STOP );
  Bind( wxEVT_BUTTON, &PanelSurfaceSources::HandleAbort, this, ID_BTN_ABORT );

  //m_panelScene->Bind( wxEVT_MOUSEWHEEL, &PanelSurfaceSources::HandleMouseWheel1, this );
  //m_panelScene->Bind( wxEVT_MOTION, &PanelSurfaceSources::HandleMouseMotionInWaveformsPanel, this );
  //m_panelScene->Bind( wxEVT_ENTER_WINDOW, &PanelSurfaceSources::HandleEnterWindow, this );
  //m_panelScene->Bind( wxEVT_LEAVE_WINDOW, &PanelSurfaceSources::HandleLeaveWindow, this );

  //Bind( wxEVT_CHAR, &PanelSurfaceSources::HandleKey, this );
  
  Bind( wxEVT_CLOSE_WINDOW, &PanelSurfaceSources::HandleClose, this );
  
  //Bind( wxEVT_SCROLL_THUMBTRACK, &PanelSurfaceSources::HandleScrollThumbTrack, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_LINEUP, &PanelSurfaceSources::HandleScrollLineChange, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_LINEDOWN, &PanelSurfaceSources::HandleScrollLineChange, this, ID_SLIDER_HORIZONTAL );
  //Bind( wxEVT_SCROLL_THUMBRELEASE, &PanelSurfaceSources::HandleScrollThumbRelease, this, ID_SLIDER_HORIZONTAL );
}

void PanelSurfaceSources::HandleClose( wxCloseEvent& event ) {
  m_connectionAppendView.disconnect();
  //m_connectionAppendWaveformView.disconnect();
  //m_connectionClearScenePanel.disconnect();
  //Unbind( wxEVT_CHAR, &PanelSurfaceSources::HandleKey, this );
}

void PanelSurfaceSources::AppendView( SceneViewCommon* p, int xRelativeSize ) {
  //View* p = new View( m_panelScene, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  //p->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  //p->SetSize( wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, 20, wxSIZE_AUTO_WIDTH );
  m_itemBoxSizerSceneElements->Add( p, xRelativeSize, wxEXPAND|wxALL, 1 );
  //m_itemBoxSizerSceneElements->Fit(p);
  //p->Refresh();
  m_panelScene->Layout();
  //return p;
}


//void PanelSurfaceSources::ClearScenePanel( void ) {
//  m_panelScene->DestroyChildren();
//}

void PanelSurfaceSources::HandleKey( wxKeyEvent& event ) {
  //std::cout << "key='" << event.GetKeyCode() << "'" << std::endl;
  event.Skip();
}

void PanelSurfaceSources::HandleMouseWheel1(wxMouseEvent& event) {
  std::cout << "basic panel wheel" << std::endl;
  event.Skip();
}

void PanelSurfaceSources::HandleEnterWindow( wxMouseEvent& event ) {
  std::cout << "mouse enter: " << wxID_HIGHEST << "," << event.GetId() << std::endl; 
  event.Skip();
}

void PanelSurfaceSources::HandleLeaveWindow( wxMouseEvent& event ) {
  std::cout << "mouse leave: " << wxID_HIGHEST << "," << event.GetId() << std::endl;
  event.Skip();
}

void PanelSurfaceSources::HandleUndo( wxCommandEvent& event ) {
  event.Skip();
}

void PanelSurfaceSources::HandleScrollThumbTrack( wxScrollEvent& event ) { // could pause playing while seeking, then on release resume playing
  std::cout << "ThumbTrack " << event.GetPosition() << std::endl;
  event.Skip();
}

void PanelSurfaceSources::HandleScrollLineChange( wxScrollEvent& event ) {  // cursor key related changes
  std::cout << "LineChange " << event.GetPosition() << std::endl;
  event.Skip();
}

void PanelSurfaceSources::HandleScrollThumbRelease( wxScrollEvent& event ) { // on mouse release
  std::cout << "ThumbRelease " << event.GetPosition() << std::endl;
  event.Skip();
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

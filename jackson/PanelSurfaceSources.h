/* 
 * File:   panelSurfaceSources.h
 * Author: rpb
 *
 * Created on February 28, 2015, 9:55 PM
 */

#pragma once

#include <boost/signals2.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/slider.h>

#include "WaveformView.h"
#include "KeyFrameView.h"

#include "TreeDisplayManager.h"

#define SYMBOL_PANELSURFACESOURCES_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELSURFACESOURCES_TITLE _("sources")
#define SYMBOL_PANELSURFACESOURCES_IDNAME ID_PANELSURFACESOURCES
#define SYMBOL_PANELSURFACESOURCES_SIZE wxSize(400, 300)
#define SYMBOL_PANELSURFACESOURCES_POSITION wxDefaultPosition

class PanelSurfaceSources: public wxPanel {    
  DECLARE_DYNAMIC_CLASS( PanelSurfaceSources )
public:
  
  typedef TreeDisplayManager::BtnEvent BtnEvent;
  
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;
  
  typedef boost::signals2::signal<void ()> signalSlider_t;
  typedef signalSlider_t::slot_type slotSlider_t;  
  
  typedef TreeDisplayManager::signalBtnEvent_t signalBtnEvent_t;
  
  PanelSurfaceSources();
  PanelSurfaceSources( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_PANELSURFACESOURCES_IDNAME, 
          const wxPoint& pos = SYMBOL_PANELSURFACESOURCES_POSITION, 
          const wxSize& size = SYMBOL_PANELSURFACESOURCES_SIZE, 
          long style = SYMBOL_PANELSURFACESOURCES_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_PANELSURFACESOURCES_IDNAME, 
          const wxPoint& pos = SYMBOL_PANELSURFACESOURCES_POSITION, 
          const wxSize& size = SYMBOL_PANELSURFACESOURCES_SIZE, 
          long style = SYMBOL_PANELSURFACESOURCES_STYLE );

  virtual ~PanelSurfaceSources();
  
  void Append( pPhysicalDisplay_t pPhysicalDisplay );
  
  void Add( TreeDisplayManager::pAudio_t pAudio ) { m_treeDisplays->Add( pAudio ); }
  
  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);
  
  void ResetButtons( void );
  
  void ClearScenePanel( void );
  WaveformView* AppendWaveformView( void );
  KeyFrameView* AppendKeyFrameView( void );
  
  // when range is 0, should disable the control
  //void SetScrollMin( int min ) { assert( 0 != m_sliderHorizontal ); m_sliderHorizontal->SetMin( min ); }  // defaults to 0..100
  //void SetScrollMax( int max ) { assert( 0 != m_sliderHorizontal ); m_sliderHorizontal->SetMax( max ); }  // defaults to 0..100
  //void SetScrollRange( int min, int max) { assert( 0 != m_sliderHorizontal ); m_sliderHorizontal->SetRange( min, max ); }

protected:  
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_PANELSURFACESOURCES,
    ID_BTN_Poly4Side,
    ID_BUTTON1,
    ID_BUTTON2,
    ID_BTN_DISPLAY,
    ID_BUTTON3,
    ID_BTN_DNDTARGET,
    ID_BTN_DNDSOURCE, 
    ID_BTN_EDITMODE, 
    ID_BTN_UNDO, 
    ID_TREE_DISPLAYS,
    ID_SLIDER_SEEK,
    ID_SLIDER_Z, ID_SLIDER_VOLUME, ID_SLIDER_FADER, ID_SLIDER_MASTER,
    ID_SPLITTER_HORIZONTAL,
    ID_PANEL, 
    ID_ST_FRAMECOUNTER, 
    ID_PANEL_SCENE,
    ID_WFV_FRONTLEFT,
    ID_WFV_FRONTRIGHT,
    ID_WFV_BACKLEFT, 
    ID_WFV_BACKRIGHT,
    ID_BTN_PLAY, ID_BTN_PAUSE, ID_BTN_STOP, ID_BTN_ABORT
  };
  
  BtnEvent m_BtnEvent;
  signalBtnEvent_t m_signalBtnEvent;
  
  boost::signals2::connection m_connectionClearScenePanel;
  boost::signals2::connection m_connectionAppendKeyFrameView;
  boost::signals2::connection m_connectionAppendWaveformView;
  
  bool m_bInEditMode;
  
  wxButton* m_btnPoly4Side;
  wxButton* m_btnDisplay;
  wxToggleButton* m_btnEditMode;
  wxButton* m_btnUndo;
  wxStaticText* m_stInfo;
  
  wxSlider* m_sliderSeek;
  wxSlider* m_sliderZ;
  wxSlider* m_sliderVolume;
  wxSlider* m_sliderFader;
  wxSlider* m_sliderMaster;
  
  wxButton* m_btnPlay;
  wxButton* m_btnPause;
  wxButton* m_btnStop;
  wxButton* m_btnAbort;  
  
  wxPanel* m_panelScene; // handles mouse events
  wxBoxSizer* m_itemBoxSizerSceneElements;
  
  TreeDisplayManager* m_treeDisplays;
  
  void HandleToggleEditMode( wxCommandEvent& event );
  void HandleUndo( wxCommandEvent& event );
  void HandleScrollThumbTrack( wxScrollEvent& event );
  void HandleScrollLineChange( wxScrollEvent& event );
  void HandleScrollThumbRelease( wxScrollEvent& event );
  
  void HandlePlay( wxCommandEvent& event );
  void HandlePause( wxCommandEvent& event );
  void HandleStop( wxCommandEvent& event );
  void HandleAbort( wxCommandEvent& event );
  
  void HandleKey( wxKeyEvent& event );

  void HandleMouseWheel1( wxMouseEvent& );

  void HandleEnterWindow( wxMouseEvent& );
  void HandleLeaveWindow( wxMouseEvent& );
  
  void HandleClose( wxCloseEvent& event );
  
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };

};


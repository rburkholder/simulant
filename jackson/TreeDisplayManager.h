/* 
 * File:   TreeDisplayManager.h
 * Author: rpb
 *
 * Created on April 5, 2015, 10:11 PM
 */

#ifndef TREEDISPLAYMANAGER_H
#define	TREEDISPLAYMANAGER_H

#include <map>
#include <array>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "PhysicalDisplay.h"
#include "SceneManager.h"

#include "Audio.h"
#include "WaveformView.h"
#include "KeyFrameView.h"

#define SYMBOL_TREEDISPLAYMANAGER_STYLE wxTR_HAS_BUTTONS | wxTR_SINGLE
#define SYMBOL_TREEDISPLAYMANAGER_TITLE _("Displays")
#define SYMBOL_TREEDISPLAYMANAGER_IDNAME ID_TREEDISPLAYMANAGER
#define SYMBOL_TREEDISPLAYMANAGER_SIZE wxSize(100, 200)
#define SYMBOL_TREEDISPLAYMANAGER_POSITION wxDefaultPosition

class TreeItemBase;

struct CommonGuiElements {

  wxStaticText* pstInfo;

  wxSlider* pSliderSeek;
  wxSlider* pSliderZ;
  wxSlider* pSliderVolume;
  wxSlider* pSliderFader;
  wxSlider* pSliderMaster;

  // may deprecate this list in favor of a SceneManager list
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef std::vector<pPhysicalDisplay_t> vpPhysicalDisplay_t;
  vpPhysicalDisplay_t vpPhysicalDisplay;

  typedef SceneManager::pSceneManager_t pSceneManager_t;
  typedef std::vector<pSceneManager_t> vpSceneManager_t;
  vpSceneManager_t vpSceneManager;

  CommonGuiElements( void ): 
  pstInfo( 0 ), pSliderSeek( 0 ), pSliderFader( 0 ), pSliderZ( 0 )
  {}
};

class TreeDisplayManager: public wxTreeCtrl {
  DECLARE_DYNAMIC_CLASS( TreeDisplayManager )
  friend class boost::serialization::access;
public:
  
  enum BtnEvent{ BtnPlay, BtnPause, BtnStop, BtnAbort };
  
  typedef boost::signals2::signal<void (BtnEvent)> signalBtnEvent_t;
  typedef signalBtnEvent_t::slot_type slotBtnEvent_t;
  
  typedef boost::signals2::signal<WaveformView* (void)> signalAppendWaveformView_t;
  typedef signalAppendWaveformView_t::slot_type slotAppendWaveformView_t;
  
  typedef boost::signals2::signal<KeyFrameView* (void)> signalAppendKeyFrameView_t;
  typedef signalAppendKeyFrameView_t::slot_type slotAppendKeyFrameView_t;
  
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;

  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;

  typedef boost::shared_ptr<Audio> pAudio_t;
  
  struct TreeItemResources {  // used by inheritors of TreeItemBase
    TreeDisplayManager&  tree;  // used for assigning the popup, plus other base class functions, eg for Bind, etc
    std::string sCurrentPath;
    TreeDisplayManager::pAudio_t pAudio;  // kept in AppProjection
    TreeItemResources( TreeDisplayManager& tree ): tree( tree ) {}
  };

  TreeDisplayManager();
  TreeDisplayManager( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEDISPLAYMANAGER_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEDISPLAYMANAGER_POSITION, 
          const wxSize& size = SYMBOL_TREEDISPLAYMANAGER_SIZE, 
          long style = SYMBOL_TREEDISPLAYMANAGER_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEDISPLAYMANAGER_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEDISPLAYMANAGER_POSITION, 
          const wxSize& size = SYMBOL_TREEDISPLAYMANAGER_SIZE, 
          long style = SYMBOL_TREEDISPLAYMANAGER_STYLE );

  ~TreeDisplayManager();
  
  void Append( pPhysicalDisplay_t pPhysicalDisplay );
  
  void Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase );
  void Delete( wxTreeItemId id );
  
  void Add( pAudio_t pAudio );
  
  void SetStaticTextInfo( wxStaticText* pstInfo );
  void SetSliders( wxSlider* sliderSeek, wxSlider* sliderZ, wxSlider* sliderVolume, wxSlider* sliderFader, wxSlider* sliderMaster );
  void SetButtonEvent( signalBtnEvent_t* signal ) { 
    m_psignalBtnEvent = signal; 
    namespace args = boost::phoenix::arg_names;
    m_connectionBtnEvent = signal->connect( boost::phoenix::bind( &TreeDisplayManager::HandleBtnEvent, this, args::arg1 ) );  
  }
  
  boost::signals2::connection ConnectSignalBtnEvent( const slotBtnEvent_t& slot ) {
    return m_psignalBtnEvent->connect( slot );
  }
  
  signalAppendWaveformView_t m_signalAppendWaveformView;
  signalAppendKeyFrameView_t m_signalAppendKeyframeView;

  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_TREEDISPLAYMANAGER
  };

  signalBtnEvent_t* m_psignalBtnEvent;
  boost::signals2::connection m_connectionBtnEvent;
  
  wxTreeItemId m_idOld;
  
  TreeItemResources m_resources;
  
  pTreeItemBase_t m_pTreeItemRoot; // root item tracked here for serialization root
  
  typedef std::map<void*,pTreeItemBase_t> mapDecoder_t;  // void* is from wxTreeItemId
  mapDecoder_t m_mapDecoder;
  
  CommonGuiElements m_guiElements;
  
  void Init();
  void CreateControls();
  
  void HandleContextMenu( wxTreeEvent& event );
  void HandleSelectionChanged( wxTreeEvent& event );
  void HandleSelectionChanging( wxTreeEvent& event );
  void HandleItemActivated( wxTreeEvent& event );
  void HandleItemDeleted( wxTreeEvent& event );
  
  void HandleBtnEvent( BtnEvent );
  
  void RemoveSelectOld( void );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips( void ) { return true; };
  
};

#endif	/* TREEDISPLAYMANAGER_H */


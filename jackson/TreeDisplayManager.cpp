/* 
 * File:   TreeDisplayManager.cpp
 * Author: rpb
 * 
 * Created on April 5, 2015, 10:11 PM
 */

//#include <map>
//#include <vector>
//#include <list>
#include <algorithm> 

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <boost/signals2.hpp>
#include <boost/chrono/chrono_io.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/menu.h>
#include <wx/image.h>

#include <wx/bitmap.h>
#include <wx/rawbmp.h>

#include <wx/filedlg.h>
#include <wx/textdlg.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <wx/event.h>

#include "common.h"
#include "MediaStreamDecode.h"

#include "tex2.h"

#include "RawImage.h"
#include "EventImage.h"

#include "Audio.h"
#include "AudioQueue.h"

#include "Outline.h"

#include "SceneElement.h"
#include "SEGrid.h"

#include "InteractiveTransform.h"
#include "TreeDisplayManager.h"

/*
  scenemanager inherits from openglcanvas, 
  and has a parent of physicaldisplay
  physicaldisplay contains FrameProjection
*/

IMPLEMENT_DYNAMIC_CLASS( TreeDisplayManager, wxTreeCtrl )

class TreeItemBase {
  friend class boost::serialization::access;
public:
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
  
  TreeItemBase( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources ): m_id( id_ ), m_resources( resources ) {}
  virtual ~TreeItemBase( void ) {}
  virtual void ShowContextMenu( void ) {}
  virtual void SetSelected( CommonGuiElements& ) {}
  virtual void RemoveSelected( CommonGuiElements& ) {}
  virtual void DeletingChild( wxTreeItemId id ) {};
  wxTreeItemId GetTreeItemId( void ) { return m_id; }
  
  void HandleRename( wxCommandEvent& event );
  
  void Rename( const wxString& sPrompt, const wxString& sDefault );
  void Rename( const wxString& sPrompt = "Change Text:" );
  
protected:
  
  wxTreeItemId m_id;  // identifier of this part of the tree control
  
  TreeDisplayManager::TreeItemResources& m_resources;
  
  typedef std::map<void*,pTreeItemBase_t> mapMembers_t;  // void* from wxTreeItemId, tracks owned items for access
  mapMembers_t m_mapMembers; 
  
  struct member_t {
    unsigned int m_type;
    void* m_void;
    pTreeItemBase_t m_pTreeItemBase;
    member_t( unsigned int type, void* void_, pTreeItemBase_t p ): m_type( type ), m_void( void_ ), m_pTreeItemBase( p ) {};
  };
  typedef std::vector<member_t> vMembers_t; // tracks ordered list for serialization
  vMembers_t m_vMembers;

  wxTreeItemId AppendSubItem( const std::string& sLabel ); // add the visual menu item
  pTreeItemBase_t  AppendSubItem( wxTreeItemId id, TreeItemBase* p );  // add the associated real object
  
  // convenience member functions for derived classes
  void AddMember( unsigned int type, wxTreeItemId id, pTreeItemBase_t p ); // stuff to be serialized
  void DeleteMember( wxTreeItemId id ); // stuff not to be serialized
  
  template<typename TreeItem, typename id_t>  
  TreeItem* AddTreeItem( const std::string& sLabel, id_t idType ) { 
    wxTreeItemId id = AppendSubItem( sLabel );
    TreeItem* p = new TreeItem( id, m_resources );
    assert( 0 != p );
    pTreeItemBase_t pTreeItemBase = AppendSubItem( id, p );
    assert( 0 != pTreeItemBase.get() );
    AddMember( idType, id, pTreeItemBase );
    return p;
  }

private:
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    std::string sLabel( m_resources.tree.GetItemText( m_id ) );
    ar & sLabel;
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    std::string sLabel;
    ar & sLabel;
    m_resources.tree.SetItemText( m_id, sLabel );
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
};

void TreeItemBase::AddMember( unsigned int type, wxTreeItemId id, pTreeItemBase_t p ) {
  assert( 0 != id.GetID() );
  assert( 0 != p.get() );
  m_vMembers.push_back( member_t( type, id.GetID(), p ) );
  m_mapMembers.insert( mapMembers_t::value_type( id.GetID(), p ) );
}

void TreeItemBase::DeleteMember( wxTreeItemId id ) {
  
  struct matchId {
    bool operator()( const TreeItemBase::member_t& member ) const { return member.m_void == m_v; }
    matchId( void* v ): m_v( v ) {};
    void* m_v;
  };
  
  mapMembers_t::const_iterator iterMap = m_mapMembers.find( id.GetID() );
  if ( m_mapMembers.end() != iterMap ) {
    m_mapMembers.erase( iterMap );
  }
  else assert( 0 );
  vMembers_t::iterator iterVec = std::find_if( m_vMembers.begin(), m_vMembers.end(), matchId( id.GetID() ) );
  assert( m_vMembers.end() != iterVec );
  m_vMembers.erase( iterVec );
}

void TreeItemBase::HandleRename( wxCommandEvent& event ) { 
  Rename();
}

void TreeItemBase::Rename( const wxString& sPrompt ) { 
  Rename( sPrompt, m_resources.tree.GetItemText( m_id ) );
}

void TreeItemBase::Rename( const wxString& sPrompt, const wxString& sDefault ) { 
  wxTextEntryDialog* p = new wxTextEntryDialog( &m_resources.tree, sPrompt, sPrompt, sDefault );
  if ( wxID_OK == p->ShowModal() ) {
    m_resources.tree.SetItemText( m_id, p->GetValue() );
  }
}

wxTreeItemId TreeItemBase::AppendSubItem( const std::string& sLabel ) {
  wxTreeItemId id = m_resources.tree.AppendItem( m_id, sLabel );
  m_resources.tree.EnsureVisible( id );
  return id;
}

TreeItemBase::pTreeItemBase_t TreeItemBase::AppendSubItem( wxTreeItemId id, TreeItemBase* p ) {
  pTreeItemBase_t pTreeItemBase( p );
  m_resources.tree.Add( id, pTreeItemBase );
  return pTreeItemBase;
}

// ===============

class TreeItemSceneElementBase : public TreeItemBase {
  friend class boost::serialization::access;
public:

  TreeItemSceneElementBase( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemSceneElementBase( void );

  void HandleDelete( wxCommandEvent& event );

  virtual void AppendToScenePanel( void ) {};
  virtual void DetachFromScenePanel( void ) {};

protected:
  
private:

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemSceneElementBase::TreeItemSceneElementBase( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
  : TreeItemBase( id, resources )
{
}

TreeItemSceneElementBase::~TreeItemSceneElementBase(void) {
}

void TreeItemSceneElementBase::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemSceneElementBase Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}

// ================

class TreeItemMonoAudioChannel {
  friend class boost::serialization::access;
public:

  TreeItemMonoAudioChannel( TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemMonoAudioChannel( void );

  void BrowseForAudio( 
    const std::string& sPrompt, 
    std::string& sAudioDirectory, std::string& sFilePath, std::string& sFilename );

  void SetChannel( unsigned int ix );
  unsigned int GetChannel( void ) const { return m_nChannel; }

  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );

  virtual void AppendToScenePanel( void );
  virtual void DetachFromScenePanel( void );

protected:
private:

  struct ChannelId: public wxObject {
    unsigned int nChannel;
    ChannelId( unsigned int nChannel_ ): nChannel( nChannel_ ) {};
  };

  //boost::signals2::connection m_connectionAudioReady;
  //boost::signals2::connection m_connectionDecodeComplete;

  boost::signals2::connection m_connectAudio;

  //MediaStreamDecode m_player;

  TreeDisplayManager::TreeItemResources& m_resources;

  typedef std::vector<int16_t> vSamples_t;
  vSamples_t m_vSamples;

  Audio::pAudioQueue_t m_pAudioQueue;
  unsigned int m_nChannel;

  WaveformView* m_pwfv;
  KeyFrameView* m_pkfv;

  int m_intOldVolume; 

  //void HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples );
  void HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples, int offset );

  void HandleDecodeComplete( void );

  void SendBuffer( void );

  void HandleScrollChangedVolume( wxScrollEvent& event );

  //void HandlePlayBuffer( TreeDisplayManager::BtnEvent event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
//    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
//    ar & m_sAudioDirectory;
//    ar & m_sFilePath;
    ar & m_intOldVolume;
    ar & m_nChannel;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
//    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
//    ar & m_sAudioDirectory;
//    ar & m_sFilePath;  // will this work with an empty string?

    ar & m_intOldVolume;  // need to set something?
    m_pAudioQueue->SetAttenuator( m_intOldVolume );

    unsigned int nChannel;
    ar & nChannel;
    SetChannel( nChannel );

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemMonoAudioChannel::TreeItemMonoAudioChannel( TreeDisplayManager::TreeItemResources& resources )
  : m_resources( resources), m_pwfv( 0 ), m_pkfv( 0 ), m_intOldVolume( 0 ), m_nChannel( 0 )
{

  m_pAudioQueue.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( m_nChannel, m_pAudioQueue );

  // will need intermediate receiver to demux the channels
  // so may be a call rather than an event namespace args = boost::phoenix::arg_names;
  //m_connectionAudioReady = m_player.ConnectAudioReady( boost::phoenix::bind( &TreeItemMonoAudio::HandleAudioForBuffer, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );
  //m_connectionDecodeComplete = m_player.ConnectDecodeDone( boost::phoenix::bind( &TreeItemMonoAudio::HandleDecodeComplete, this ) );

}

TreeItemMonoAudioChannel::~TreeItemMonoAudioChannel( void ) {

  m_pwfv = 0;
  m_pkfv = 0;

  m_resources.pAudio->Detach( m_nChannel, m_pAudioQueue );
  m_pAudioQueue.reset();

  //m_connectionDecodeComplete.disconnect();
  //m_connectionAudioReady.disconnect();

}

void TreeItemMonoAudioChannel::SetChannel( unsigned int ix) {
  if ( ix != m_nChannel ) {
    m_resources.pAudio->Detach( m_nChannel, m_pAudioQueue );
  }
  m_nChannel = ix;
  m_resources.pAudio->Attach( m_nChannel, m_pAudioQueue );
}

void TreeItemMonoAudioChannel::AppendToScenePanel( void ) {

  m_pwfv = *m_resources.tree.m_signalAppendWaveformView();
  assert( 0 != m_pwfv );
  m_pwfv->SetSamples( &m_vSamples );
  m_pkfv = *m_resources.tree.m_signalAppendKeyframeView();
  assert( 0 != m_pkfv );

}

void TreeItemMonoAudioChannel::DetachFromScenePanel( void ) {
  m_pwfv = 0;
  m_pwfv = 0;
}

void TreeItemMonoAudioChannel::SetSelected( CommonGuiElements& elements ) {

  namespace args = boost::phoenix::arg_names;
  m_connectAudio = m_resources.pAudio->m_signalFramesProcessed.connect( boost::phoenix::bind( &WaveformView::UpdatePlayCursor, m_pwfv, args::arg1 ) );

  elements.pSliderVolume->Bind( wxEVT_SCROLL_CHANGED, &TreeItemMonoAudioChannel::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Bind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMonoAudioChannel::HandleScrollChangedVolume, this );
  elements.pSliderVolume->SetValue( m_intOldVolume );
  elements.pSliderVolume->Enable();
}

void TreeItemMonoAudioChannel::RemoveSelected( CommonGuiElements& elements ) {
  elements.pSliderVolume->Enable( false );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_CHANGED, &TreeItemMonoAudioChannel::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMonoAudioChannel::HandleScrollChangedVolume, this );

  m_connectAudio.disconnect();
}

// need a separate one for each waveform
void TreeItemMonoAudioChannel::HandleScrollChangedVolume( wxScrollEvent& event ) {
  int val = event.GetPosition();
  m_pAudioQueue->SetAttenuator( val );
  m_intOldVolume = val;
}

// todo:  need to confirm is mono, or choose a specific channel
void TreeItemMonoAudioChannel::BrowseForAudio( 
  const std::string& sPrompt, 
  std::string& sAudioDirectory, std::string& sFilePath, std::string& sFilename ) {

  //std::cout << "LoadMusic" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_resources.tree.GetParent(),
    //m_pPhysicalDisplay->GetFrame(), 
    sPrompt, sAudioDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    sAudioDirectory = dialogOpenFile.GetDirectory();

    sFilePath = dialogOpenFile.GetPath(); 
    sFilename = dialogOpenFile.GetFilename();
  }
}

//void TreeItemMonoAudio::HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples ) {
//  std::cout << "TreeItemMusic::HandleAudio" << std::endl;
//  assert( 2 == nChannels );
//  assert( AV_SAMPLE_FMT_S16P == format );
//  const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
//  boost::strict_lock<AudioQueue<int16_t> > guardLeft( *m_pAudioQueueLeft );
//  m_pAudioQueueLeft->AddSamples( nSamples, pSamples[0], guardLeft );
//  boost::strict_lock<AudioQueue<int16_t> > guardRight( *m_pAudioQueueRight );
//  m_pAudioQueueRight->AddSamples( nSamples, pSamples[1], guardRight );
//}

// audio buffers coming from libav decoder
// going to be difficult to deal with on a stereo stream
// so therefore will need channel count, channel id
void TreeItemMonoAudioChannel::HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples, int offset ) {
  // offset is index into which channel to use of nChannels
  switch ( format ) {
    case AV_SAMPLE_FMT_S16P: // multiple buffers
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* p = pSamples[offset];
      for ( int cnt = 0; cnt < nSamples; ++cnt ) {
        m_vSamples.push_back( *p );  ++p;
      }
    }
    break;
    case AV_SAMPLE_FMT_S16:   // interleaved
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* p = pSamples[0];
      p += offset;
      for ( int cnt = 0; cnt < nSamples; ++cnt ) {
        m_vSamples.push_back( *p ); p += nChannels;
      }
    }
    break;
    default:
      assert( 0 );
  }

}

void TreeItemMonoAudioChannel::SendBuffer( void ) {
  if ( 0 == m_vSamples.size() && 0 == m_vSamples.size() ) {
    std::cout << "Channel " << m_nChannel << " nothing to play" << std::endl;
  }
  else {
    std::cout << "Channel " << m_nChannel << ": stuffing buffers ..." << std::endl;
    boost::strict_lock<AudioQueue<int16_t> > guard( *m_pAudioQueue );
    m_pAudioQueue->AddSamples( m_vSamples.size(), &(m_vSamples[0]), guard );
    std::cout << "ready to play" << std::endl;
  }
}

void TreeItemMonoAudioChannel::HandleDecodeComplete( void ) {
  std::cout << "Audio Decode Complete: " << m_nChannel << ", " << m_vSamples.size() << ", " << m_vSamples.size() << " samples" << std::endl;
  if ( 0 != m_pwfv )  m_pwfv->SetSamples( &m_vSamples );  
  //m_pwfvFrontLeft->Refresh();
}

// ================

class TreeItemMonoAudio: public TreeItemSceneElementBase {
  friend class boost::serialization::access;
public:

  TreeItemMonoAudio( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemMonoAudio( void );

  void SetChannel( unsigned int ix );
  unsigned int GetChannel( void ) const { return m_nChannel; }

  virtual void ShowContextMenu( void );

  void BrowseForAudio( void );
  void SelectAudio( void );

  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );

  virtual void AppendToScenePanel( void );
  virtual void DetachFromScenePanel( void );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,
    MISelectAudio, MIAudioSeek, MIAudioLoadBuffer, MIAudioStop, MIAudioStats, MIAudioSendBuffer, MIAudioSetChannel,
    MIDelete, MIRename
  };

  struct ChannelId: public wxObject {
    unsigned int nChannel;
    ChannelId( unsigned int nChannel_ ): nChannel( nChannel_ ) {};
  };

  std::string m_sAudioDirectory;
  std::string m_sFilePath;

  boost::signals2::connection m_connectionBtnEvent;

  boost::signals2::connection m_connectionAudioReady;
  boost::signals2::connection m_connectionDecodeComplete;

  boost::signals2::connection m_connectAudio;

  MediaStreamDecode m_player;

  typedef std::vector<int16_t> vSamples_t;
  vSamples_t m_vSamples;

  Audio::pAudioQueue_t m_pAudioQueue;
  unsigned int m_nChannel;

  WaveformView* m_pwfv;

  KeyFrameView* m_pkfv;

  int m_intOldVolume; 

  void HandleSelectAudio( wxCommandEvent& event );
  void HandleSetChannel( wxCommandEvent& event );

  //void HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples );
  void HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples, int offset );

  void HandleDecodeComplete( void );

  void HandleStats( wxCommandEvent& event );
  void HandleSeek( wxCommandEvent& event );
  void HandleLoadBuffer( wxCommandEvent& event );
  void HandleSendBuffer( wxCommandEvent& event );
  void HandleStop( wxCommandEvent& event );

  void HandleScrollChangedVolume( wxScrollEvent& event );

  //void HandlePlayBuffer( TreeDisplayManager::BtnEvent event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
    ar & m_sAudioDirectory;
    ar & m_sFilePath;
    ar & m_intOldVolume;
    ar & m_nChannel;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
    ar & m_sAudioDirectory;
    ar & m_sFilePath;  // will this work with an empty string?
    ar & m_intOldVolume;  // need to set something?

    unsigned int nChannel;
    ar & nChannel;
    SetChannel( nChannel );

    m_pAudioQueue->SetAttenuator( m_intOldVolume );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemMonoAudio::TreeItemMonoAudio( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
  : TreeItemSceneElementBase( id, resources ), m_pwfv( 0 ), m_pkfv( 0 ), m_intOldVolume( 0 ), m_nChannel( 0 )
{
#ifdef __WXMSW__
  m_sAudioDirectory = "D:\\Data\\Projects\\Jackson 2015\\Emily\\dance\\Bounced Files";
#else
  m_sAudioDirectory = "~/Music/";
#endif

  m_pAudioQueue.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( m_nChannel, m_pAudioQueue );

  // will need intermediate receiver to demux the channels
  // so may be a call rather than an event namespace args = boost::phoenix::arg_names;
  //m_connectionAudioReady = m_player.ConnectAudioReady( boost::phoenix::bind( &TreeItemMonoAudio::HandleAudioForBuffer, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );
  m_connectionDecodeComplete = m_player.ConnectDecodeDone( boost::phoenix::bind( &TreeItemMonoAudio::HandleDecodeComplete, this ) );

}

TreeItemMonoAudio::~TreeItemMonoAudio( void ) {

  m_pwfv = 0;
  m_pkfv = 0;

  m_resources.pAudio->Detach( m_nChannel, m_pAudioQueue );
  m_pAudioQueue.reset();

  m_connectionDecodeComplete.disconnect();
  m_connectionAudioReady.disconnect();

}

void TreeItemMonoAudio::SetChannel( unsigned int ix) {
  if ( ix != m_nChannel ) {
    m_resources.pAudio->Detach( m_nChannel, m_pAudioQueue );
  }
  m_nChannel = ix;
  m_resources.pAudio->Attach( m_nChannel, m_pAudioQueue );
}

void TreeItemMonoAudio::AppendToScenePanel( void ) {

  m_pwfv = *m_resources.tree.m_signalAppendWaveformView();
  assert( 0 != m_pwfv );
  m_pwfv->SetSamples( &m_vSamples );
  m_pkfv = *m_resources.tree.m_signalAppendKeyframeView();
  assert( 0 != m_pkfv );

}

void TreeItemMonoAudio::DetachFromScenePanel( void ) {
  m_pwfv = 0;
  m_pwfv = 0;
}

void TreeItemMonoAudio::SetSelected( CommonGuiElements& elements ) {
  namespace args = boost::phoenix::arg_names;
  m_connectAudio = m_resources.pAudio->m_signalFramesProcessed.connect( boost::phoenix::bind( &WaveformView::UpdatePlayCursor, m_pwfv, args::arg1 ) );

  elements.pSliderVolume->Bind( wxEVT_SCROLL_CHANGED, &TreeItemMonoAudio::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Bind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMonoAudio::HandleScrollChangedVolume, this );
  elements.pSliderVolume->SetValue( m_intOldVolume );
  elements.pSliderVolume->Enable();
}

void TreeItemMonoAudio::RemoveSelected( CommonGuiElements& elements ) {
  elements.pSliderVolume->Enable( false );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_CHANGED, &TreeItemMonoAudio::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMonoAudio::HandleScrollChangedVolume, this );

  m_connectAudio.disconnect();
}

// need a separate one for each waveform
void TreeItemMonoAudio::HandleScrollChangedVolume( wxScrollEvent& event ) {
  int val = event.GetPosition();
  m_pAudioQueue->SetAttenuator( val );
  m_intOldVolume = val;
}

void TreeItemMonoAudio::ShowContextMenu( void ) {

  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MISelectAudio, "Select Audio" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMonoAudio::HandleSelectAudio, this, MISelectAudio );

  for ( unsigned int cnt = 0; cnt < m_resources.pAudio->GetChannelCount(); ++cnt ) {
    std::string s( "mono channel " );
    s += boost::lexical_cast<std::string>( cnt );
    pMenu->Append( MIAudioSetChannel, s ); 
    ChannelId* p = new ChannelId( cnt );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMonoAudio::HandleSetChannel, this, MIAudioSetChannel, MIAudioSetChannel, (wxObject*) p );
  }

  //  pMenu->Append( MIReset, "Reset" );
  //  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );

  //  pMenu->Append( MIVideoPause, "Pause" );
  //  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandlePause, this, MIVideoPause );

  //  pMenu->Append( MIVideoResume, "Resume" );
  //  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleResume, this, MIVideoResume );

  //pMenu->Append( MIMusicSeek, "Seek" );
  //pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSeek, this, MIMusicSeek );

  pMenu->Append( MIAudioStats, "Stats" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMonoAudio::HandleStats, this, MIAudioStats );

  pMenu->Append( MIAudioLoadBuffer, "Load Buffer" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMonoAudio::HandleLoadBuffer, this, MIAudioLoadBuffer );

  pMenu->Append( MIAudioSendBuffer, "Send Buffer" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMonoAudio::HandleSendBuffer, this, MIAudioSendBuffer );

  pMenu->Append( MIRename, "Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );

  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemSceneElementBase::HandleDelete, this, MIDelete );

  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemMonoAudio::HandleStats( wxCommandEvent& event ) {
  m_player.EmitStats();
}

void TreeItemMonoAudio::HandleLoadBuffer( wxCommandEvent& event ) {
  // need to test that the file is open
  //std::cout << "play start" << std::endl;
  m_player.Play();
}

void TreeItemMonoAudio::HandleStop( wxCommandEvent& event ) {
  m_player.Stop();  // should actually coordinate through local state machine
}

void TreeItemMonoAudio::HandleSelectAudio( wxCommandEvent& event ) {
  BrowseForAudio();
}

void TreeItemMonoAudio::HandleSetChannel( wxCommandEvent& event ) {
  ChannelId* p = (ChannelId*)event.GetEventUserData();
  unsigned int ix = p->nChannel;
  assert( ix < m_resources.pAudio->GetChannelCount() );
  SetChannel( ix );
  std::cout << "Set Channel to " << ix << std::endl;
}

// todo:  need to confirm is mono, or choose a specific channel
void TreeItemMonoAudio::BrowseForAudio( void ) {

  //std::cout << "LoadMusic" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_resources.tree.GetParent(),
    //m_pPhysicalDisplay->GetFrame(), 
    wxT("Select Mono Audio" ), m_sAudioDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sAudioDirectory = dialogOpenFile.GetDirectory();

    m_sFilePath = dialogOpenFile.GetPath(); 

    SelectAudio();

    Rename( "Button Label:", dialogOpenFile.GetFilename() );

  }
}

void TreeItemMonoAudio::SelectAudio( void ) {
  m_vSamples.clear();
  m_player.Close();
  if ( m_player.Open( m_sFilePath ) ) {  // means it needs to be closed manually or automatically
    if ( m_player.GetBestAudioStreamFound() ) {
      assert( 44100 == m_player.GetAudioSampleRate() );  // need to be a bit more flexible
    }
    //m_player.Play();  // get the stream into our local buffer, may have issue with premature closure
  }
}

//void TreeItemMonoAudio::HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples ) {
//  std::cout << "TreeItemMusic::HandleAudio" << std::endl;
//  assert( 2 == nChannels );
//  assert( AV_SAMPLE_FMT_S16P == format );
//  const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
//  boost::strict_lock<AudioQueue<int16_t> > guardLeft( *m_pAudioQueueLeft );
//  m_pAudioQueueLeft->AddSamples( nSamples, pSamples[0], guardLeft );
//  boost::strict_lock<AudioQueue<int16_t> > guardRight( *m_pAudioQueueRight );
//  m_pAudioQueueRight->AddSamples( nSamples, pSamples[1], guardRight );
//}

// audio buffers coming from libav decoder
// going to be difficult to deal with on a stereo stream
// so therefore will need channel count, channel id
void TreeItemMonoAudio::HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples, int offset ) {
  // offset is index into which channel to use of nChannels
  switch ( format ) {
    case AV_SAMPLE_FMT_S16P: // multiple buffers
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* p = pSamples[offset];
      for ( int cnt = 0; cnt < nSamples; ++cnt ) {
        m_vSamples.push_back( *p );  ++p;
      }
    }
    break;
    case AV_SAMPLE_FMT_S16:   // interleaved
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* p = pSamples[0];
      p += offset;
      for ( int cnt = 0; cnt < nSamples; ++cnt ) {
        m_vSamples.push_back( *p ); p += nChannels;
      }
    }
    break;
    default:
      assert( 0 );
  }

}

void TreeItemMonoAudio::HandleSendBuffer( wxCommandEvent& event ) {
  if ( 0 == m_vSamples.size() && 0 == m_vSamples.size() ) {
    std::cout << "nothing to play" << std::endl;
  }
  else {
    std::cout << "stuffing buffers to play " << m_nChannel << " ..." << std::endl;
    boost::strict_lock<AudioQueue<int16_t> > guard( *m_pAudioQueue );
    m_pAudioQueue->AddSamples( m_vSamples.size(), &(m_vSamples[0]), guard );
    std::cout << "ready to play" << std::endl;
  }
}

void TreeItemMonoAudio::HandleDecodeComplete( void ) {
  std::cout << "Audio Decode Complete: " << m_nChannel << ", " << m_vSamples.size() << ", " << m_vSamples.size() << " samples" << std::endl;
  if ( 0 != m_pwfv )  m_pwfv->SetSamples( &m_vSamples );  //m_pwfvFrontLeft->Refresh();
}

// ================

// 20150609 refactor to use two sounds.  then can have mono-channel sounds

class TreeItemMusic: public TreeItemSceneElementBase {
  friend class boost::serialization::access;
public:
  
  TreeItemMusic( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemMusic( void );
  
  virtual void ShowContextMenu( void );
  
  void BrowseForMusic( void );
  void SelectMusic( void );
  
  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );
  
  virtual void AppendToScenePanel( void );
  virtual void DetachFromScenePanel( void );

protected:
private:

  enum {
    ID_Null = wxID_HIGHEST,
    MISelectMusic, MIMusicSeek, MIMusicLoadBuffer, MIMusicStop, MIMusicStats, MIMusicSendBuffer, MIAudioSetChannel,
    MIDelete, MIRename
  };
  
  struct ChannelId: public wxObject {
    unsigned int nChannel;
    ChannelId( unsigned int nChannel_ ): nChannel( nChannel_ ) {};
  };

  std::string m_sMusicDirectory;
  std::string m_sFilePath;
  
  boost::signals2::connection m_connectionBtnEvent;
  
  boost::signals2::connection m_connectionAudioReady;
  boost::signals2::connection m_connectionDecodeComplete;
  
  boost::signals2::connection m_connectAudioLeft;
  boost::signals2::connection m_connectAudioRight;
  
  MediaStreamDecode m_player;

  unsigned int m_channelLeft;
  unsigned int m_channelRight;
  
  typedef std::vector<int16_t> vSamples_t;
  vSamples_t m_vSamplesLeft;
  vSamples_t m_vSamplesRight;
  
  Audio::pAudioQueue_t m_pAudioQueueLeft;
  Audio::pAudioQueue_t m_pAudioQueueRight;
  
  WaveformView* m_pwfvLeft;  // make this more generic later, maybe stereo vs mono as well
  WaveformView* m_pwfvRight;

  KeyFrameView* m_pkfvLeft;
  KeyFrameView* m_pkfvRight;
  
  int m_intOldVolumeLeft; 
  int m_intOldVolumeRight; 

  void SetChannel( unsigned int nChannel );
  
  void HandleSelectMusic( wxCommandEvent& event );
  void HandleSetChannel( wxCommandEvent& event );

  void HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples );
  void HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples );
  
  void HandleDecodeComplete( void );
    
  void HandleStats( wxCommandEvent& event );
  void HandleSeek( wxCommandEvent& event );
  void HandleLoadBuffer( wxCommandEvent& event );
  void HandleSendBuffer( wxCommandEvent& event );
  void HandleStop( wxCommandEvent& event );
  
  void HandleScrollChangedVolume( wxScrollEvent& event );

  //void HandlePlayBuffer( TreeDisplayManager::BtnEvent event );
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
    ar & m_sMusicDirectory;
    ar & m_sFilePath;

    ar & m_intOldVolumeLeft;
    ar & m_intOldVolumeRight;

    ar & m_channelLeft / 2; // flaky way of doing this for now
    //ar & m_channelRight;
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
    ar & m_sMusicDirectory;
    ar & m_sFilePath;  // will this work with an empty string?

    ar & m_intOldVolumeLeft;
    m_pAudioQueueLeft->SetAttenuator( m_intOldVolumeLeft );
    ar & m_intOldVolumeRight;
    m_pAudioQueueRight->SetAttenuator( m_intOldVolumeRight );    

    unsigned int n;
    ar & n;
    SetChannel( n / 2 ); // flaky way of doing this
     
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
};

TreeItemMusic::TreeItemMusic( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
: TreeItemSceneElementBase( id, resources ), 
  m_pwfvLeft( 0 ), m_pwfvRight( 0 ), 
  m_intOldVolumeLeft( 0 ), m_intOldVolumeRight( 0 ),
  m_channelLeft( 0 ), m_channelRight( 1 )
{
#ifdef __WXMSW__
  m_sMusicDirectory = "D:\\Data\\Projects\\Jackson 2015\\Emily\\dance\\Bounced Files";
#else
  m_sMusicDirectory = "~/Music/";
#endif

  m_pAudioQueueLeft.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( 0, m_pAudioQueueLeft );
  
  m_pAudioQueueRight.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( 1, m_pAudioQueueRight );
  
  namespace args = boost::phoenix::arg_names;
  m_connectionAudioReady = m_player.ConnectAudioReady( boost::phoenix::bind( &TreeItemMusic::HandleAudioForBuffer, this, args::arg1, args::arg2, args::arg3, args::arg4 ) );
  m_connectionDecodeComplete = m_player.ConnectDecodeDone( boost::phoenix::bind( &TreeItemMusic::HandleDecodeComplete, this ) );
  
}

TreeItemMusic::~TreeItemMusic( void ) {
  
  m_pwfvLeft = 0;
  m_pwfvRight = 0;
  
  m_resources.pAudio->Detach( 0, m_pAudioQueueLeft );
  m_pAudioQueueLeft.reset();
  
  m_resources.pAudio->Detach( 1, m_pAudioQueueRight );
  m_pAudioQueueRight.reset();
  
  m_connectionDecodeComplete.disconnect();
  m_connectionAudioReady.disconnect();
  
}

void TreeItemMusic::SetChannel( unsigned int nChannel) {
  //unsigned int nChannel = ix * 2;
  nChannel *= 2;
  if ( nChannel != m_channelLeft ) {
    m_resources.pAudio->Detach( m_channelLeft, m_pAudioQueueLeft );
  }
  if ( ( nChannel + 1 ) != m_channelRight ) {
    m_resources.pAudio->Detach( m_channelRight, m_pAudioQueueRight );
  }
  m_channelLeft = nChannel;
  m_resources.pAudio->Attach( m_channelLeft, m_pAudioQueueLeft );
  m_channelRight = m_channelLeft + 1;
  m_resources.pAudio->Attach( m_channelRight, m_pAudioQueueRight );
}

void TreeItemMusic::AppendToScenePanel( void ) {
  
  m_pwfvLeft = *m_resources.tree.m_signalAppendWaveformView();
  assert( 0 != m_pwfvLeft );
  m_pwfvLeft->SetSamples( &m_vSamplesLeft );
  m_pkfvLeft = *m_resources.tree.m_signalAppendKeyframeView();
  assert( 0 != m_pkfvLeft );
  
  m_pwfvRight = *m_resources.tree.m_signalAppendWaveformView();
  assert( 0 != m_pwfvRight );
  m_pwfvRight->SetSamples( &m_vSamplesRight );
  m_pkfvRight = *m_resources.tree.m_signalAppendKeyframeView();
  assert( 0 != m_pkfvRight );
}

void TreeItemMusic::DetachFromScenePanel( void ) {
  m_pwfvLeft = 0;
  m_pkfvLeft = 0;
  m_pwfvRight = 0;
  m_pkfvRight = 0;
}

void TreeItemMusic::SetSelected( CommonGuiElements& elements ) {
  namespace args = boost::phoenix::arg_names;
  m_connectAudioLeft = m_resources.pAudio->m_signalFramesProcessed.connect( boost::phoenix::bind( &WaveformView::UpdatePlayCursor, m_pwfvLeft, args::arg1 ) );
  m_connectAudioRight = m_resources.pAudio->m_signalFramesProcessed.connect( boost::phoenix::bind( &WaveformView::UpdatePlayCursor, m_pwfvRight, args::arg1 ) );
  
  elements.pSliderVolume->Bind( wxEVT_SCROLL_CHANGED, &TreeItemMusic::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Bind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMusic::HandleScrollChangedVolume, this );
  elements.pSliderVolume->SetValue( m_intOldVolumeLeft );
  elements.pSliderVolume->Enable();
}

void TreeItemMusic::RemoveSelected( CommonGuiElements& elements ) {
  elements.pSliderVolume->Enable( false );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_CHANGED, &TreeItemMusic::HandleScrollChangedVolume, this );
  elements.pSliderVolume->Unbind( wxEVT_SCROLL_THUMBTRACK, &TreeItemMusic::HandleScrollChangedVolume, this );
  
  m_connectAudioLeft.disconnect();
  m_connectAudioRight.disconnect();
}

// need a separate one for each waveform
void TreeItemMusic::HandleScrollChangedVolume( wxScrollEvent& event ) {
  int val = event.GetPosition();
  m_pAudioQueueLeft->SetAttenuator( val );
  m_pAudioQueueRight->SetAttenuator( val );
  m_intOldVolumeLeft = val;
  m_intOldVolumeRight = val;
}

void TreeItemMusic::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MISelectMusic, "Select Music" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSelectMusic, this, MISelectMusic );

  unsigned int ttl = m_resources.pAudio->GetChannelCount() / 2; // rounds off odd channel
  for ( unsigned int cnt = 0; cnt < ttl; ++cnt ) {
    std::string s( "Stereo Set " );
    s += boost::lexical_cast<std::string>( cnt );
    pMenu->Append( MIAudioSetChannel, s ); 
    ChannelId* p = new ChannelId( cnt );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSetChannel, this, MIAudioSetChannel, MIAudioSetChannel, (wxObject*) p );
  }

  //  pMenu->Append( MIReset, "Reset" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
//  pMenu->Append( MIVideoPause, "Pause" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandlePause, this, MIVideoPause );
  
//  pMenu->Append( MIVideoResume, "Resume" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleResume, this, MIVideoResume );
  
  //pMenu->Append( MIMusicSeek, "Seek" );
  //pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSeek, this, MIMusicSeek );
  
  pMenu->Append( MIMusicStats, "Stats" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleStats, this, MIMusicStats );
  
  pMenu->Append( MIMusicLoadBuffer, "Load Buffer" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleLoadBuffer, this, MIMusicLoadBuffer );
  
  pMenu->Append( MIMusicSendBuffer, "Send Buffer" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSendBuffer, this, MIMusicSendBuffer );
  
  pMenu->Append( MIRename, "Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemSceneElementBase::HandleDelete, this, MIDelete );
  
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemMusic::HandleSetChannel( wxCommandEvent& event ) {
  ChannelId* p = (ChannelId*)event.GetEventUserData();
  unsigned int ix = p->nChannel;
  assert( ix < ( m_resources.pAudio->GetChannelCount() / 2 ) );
  SetChannel( ix );
  std::cout << "Set Stereo Pair to " << ix << std::endl;
}

void TreeItemMusic::HandleStats( wxCommandEvent& event ) {
  m_player.EmitStats();
}

void TreeItemMusic::HandleLoadBuffer( wxCommandEvent& event ) {
  // need to test that the file is open
  //std::cout << "play start" << std::endl;
  m_player.Play();
}

void TreeItemMusic::HandleStop( wxCommandEvent& event ) {
  m_player.Stop();  // should actually coordinate through local state machine
}

void TreeItemMusic::HandleSelectMusic( wxCommandEvent& event ) {
  BrowseForMusic();
}

void TreeItemMusic::BrowseForMusic( void ) {
  
  //std::cout << "LoadMusic" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_resources.tree.GetParent(),
    //m_pPhysicalDisplay->GetFrame(), 
    wxT("Select Music" ), m_sMusicDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sMusicDirectory = dialogOpenFile.GetDirectory();
    
    m_sFilePath = dialogOpenFile.GetPath(); 
    
    SelectMusic();
    
    Rename( "Button Label:", dialogOpenFile.GetFilename() );
    
  }
}

void TreeItemMusic::SelectMusic( void ) {
  m_vSamplesLeft.clear();
  m_vSamplesRight.clear();
  m_player.Close();
  if ( m_player.Open( m_sFilePath ) ) {  // means it needs to be closed manually or automatically
    if ( m_player.GetBestAudioStreamFound() ) {
      assert( 44100 == m_player.GetAudioSampleRate() );  // need to be a bit more flexible
    }
    //m_player.Play();  // get the stream into our local buffer, may have issue with premature closure
  }
}

void TreeItemMusic::HandleAudioForPlay( AVSampleFormat format, void* buffers, int nChannels, int nSamples ) {
  std::cout << "TreeItemMusic::HandleAudio" << std::endl;
  assert( 2 == nChannels );
  assert( AV_SAMPLE_FMT_S16P == format );
  const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
  boost::strict_lock<AudioQueue<int16_t> > guardLeft( *m_pAudioQueueLeft );
  m_pAudioQueueLeft->AddSamples( nSamples, pSamples[0], guardLeft );
  boost::strict_lock<AudioQueue<int16_t> > guardRight( *m_pAudioQueueRight );
  m_pAudioQueueRight->AddSamples( nSamples, pSamples[1], guardRight );
}

// audio buffers coming from libav decoder
void TreeItemMusic::HandleAudioForBuffer( AVSampleFormat format, void* buffers, int nChannels, int nSamples ) {
  assert( 2 == nChannels );
  switch ( format ) {
    case AV_SAMPLE_FMT_S16P: // multiple buffers
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* pLeft = pSamples[0];
      const int16_t* pRight = pSamples[1];
      for ( int ix = 0; ix < nSamples; ++ix ) {
        m_vSamplesLeft.push_back( *pLeft );  ++pLeft;
        m_vSamplesRight.push_back( *pRight );  ++pRight;
      }
    }
      break;
    case AV_SAMPLE_FMT_S16:   // interleaved
    {
      const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
      const int16_t* p = pSamples[0];
      for ( int ix = 0; ix < nSamples; ++ix ) {
        m_vSamplesLeft.push_back( *p ); ++p;
        m_vSamplesRight.push_back( *p ); ++p;
      }
    }
      break;
    default:
      assert( 0 );
  }
  
}

void TreeItemMusic::HandleSendBuffer( wxCommandEvent& event ) {
  if ( 0 == m_vSamplesLeft.size() && 0 == m_vSamplesLeft.size() ) {
    std::cout << "nothing to play" << std::endl;
  }
  else {
    std::cout << "stuffing buffers to play ..." << std::endl;
    boost::strict_lock<AudioQueue<int16_t> > guardLeft( *m_pAudioQueueLeft );
    m_pAudioQueueLeft->AddSamples( m_vSamplesLeft.size(), &(m_vSamplesLeft[0]), guardLeft );
    boost::strict_lock<AudioQueue<int16_t> > guardRight( *m_pAudioQueueRight );
    m_pAudioQueueRight->AddSamples( m_vSamplesLeft.size(), &(m_vSamplesRight[0]), guardRight );
    std::cout << "ready to play" << std::endl;
  }
}

void TreeItemMusic::HandleDecodeComplete( void ) {
  std::cout << "Audio Decode Complete: " << m_vSamplesLeft.size() << ", " << m_vSamplesRight.size() << " samples" << std::endl;
  if ( 0 != m_pwfvLeft )  m_pwfvLeft->SetSamples( &m_vSamplesLeft );  //m_pwfvFrontLeft->Refresh();
  if ( 0 != m_pwfvRight ) m_pwfvRight->SetSamples( &m_vSamplesRight ); //m_pwfvFrontRight->Refresh();
}

// ================

class TreeItemDMX: public TreeItemSceneElementBase {
  friend class boost::serialization::access;
public:

  TreeItemDMX( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemDMX( void );

  virtual void ShowContextMenu( void );

  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,

    MIDelete, MIRename
  };

  KeyFrameView* m_pkfvRight;

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemDMX::TreeItemDMX( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
  : TreeItemSceneElementBase( id, resources )
{
}

TreeItemDMX::~TreeItemDMX( void ) {
}

void TreeItemDMX::SetSelected( CommonGuiElements& elements ) {
}

void TreeItemDMX::RemoveSelected( CommonGuiElements& elements ) {
}

void TreeItemDMX::ShowContextMenu( void ) {

  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );

  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemSceneElementBase::HandleDelete, this, MIDelete );

  m_resources.tree.PopupMenu( pMenu );
}

// ================

class TreeItemMidi: public TreeItemSceneElementBase {
  friend class boost::serialization::access;
public:

  TreeItemMidi( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemMidi( void );

  virtual void ShowContextMenu( void );

  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,

    MIDelete, MIRename
  };

  KeyFrameView* m_pkfvRight;

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemMidi::TreeItemMidi( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
  : TreeItemSceneElementBase( id, resources )
{
}

TreeItemMidi::~TreeItemMidi( void ) {
}

void TreeItemMidi::SetSelected( CommonGuiElements& elements ) {
}

void TreeItemMidi::RemoveSelected( CommonGuiElements& elements ) {
}

void TreeItemMidi::ShowContextMenu( void ) {

  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );

  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemSceneElementBase::HandleDelete, this, MIDelete );

  m_resources.tree.PopupMenu( pMenu );
}

// ================

class TreeItemVisualCommon: public TreeItemSceneElementBase, public InteractiveTransform {
  friend class boost::serialization::access;
public:
  
  //typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  
  typedef boost::signals2::signal<void ( const glm::mat4& )> signalTransformUpdated_t;
  typedef signalTransformUpdated_t::slot_type slotTransformUpdated_t;
  
  //TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  //TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemVisualCommon( void );
  
  boost::signals2::connection ConnectTransformUpdated( const slotTransformUpdated_t& slot ) {
    return m_signalTransformUpdated.connect( slot );
  }

  void SetSceneManager ( size_t ix );
  
  //void GetTransformMatrix( glm::mat4& matrix ) { matrix = InteractiveTransform::GetTransformMatrix(); };
  
  void HandleDelete( wxCommandEvent& event );  // compiler doesn't like in protected
  void HandleReset( wxCommandEvent& event );  // compiler doesn't like in protected
  
protected:
  
  signalTransformUpdated_t m_signalTransformUpdated;
  
  //pPhysicalDisplay_t m_pPhysicalDisplay;
  pSceneManager_t m_pSceneManager;
  size_t m_ixSceneManager;
  
  //virtual void UpdateTransformMatrix( const glm::mat4& ) {};  // originates in InteractiveTransform inheritance

  void SetSelected( CommonGuiElements& );  // from tree menu
  void RemoveSelected( CommonGuiElements& );  // from tree menu

  virtual void SceneManagerActivated( void ) {};
  
private:
};

//TreeItemVisualCommon::TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager ) 
//TreeItemVisualCommon::TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager ) 
TreeItemVisualCommon::TreeItemVisualCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources ) 
: TreeItemSceneElementBase( id, resources ), 
  //InteractiveTransform( pPhysicalDisplay->GetFrame()->GetClientSize().GetWidth(), pPhysicalDisplay->GetFrame()->GetClientSize().GetHeight() ), 
  //InteractiveTransform( pSceneManager->GetClientSize().GetWidth(), pSceneManager->GetClientSize().GetHeight() ),
  InteractiveTransform( 20, 10 ) // dummy till assigned to SceneManager 
  //m_bActive( false ), 
  //m_pPhysicalDisplay( pPhysicalDisplay ), 
  //m_pSceneManager( pSceneManager )
{
  
}

TreeItemVisualCommon::~TreeItemVisualCommon( void ) {
}

void TreeItemVisualCommon::SetSceneManager ( size_t ix ) {
  assert( m_resources.vpSceneManager.size() > ix );
  m_ixSceneManager = ix;
  m_pSceneManager = m_resources.vpSceneManager[ ix ];
  InteractiveTransform::Set( m_pSceneManager->GetClientSize().GetWidth(), m_pSceneManager->GetClientSize().GetHeight() );
  SceneManagerActivated();
}

void TreeItemVisualCommon::SetSelected( CommonGuiElements& cge ) {
  InteractiveTransform::Activate( m_pSceneManager.get(), cge.pSliderZ, cge.pSliderFader );
}

void TreeItemVisualCommon::RemoveSelected( CommonGuiElements& cge ) {
  InteractiveTransform::DeActivate();
}

void TreeItemVisualCommon::HandleReset( wxCommandEvent& event ) {
  std::cout << "Reset" << std::endl;
  ResetTransformMatrix();
}

void TreeItemVisualCommon::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Tree Item Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}

// ================

class TreeItemGrid: public TreeItemVisualCommon {
  friend class boost::serialization::access;
public:
  
  //typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  TreeItemGrid( 
    //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
    wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  virtual ~TreeItemGrid( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIDelete, MIReset, MIRename
  };
  
  typedef SceneManager::key_t key_t;
  typedef boost::shared_ptr<SEGrid> pSEGrid_t;
  
  pSEGrid_t m_pGrid;
  key_t m_keyGrid;
  
  virtual void UpdateTransformMatrix( const glm::mat4& );
  
};

TreeItemGrid::TreeItemGrid( 
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager)
  wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager)
: 
  //TreeItemVisualCommon( id, resources, pPhysicalDisplay, pSceneManager ),
  //TreeItemVisualCommon( id, resources, pSceneManager ),
  TreeItemVisualCommon( id, resources ),
  m_keyGrid( 0 )
{
  
  std::cout << "Tree Item Add Grid" << std::endl;
  
  m_pGrid.reset( new SEGrid(  m_resources.sCurrentPath ) );
  m_keyGrid = m_pSceneManager->Add( FpsGenerator::fps24, m_pGrid );
 
  ResetTransformMatrix();
  
}

TreeItemGrid::~TreeItemGrid( void ) {
  if ( 0 != m_keyGrid ) 
    m_pSceneManager->Delete( m_keyGrid );
}

void TreeItemGrid::UpdateTransformMatrix( const glm::mat4& matrix ) {
  m_pGrid->UpdateTransform( matrix );
  m_signalTransformUpdated( matrix );
}

void TreeItemGrid::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIReset, "Reset" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  pMenu->Append( MIRename, "Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_resources.tree.PopupMenu( pMenu );
}

// ================

class TreeItemColour: public TreeItemVisualCommon {
  friend class boost::serialization::access;
public:

  TreeItemColour( wxTreeItemId id_, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemColour( void );

  virtual void ShowContextMenu( void );

  void SetSelected( CommonGuiElements& elements );
  void RemoveSelected( CommonGuiElements& elements );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,

    MIDelete, MIRename
  };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemSceneElementBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemSceneElementBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemColour::TreeItemColour( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
  : TreeItemVisualCommon( id, resources )
{
}

TreeItemColour::~TreeItemColour( void ) {

}

void TreeItemColour::SetSelected( CommonGuiElements& elements ) {
}

void TreeItemColour::RemoveSelected( CommonGuiElements& elements ) {
}

void TreeItemColour::ShowContextMenu( void ) {

  wxMenu* pMenu = new wxMenu();

  //  pMenu->Append( MIMusicSendBuffer, "Send Buffer" );
  //  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemMusic::HandleSendBuffer, this, MIMusicSendBuffer );

  pMenu->Append( MIRename, "Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemSceneElementBase::HandleDelete, this, MIDelete );

  m_resources.tree.PopupMenu( pMenu );
}

// ================

class TreeItemImageCommon: public TreeItemVisualCommon {
  friend class boost::serialization::access;
public:
  
  //typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  //TreeItemImageCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  //TreeItemImageCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  TreeItemImageCommon( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemImageCommon( void );
  
  boost::signals2::connection ConnectFrameTrigger( const SETexture::slotFrame_t& slot ) { return m_pTexture->Connect( slot ); }
  
protected:
  
  static wxString m_sPictureDirectory;
  static wxString m_sVideoDirectory;
  
  typedef SETexture::pImage_t pImage_t;
  
  typedef SceneManager::key_t key_t;
  typedef boost::shared_ptr<SETexture> pSETexture_t;
  
  typedef RawImage::pRawImage_t pRawImage_t;
  
  key_t m_keyTexture;
  pSETexture_t m_pTexture;
  
  void Enable( FpsGenerator::FPS );
  void Enable( size_t num, size_t den );
  void Disable( void );
  
  void SetImage( pImage_t pImage );
  void SetImage( pRawImage_t pRawImage );
  
  virtual void UpdateTransformMatrix( const glm::mat4& );
  virtual void UpdateFade( float fade );
  
private:
  
  pImage_t m_pImage;
  
};

#ifdef __WXMSW__
wxString TreeItemImageCommon::m_sPictureDirectory( wxT( "C:\\Users\\me\\Pictures" ) );
wxString TreeItemImageCommon::m_sVideoDirectory( wxT( "D:\\Data\\Torrents") );
#else
wxString TreeItemImageCommon::m_sPictureDirectory( wxT( "~/Pictures/" ) );
wxString TreeItemImageCommon::m_sVideoDirectory( wxT( "~/Videos/") );
#endif

TreeItemImageCommon::TreeItemImageCommon( 
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager )
  wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
: 
  //TreeItemVisualCommon( id, resources, pPhysicalDisplay, pSceneManager ),
  //TreeItemVisualCommon( id, resources, pSceneManager ),
  TreeItemVisualCommon( id, resources ),
  m_keyTexture( 0 )
{
  
  std::cout << "Tree Item Add Image Common" << std::endl;
  
  m_pTexture.reset( new SETexture( m_resources.sCurrentPath ) );
  m_pTexture->SetTransform( InteractiveTransform::GetTransformMatrix() );
    
}

TreeItemImageCommon::~TreeItemImageCommon( void ) {
  Disable();
}

void TreeItemImageCommon::Enable(size_t num, size_t den) {
  Disable();
  m_keyTexture = m_pSceneManager->Add( num, den, m_pTexture );
}

void TreeItemImageCommon::Enable( FpsGenerator::FPS fps ) {
  Disable();
  m_keyTexture = m_pSceneManager->Add( fps, m_pTexture );
}

void TreeItemImageCommon::Disable() {
  if ( 0 != m_keyTexture ) {
    m_pSceneManager->Delete( m_keyTexture );
    m_keyTexture = 0;
  }
}

void TreeItemImageCommon::SetImage( pImage_t pImage ) {  // load picture and create object
  assert( 0 != m_pTexture.use_count() );
  assert( 0 != pImage.use_count() );
  if ( pImage->IsOk() ) {
    m_pTexture->SetImage( pImage );
    InteractiveTransform::UpdateTransformMatrix();
  }
  // some old remnants for posterity's reference
//    FrameProjection* pfp = m_pPhysicalDisplay->GetFrame();
//    wxClientDC dc( pfp );
//    dc.DrawBitmap( bitmap, wxPoint( 0, 0 ) );
  
}

void TreeItemImageCommon::SetImage( pRawImage_t pRawImage ) {  // load picture and create object
  assert( 0 != m_pTexture.use_count() );
  assert( 0 != pRawImage.use_count() );
  m_pTexture->SetImage( pRawImage );
  InteractiveTransform::UpdateTransformMatrix();
}

void TreeItemImageCommon::UpdateTransformMatrix( const glm::mat4& matrix ) {
  if ( 0 != m_pTexture.get() ) {
    m_pTexture->SetTransform( matrix );
  }
  m_signalTransformUpdated( matrix );
}

void TreeItemImageCommon::UpdateFade(float fade) {
  if ( 0 != m_pTexture.get() ) {
    m_pTexture->SetAlpha( fade );
  }
}

// ================

class TreeItemImage: public TreeItemImageCommon {
  friend class boost::serialization::access;
public:
  
  //typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  //TreeItemImage( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  //TreeItemImage( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  TreeItemImage( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemImage( void );
  
  virtual void ShowContextMenu( void );
  
protected:
  virtual void SceneManagerActivated( void );
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIDelete, MIReset, MILoadImage
  };
  
  typedef SETexture::pImage_t pImage_t;
  
  void LoadImage( void );
  void LoadImageCommon( pImage_t pImage );

  void HandleLoadImage( wxCommandEvent& event );  // reuse existing Scene Element
  
};

TreeItemImage::TreeItemImage( 
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager )
  wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
: 
  //TreeItemImageCommon( id, resources, pPhysicalDisplay, pSceneManager )
  //TreeItemImageCommon( id, resources, pSceneManager )
  TreeItemImageCommon( id, resources )
{
  
  std::cout << "Tree Item Add Image" << std::endl;
  
  ResetTransformMatrix();
  
  LoadImage();
  
  InteractiveTransform::UpdateTransformMatrix();
  
}

void TreeItemImage::SceneManagerActivated(void) {
  TreeItemImageCommon::Enable( FpsGenerator::fps24 );
}

TreeItemImage::~TreeItemImage( void ) {
  TreeItemImageCommon::Disable();
}

void TreeItemImage::HandleLoadImage( wxCommandEvent& event ) {
  LoadImage();
}

void TreeItemImage::LoadImage( void ) {  // on class instantiation, load image, create related objects
  
  pImage_t pImage( new wxImage );
  
  LoadImageCommon( pImage );
  TreeItemImageCommon::SetImage( pImage );
}

void TreeItemImage::LoadImageCommon( pImage_t pImage ) {
  
  std::cout << "TreeItemCanvasGrid LoadImage" << std::endl;  
  
  wxFileDialog dialogOpenFile( 
    m_resources.tree.GetParent(),
    //m_pPhysicalDisplay->GetFrame(), 
    wxT("Select Image" ), m_sPictureDirectory, "", 
    //"JPG Files (*.jpg)|*.jpg", 
    _("Image Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sPictureDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sPictureDirectory << std::endl;
    assert( 0 != pImage.use_count() );
    //wxImage& image( pImage.)
    assert( pImage->LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
  }
  
}

void TreeItemImage::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MILoadImage, "Load &Image" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemImage::HandleLoadImage, this, MILoadImage );
  
  pMenu->Append( MIReset, "Reset" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_resources.tree.PopupMenu( pMenu );
}

// ================

class TreeItemVideo: public TreeItemImageCommon {
  friend class boost::serialization::access;
public:
  
  //typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  //TreeItemVideo( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  //TreeItemVideo( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  TreeItemVideo( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemVideo( void );
  
  virtual void ShowContextMenu( void );
  
  virtual void SetSelected( CommonGuiElements& );
  virtual void RemoveSelected( CommonGuiElements& );

protected:
  virtual void SceneManagerActivated( void );
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIReset, MIDelete, MISelectVideo, 
    MIVideoPause, MIVideoResume, MIVideoStop,
    MIVideoSeek, MIVideoPlay, MIVideoStats
  };
  
  typedef SceneManager::key_t key_t;
  //typedef TreeItemImageCommon::pSETexture_t pSETexture_t;
  typedef RawImage::pRawImage_t pRawImage_t;
  typedef std::vector<pRawImage_t> vpRawImage_t;
  typedef std::list<pRawImage_t> lpRawImage_t;
  
  wxString m_sPictureDirectory;
  wxString m_sVideoDirectory;
  
  vpRawImage_t m_vpRawImage;
  vpRawImage_t::size_type m_ixvRawImage;  // allows cycling through m_vpRawImage
  
  lpRawImage_t m_lpRawImage;
  
  MediaStreamDecode m_player;
  bool m_bResumed;
  
  int64_t m_ttlVideoFrames;
  int64_t m_ttlAudioFrames;
  int64_t m_duration;  
  
  Audio::pAudioQueue_t m_pAudioQueueLeft;
  Audio::pAudioQueue_t m_pAudioQueueRight;
  
  AVRational m_timebase;
  
  int m_nThumbPosition;
  
  wxStaticText* m_pstInfo;
  
  boost::signals2::connection m_connectionFrameTrigger;
  boost::signals2::connection m_connectionImageReady;
  boost::signals2::connection m_connectionAudioReady;
  
  void HandleLoadVideo( wxCommandEvent& event );  // need to recode (this is where it actually starts)
  void LoadVideo( void );
  
  void HandleStats( wxCommandEvent& event );
  void HandleSeek( wxCommandEvent& event );
  void HandlePlay( wxCommandEvent& event );
  void HandlePause( wxCommandEvent& event );
  void HandleResume( wxCommandEvent& event );
  void HandleStop( wxCommandEvent& event );
  
  void HandleImage( RawImage::pRawImage_t, const structTimeSteps& );
  void HandleAudio( AVSampleFormat format, void* buffers, int nChannels, int nSamples );
  void HandleEventImage( EventImage& );
  void ShowImage( void );  // show next image from vector
  
  void HandleScrollThumbTrack( wxScrollEvent& event );
  void HandleScrollLineChange( wxScrollEvent& event );
  void HandleScrollThumbRelease( wxScrollEvent& event );
};

TreeItemVideo::TreeItemVideo( 
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager )
  wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources )
: 
  //TreeItemImageCommon( id, resources, pPhysicalDisplay, pSceneManager ),
  //TreeItemImageCommon( id, resources, pSceneManager ),
  TreeItemImageCommon( id, resources ),
  m_ttlVideoFrames( 0 ), m_ttlAudioFrames( 0 ),
  m_ixvRawImage( 0 ), m_bResumed( true ),
  m_pstInfo( 0 ), m_nThumbPosition( 0 )
{
  
  std::cout << "Tree Item Add Video" << std::endl; 
  
  //m_sPictureDirectory = wxT( "~/Pictures/");
  //m_sVideoDirectory = wxT( "~/Videos/");

  // **** will need to set a specific instance id so that multiple frames can be run
  wxApp::GetInstance()->Bind( EVENT_IMAGE, &TreeItemVideo::HandleEventImage, this );
  
  m_pAudioQueueLeft.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( 0, m_pAudioQueueLeft );
  m_pAudioQueueRight.reset( new AudioQueue<int16_t> );
  m_resources.pAudio->Attach( 0, m_pAudioQueueRight );
  
  namespace args = boost::phoenix::arg_names;
  m_connectionFrameTrigger = TreeItemImageCommon::ConnectFrameTrigger( boost::phoenix::bind( &TreeItemVideo::ShowImage, this ) );
  m_connectionImageReady = m_player.ConnectImageReady( boost::phoenix::bind( &TreeItemVideo::HandleImage, this, args::arg1, args::arg2 ) );
  m_connectionAudioReady = m_player.ConnectAudioReady( boost::phoenix::bind( &TreeItemVideo::HandleAudio, this, args::arg1, args::arg2, args::arg3, args::arg4 ) );
  
  ResetTransformMatrix();
  
  //LoadVideo();
  
  //InteractiveTransform::UpdateTransformMatrix();  // probably won't have coordinates at this time, so may not be necessary
  
}

void TreeItemVideo::SceneManagerActivated(void) {
}

TreeItemVideo::~TreeItemVideo( void ) {
  
  m_connectionAudioReady.disconnect();
  m_connectionImageReady.disconnect();
  m_connectionFrameTrigger.disconnect();
  
  TreeItemImageCommon::Disable();
  
}

void TreeItemVideo::SetSelected( CommonGuiElements& elements ) {
  //std::cout << "setting " << this->m_id.GetID() << std::endl;
  TreeItemVisualCommon::SetSelected( elements );
  m_pstInfo = elements.pstInfo;
  if ( 0 != elements.pSliderSeek ) {
    elements.pSliderSeek->Bind( wxEVT_SCROLL_THUMBTRACK, &TreeItemVideo::HandleScrollThumbTrack, this );
    elements.pSliderSeek->Bind( wxEVT_SCROLL_LINEUP, &TreeItemVideo::HandleScrollLineChange, this );
    elements.pSliderSeek->Bind( wxEVT_SCROLL_LINEDOWN, &TreeItemVideo::HandleScrollLineChange, this );
    elements.pSliderSeek->Bind( wxEVT_SCROLL_THUMBRELEASE, &TreeItemVideo::HandleScrollThumbRelease, this );
    if ( 0 < m_ttlVideoFrames ) {
      elements.pSliderSeek->SetMin( 1 );
      elements.pSliderSeek->SetMax( m_ttlVideoFrames );
      elements.pSliderSeek->Enable( true );
    }
    else {
      if ( 0 < m_duration ) {
        elements.pSliderSeek->SetMin( 1 );
        elements.pSliderSeek->SetMax( ( m_duration * m_timebase.num ) / m_timebase.den );
        elements.pSliderSeek->Enable( true );
      }
    }
    if ( 0 == m_nThumbPosition ) {
      elements.pSliderSeek->SetValue( 1 );
    }
    else {
      elements.pSliderSeek->SetValue( m_nThumbPosition );
    }

  }
}
  
void TreeItemVideo::RemoveSelected( CommonGuiElements& elements ) {
  //std::cout << "removing " << this->m_id.GetID() << std::endl;
  if ( 0 != m_pstInfo ) {
    m_pstInfo->SetLabel( "" );
    m_pstInfo = 0;
  }
  if ( 0 != elements.pSliderSeek ) {
    m_nThumbPosition = elements.pSliderSeek->GetValue();
    elements.pSliderSeek->Enable( false );
    elements.pSliderSeek->SetMin( 0 );
    elements.pSliderSeek->SetMax( 100 );
    elements.pSliderSeek->Unbind( wxEVT_SCROLL_THUMBTRACK, &TreeItemVideo::HandleScrollThumbTrack, this );
    elements.pSliderSeek->Unbind( wxEVT_SCROLL_LINEUP, &TreeItemVideo::HandleScrollLineChange, this );
    elements.pSliderSeek->Unbind( wxEVT_SCROLL_LINEDOWN, &TreeItemVideo::HandleScrollLineChange, this );
    elements.pSliderSeek->Unbind( wxEVT_SCROLL_THUMBRELEASE, &TreeItemVideo::HandleScrollThumbRelease, this );
  }
  TreeItemVisualCommon::RemoveSelected( elements );
}

  
void TreeItemVideo::HandleScrollThumbTrack( wxScrollEvent& event ) {
  //std::cout << "ThumbTrack " << event.GetPosition() << std::endl;
  m_nThumbPosition = event.GetPosition();
}

void TreeItemVideo::HandleScrollLineChange( wxScrollEvent& event ) {
  std::cout << "LineChange " << event.GetPosition() << std::endl;
  m_nThumbPosition = event.GetPosition();
}

void TreeItemVideo::HandleScrollThumbRelease( wxScrollEvent& event ) {
  std::cout << "ThumbRelease " << event.GetPosition() << std::endl;
  m_nThumbPosition = event.GetPosition();
}

void TreeItemVideo::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MISelectVideo, "Load Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleLoadVideo, this, MISelectVideo );

//  pMenu->Append( MIReset, "Reset" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
//  pMenu->Append( MIVideoPause, "Pause" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandlePause, this, MIVideoPause );
  
//  pMenu->Append( MIVideoResume, "Resume" );
//  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleResume, this, MIVideoResume );
  
  pMenu->Append( MIVideoSeek, "Seek" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleSeek, this, MIVideoSeek );
  
  pMenu->Append( MIVideoPlay, "Play" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandlePlay, this, MIVideoPlay );
  
  pMenu->Append( MIVideoStop, "Stop" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleStop, this, MIVideoStop );
  
  pMenu->Append( MIVideoStats, "Stats" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleStats, this, MIVideoStats );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemVideo::HandleLoadVideo( wxCommandEvent& event ) {
  LoadVideo();
  InteractiveTransform::UpdateTransformMatrix();  // probably won't have coordinates at this time, so may not be necessary
}

void TreeItemVideo::HandleStats( wxCommandEvent& event ) {
  m_player.EmitStats();
}

void TreeItemVideo::HandleSeek( wxCommandEvent& event ) {
  if ( 0 != m_nThumbPosition ) {
    if ( 0 < m_ttlVideoFrames ) {
      std::cout << "seeking by frame " << m_nThumbPosition << std::endl;
      m_player.SeekByFrame( m_nThumbPosition );
    }
    else {
      if ( 0 < m_duration ) {
        int64_t seek = (int64_t)m_nThumbPosition * (int64_t)m_timebase.den / (int64_t)m_timebase.num;
        std::cout 
          << "seeking by time " 
          << m_nThumbPosition << "*" 
          << m_timebase.den << "/"
          << m_timebase.num 
          << "=" << seek << std::endl;
        m_player.SeekByTime( seek );
      }
    }
  }
}

void TreeItemVideo::HandlePause( wxCommandEvent& event ) {
  std::cout << "pause not done" << std::endl;
}

void TreeItemVideo::HandlePlay( wxCommandEvent& event ) {
  // need to test that the file is open
  std::cout << "play start" << std::endl;
  m_player.Play();
}

void TreeItemVideo::HandleResume( wxCommandEvent& event ) {
  std::cout << "resume not done" << std::endl;
}

void TreeItemVideo::HandleStop( wxCommandEvent& event ) {
  m_player.Stop();  // should actually coordinate through local state machine
}

void TreeItemVideo::LoadVideo( void ) {
  
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_resources.tree.GetParent(),
    //m_pPhysicalDisplay->GetFrame(), 
    wxT("Select Video" ), m_sVideoDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sVideoDirectory = dialogOpenFile.GetDirectory();
    
    std::string sPath( dialogOpenFile.GetPath() );
    //std::cout << "chose " << sPath << std::endl;
    
    m_player.Close();
    if ( m_player.Open( sPath ) ) {  // means it needs to be closed manually or automatically
      AVRational fr = m_player.GetVideoFrameRate();
      m_ttlAudioFrames = m_player.GetTotalAudioFrames();
      m_ttlVideoFrames = m_player.GetTotalVideoFrames();
      m_duration = m_player.GetDuration();
      m_timebase = m_player.GetTimeBase();
      if ( m_player.GetBestAudioStreamFound() ) {
//        assert( 44100 == m_player.GetAudioSampleRate() );  // need to be a bit more flexible
      }
      
      TreeItemImageCommon::Enable( fr.num, fr.den );
      //m_player.Play();
    }
    
    // handle async termination of stream
    
  }
  else {
    
  }
}

void TreeItemVideo::HandleImage( RawImage::pRawImage_t pRawImage, const structTimeSteps& ts ) {
  EventImage* p( new EventImage( EVENT_IMAGE, -1, pRawImage, GetTreeItemId(), ts ) );
  //p->nVideoFrame = info.nVideoFrame;
  wxApp::GetInstance()->QueueEvent( p );
}

void TreeItemVideo::HandleAudio( AVSampleFormat format, void* buffers, int nChannels, int nSamples ) {
  if ( 2 == nChannels ) {
    const int16_t** pSamples( reinterpret_cast<const int16_t**>( buffers ) );
    switch ( format ) {
      case AV_SAMPLE_FMT_S16P: { // see TreeItemMusic for better decoder, maybe share buffering process
        boost::strict_lock<AudioQueue<int16_t> > guardLeft( *m_pAudioQueueLeft );
        m_pAudioQueueLeft->AddSamples( nSamples, pSamples[0], guardLeft );
        boost::strict_lock<AudioQueue<int16_t> > guardRight( *m_pAudioQueueRight );
        m_pAudioQueueRight->AddSamples( nSamples, pSamples[1], guardRight );
      }
        break;
    }
    
  }
}

void TreeItemVideo::HandleEventImage( EventImage& event ) {
  
  typedef boost::chrono::milliseconds ms;
  typedef boost::chrono::microseconds mu;
  static boost::chrono::duration<int64_t, boost::milli> onesec( 1000 );
  
  bool bSkip( true );
  
  if ( this->GetTreeItemId() == event.GetUser() ) {
    structTimeSteps ts( event.GetTimeSteps() );

    ts.queue2 = boost::chrono::high_resolution_clock::now();

    // by putting a lock on the vector, we may no longer need this event
    // don't do this, doesn't scale for large videos, maybe sometime in the future
    
    m_lpRawImage.push_back( event.GetRawImage() );
    
    lpRawImage_t::size_type size( m_lpRawImage.size() );
    //std::cout << "v size: " << size;
    if ( 16 < size ) {
      if ( m_bResumed ) {
        //std::cout << " paused";
        m_player.Pause();
        m_bResumed = false;
      }
    }
    //std::cout << std::endl;
    
  //  wxBitmap bitmap( *event.GetImage() );
  //  FrameProjection* pfp = (FrameProjection*) event.GetVoid();
  //  wxClientDC dc( pfp );
  //  dc.DrawBitmap( bitmap, wxPoint( 10, 10 ) );

    ts.drawn = boost::chrono::high_resolution_clock::now();
/*
    std::cout << "stat:" 
      << "  parse "  << boost::chrono::duration_cast<mu>( ts.parse - ts.start )
      << ", decode " << boost::chrono::duration_cast<mu>( ts.decoded - ts.parse )
      << ", filled " << boost::chrono::duration_cast<mu>( ts.filled - ts.decoded )
      << ", scaled " << boost::chrono::duration_cast<ms>( ts.scaled - ts.filled )
      << ", queue1 " << boost::chrono::duration_cast<mu>( ts.queue1 - ts.scaled )
      << ", xformd " << boost::chrono::duration_cast<ms>( ts.copied - ts.queue1 )
      << ", queue2 " << boost::chrono::duration_cast<mu>( ts.queue2 - ts.copied )
      << ", drawn "  << boost::chrono::duration_cast<mu>( ts.drawn - ts.queue2 )
      << std::endl;
*/    
    bSkip = false;
  }
  
  event.Skip( bSkip );
    
}

void TreeItemVideo::ShowImage( void ) {
  
  lpRawImage_t::size_type size1( m_lpRawImage.size() );
  if ( 0 != size1 ) {
    pRawImage_t pRawImage = m_lpRawImage.front();
    //m_vpImage.push_back( pImage );
    TreeItemImageCommon::SetImage( pRawImage );
    m_lpRawImage.pop_front();

    if ( 0 != m_pstInfo ) {
      std::string s;
      s += "frame " + boost::lexical_cast<std::string>( pRawImage->nVideoFrame );
      if ( AV_NOPTS_VALUE != pRawImage->pkt_dts ) {
        s+= " dts " + boost::lexical_cast<std::string>( pRawImage->pkt_dts );
      }
      if ( AV_NOPTS_VALUE != pRawImage->pkt_pts ) {
        s+= " pts " + boost::lexical_cast<std::string>( pRawImage->pkt_pts );
      }
      m_pstInfo->SetLabel( s );
    }

  }
  
  size1 = m_lpRawImage.size();
  if ( 8 > size1 ) {
    if ( !m_bResumed ) {
      //std::cout << "v resumed " << size1 << std::endl;;
      m_player.Resume();
      m_bResumed = true;
    }
  }
  
  /*
  vpImage_t::size_type size( m_vpImage.size() );
  if ( 0 != size ) {
    TreeItemImageCommon::SetImage( m_vpImage[ m_ixvImage ] );
    ++m_ixvImage;
    assert( size >= m_ixvImage );
    if ( size == m_ixvImage ) {
      m_ixvImage = 0;
    }
    else {
    }
  }
   */
}

// ================

class TreeItemProjectorArea: public TreeItemSceneElementBase {
  friend class boost::serialization::access;
public:

  TreeItemProjectorArea( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemProjectorArea( void );

  void SetSceneManager( size_t ix );

  virtual void ShowContextMenu( void );

  void DetachFromScenePanel( void );
  void AppendToScenePanel( void );
  
  protected:

private:

  enum {
    ID_Null = wxID_HIGHEST,
    MIAddColour, MIAddImage, MIAddVideo,
    MIRename, MIDelete
  };

  enum IdTreeItemType {
    IdMusic = 201, IdColour, IdImage, IdVideo, IdMidi, IdDMX
  };

  size_t m_ixSceneManager;

  void HandleAddColour( wxCommandEvent& event );
  void HandleAddImage( wxCommandEvent& event );
  void HandleAddVideo( wxCommandEvent& event );

  void HandleDelete( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar << boost::serialization::base_object<const TreeItemBase>(*this);
    ar & m_ixSceneManager;
    const vMembers_t::size_type n = m_vMembers.size();
    ar << n;
    for ( vMembers_t::const_iterator iter = m_vMembers.begin(); iter != m_vMembers.end(); ++iter ) {
      ar << ( iter->m_type );
      switch ( iter->m_type ) {
        case IdColour:
        {
          const TreeItemColour* pColour = dynamic_cast<TreeItemColour*>( iter->m_pTreeItemBase.get() );
          ar & *pColour;
        }
        break;
        case IdImage:
        {
          const TreeItemImage* pImage = dynamic_cast<TreeItemImage*>( iter->m_pTreeItemBase.get() );
          ar & *pImage;
        }
        break;
        case IdVideo:
        {
          const TreeItemVideo* pVideo = dynamic_cast<TreeItemVideo*>( iter->m_pTreeItemBase.get() );
          ar & *pVideo;
        }
        break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemBase>(*this);
    ar & m_ixSceneManager;
    assert( m_resources.vpSceneManager.size() > m_ixSceneManager );
    vMembers_t::size_type n;
    ar & n;
    for ( vMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdColour:
        {
          TreeItemColour* p = AddTreeItem<TreeItemColour,IdTreeItemType>( "Colour", IdColour );
          p->SetSceneManager( m_ixSceneManager );
          ar & *p;
        }
        break;
        case IdImage:
        {
          TreeItemImage* p = AddTreeItem<TreeItemImage,IdTreeItemType>( "Image", IdImage );
          p->SetSceneManager( m_ixSceneManager );
          ar & *p;
        }
        break;
        case IdVideo:
        {
          TreeItemVideo* p = AddTreeItem<TreeItemVideo,IdTreeItemType>( "Video", IdVideo );
          p->SetSceneManager( m_ixSceneManager );
          ar & *p;
        }
        break;
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemProjectorArea::TreeItemProjectorArea( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources ) 
  : TreeItemSceneElementBase(id, resources) {
}

TreeItemProjectorArea::~TreeItemProjectorArea(void) {
}

void TreeItemProjectorArea::AppendToScenePanel( void ) {
  // recursive process of scene elements
  for ( vMembers_t::iterator iter = m_vMembers.begin(); m_vMembers.end() != iter; ++iter ) {
    dynamic_cast<TreeItemSceneElementBase*>( iter->m_pTreeItemBase.get() )->AppendToScenePanel();
  }
}

void TreeItemProjectorArea::DetachFromScenePanel( void ) {
  // recursive process of scene elements
  for ( vMembers_t::iterator iter = m_vMembers.begin(); m_vMembers.end() != iter; ++iter ) {
    dynamic_cast<TreeItemSceneElementBase*>( iter->m_pTreeItemBase.get() )->DetachFromScenePanel();
  }
}

void TreeItemProjectorArea::SetSceneManager ( size_t ix ) {
  assert( m_resources.vpSceneManager.size() > ix );
  m_ixSceneManager = ix;
}

void TreeItemProjectorArea::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddColour, "&Colour" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemProjectorArea::HandleAddColour, this, MIAddColour );
  pMenu->Append( MIAddImage, "&Image" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemProjectorArea::HandleAddImage, this, MIAddImage );
  pMenu->Append( MIAddVideo, "&Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemProjectorArea::HandleAddVideo, this, MIAddVideo );
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemProjectorArea::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemProjectorArea::HandleDelete, this, MIDelete );
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemProjectorArea::HandleAddColour( wxCommandEvent& event ) {
  TreeItemColour* p = AddTreeItem<TreeItemColour,IdTreeItemType>( "Colour", IdColour );
  p->SetSceneManager( m_ixSceneManager );
  p->Rename();
}

void TreeItemProjectorArea::HandleAddImage( wxCommandEvent& event ) {
  TreeItemImage* p = AddTreeItem<TreeItemImage,IdTreeItemType>( "Image", IdImage );
  p->SetSceneManager( m_ixSceneManager );
  p->Rename();
}

void TreeItemProjectorArea::HandleAddVideo( wxCommandEvent& event ) {
  TreeItemVideo* p = AddTreeItem<TreeItemVideo,IdTreeItemType>( "Video", IdVideo );
  p->SetSceneManager( m_ixSceneManager );
  p->Rename();
}

void TreeItemProjectorArea::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemProjectorArea Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}

// ================

class TreeItemScene: public TreeItemBase {
  friend class boost::serialization::access;
public:

  TreeItemScene( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources );
  virtual ~TreeItemScene( void );

  virtual void ShowContextMenu( void );

  virtual void SetSelected( CommonGuiElements& ); // rebuild the panelScene
  virtual void RemoveSelected( CommonGuiElements& ) {}  // not using yet
  
protected:

private:

  enum {
    ID_Null = wxID_HIGHEST,
    MIAddSubGroup, MIAddMusic, MIAddColour, MIAddImage, MIAddVideo, MIAddMidi, MIAddDMX, MIAddProjectorAreas, MIAddMonoAudio,
    MIRename, MIDelete
  };

  enum IdTreeItemType {
    IdMusic = 201, IdColour, IdImage, IdVideo, IdMidi, IdDMX, IdProjectorArea, IdMonoAudio,
  };

  bool m_bAddedProjectorAreas;

  void HandleAddMusic( wxCommandEvent& event );
  TreeItemMusic* AddMusic( void );

  void HandleAddMonoAudio( wxCommandEvent& event );

  void HandleAddMidi( wxCommandEvent& event );
  void HandleAddDMX( wxCommandEvent& event );
  void HandleAddProjectorAreas( wxCommandEvent& event );

  void HandleDelete( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar << boost::serialization::base_object<const TreeItemBase>(*this);
    const vMembers_t::size_type n = m_vMembers.size();
    ar << n;
    for ( vMembers_t::const_iterator iter = m_vMembers.begin(); iter != m_vMembers.end(); ++iter ) {
      ar << ( iter->m_type );
      switch ( iter->m_type ) {
        case IdMusic:
        {
          const TreeItemMusic* pMusic = dynamic_cast<TreeItemMusic*>( iter->m_pTreeItemBase.get() );
          ar & *pMusic;
        }
        break;
        case IdMonoAudio:
        {
          const TreeItemMonoAudio* pMonoAudio = dynamic_cast<TreeItemMonoAudio*>( iter->m_pTreeItemBase.get() );
          ar & *pMonoAudio;
        }
        break;
        case IdMidi:
        {
          const TreeItemMidi* pMidi = dynamic_cast<TreeItemMidi*>( iter->m_pTreeItemBase.get() );
          ar & *pMidi;
        }
        break;
        case IdDMX:
        {
          const TreeItemDMX* pDMX = dynamic_cast<TreeItemDMX*>( iter->m_pTreeItemBase.get() );
          ar & *pDMX;
        }
        break;
        case IdProjectorArea:
        {
          const TreeItemProjectorArea* pProjectorArea = dynamic_cast<TreeItemProjectorArea*>( iter->m_pTreeItemBase.get() );
          ar & *pProjectorArea;
        }
        break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemBase>(*this);
    vMembers_t::size_type n;
    ar & n;
    for ( vMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdMusic:
        {
          TreeItemMusic* pMusic = AddTreeItem<TreeItemMusic,IdTreeItemType>( "Music", IdMusic );
          ar & *pMusic;
          pMusic->SelectMusic();
        }
        break;
        case IdMonoAudio:
        {
          TreeItemMonoAudio* pMonoAudio = AddTreeItem<TreeItemMonoAudio,IdTreeItemType>( "Mono Audio", IdMonoAudio );
          ar & *pMonoAudio;
          pMonoAudio->SelectAudio();
        }
          break;
        case IdMidi:
        {
          TreeItemMidi* pMidi = AddTreeItem<TreeItemMidi,IdTreeItemType>( "Midi", IdMidi );
          ar & *pMidi;
        }
        break;
        case IdDMX:
        {
          TreeItemDMX* pDMX = AddTreeItem<TreeItemDMX,IdTreeItemType>( "DMX", IdDMX );
          ar & *pDMX;
        }
        break;
        case IdProjectorArea:
        {
          // will need to abort if the stored count doesn't match the active count?
          // future todo:  a dynamic creation of displays, so not all are used for projection
          TreeItemProjectorArea* pProjectorArea = AddTreeItem<TreeItemProjectorArea,IdTreeItemType>( "Display", IdProjectorArea );
          ar & *pProjectorArea;  // needs to load appropriate SceneManager itself
        }
        break;
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

TreeItemScene::TreeItemScene( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources ) 
  : TreeItemBase(id, resources), m_bAddedProjectorAreas( false ) {
}

TreeItemScene::~TreeItemScene(void) {
}

void TreeItemScene::SetSelected( CommonGuiElements& ) {
  if ( this->m_id != m_resources.currentScene ) {
    for ( vMembers_t::iterator iter = m_vMembers.begin(); m_vMembers.end() != iter; ++iter ) {
      dynamic_cast<TreeItemSceneElementBase*>( iter->m_pTreeItemBase.get() )->DetachFromScenePanel();
    }
    m_resources.tree.m_signalClearScenePanel();
  }
  m_resources.currentScene = this->m_id;
  for ( vMembers_t::iterator iter = m_vMembers.begin(); m_vMembers.end() != iter; ++iter ) {
    dynamic_cast<TreeItemSceneElementBase*>( iter->m_pTreeItemBase.get() )->AppendToScenePanel();
  }
}

void TreeItemScene::ShowContextMenu( void ) {  // need scene elements instead once I get this figured out
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddMusic, "&Music" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleAddMusic, this, MIAddMusic );
  pMenu->Append( MIAddMonoAudio, "&Mono Audio" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleAddMonoAudio, this, MIAddMonoAudio );
  pMenu->Append( MIAddMidi, "M&idi" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleAddMidi, this, MIAddMidi );
  pMenu->Append( MIAddDMX, "&DMX" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleAddDMX, this, MIAddDMX );
  if (!m_bAddedProjectorAreas) {
    pMenu->Append( MIAddProjectorAreas, "&Projectors" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleAddProjectorAreas, this, MIAddProjectorAreas );
  }
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScene::HandleDelete, this, MIDelete );
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemScene::HandleAddProjectorAreas( wxCommandEvent& event ) {
  if (!m_bAddedProjectorAreas) {
    for ( size_t ix = 0; ix < m_resources.vpSceneManager.size(); ++ix ) {
      std::string sDesc( "Display " );
      sDesc += boost::lexical_cast<std::string>( ix );
      TreeItemProjectorArea* p = AddTreeItem<TreeItemProjectorArea,IdTreeItemType>( sDesc, IdProjectorArea );
      p->SetSceneManager( ix );
    }
    m_bAddedProjectorAreas = true;
  }
}

void TreeItemScene::HandleAddMusic( wxCommandEvent& event ) {
  TreeItemMusic* p = AddTreeItem<TreeItemMusic,IdTreeItemType>( "Music", IdMusic );
  p->Rename();
}

void TreeItemScene::HandleAddMonoAudio( wxCommandEvent& event ) {
  TreeItemMonoAudio* p = AddTreeItem<TreeItemMonoAudio,IdTreeItemType>( "Mono Audio", IdMonoAudio );
  p->Rename();
}

void TreeItemScene::HandleAddMidi( wxCommandEvent& event ) {
  TreeItemMidi* p = AddTreeItem<TreeItemMidi,IdTreeItemType>( "Midi", IdMidi );
  p->Rename();
}

void TreeItemScene::HandleAddDMX( wxCommandEvent& event ) {
  TreeItemDMX* p = AddTreeItem<TreeItemDMX,IdTreeItemType>( "DMX", IdDMX );
  p->Rename();
}

void TreeItemScene::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemScene Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}

// ================

class TreeItemGroup: public TreeItemBase {
  friend class boost::serialization::access;
public:

  // deals with organizing groups of branches, eg:  master - act - scene
  TreeItemGroup( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources ): TreeItemBase( id, resources ) {}
  virtual ~TreeItemGroup( void ) {};

  virtual void ShowContextMenu( void );

protected:

  enum IdTreeItemType {
    IdGroup = 201, IdScene
  };

  TreeItemGroup* AddGroup( void );  // for TreeItemRoot

private:

  enum {
    ID_Null = wxID_HIGHEST,
    MIAddSubGroup, MIAddMusic, MIAddScene,
    MIDelete, MIRename
  };

  void HandleAddGroup( wxCommandEvent& event );
  void HandleAddMusic( wxCommandEvent& event );
  TreeItemMusic* AddMusic( void );

  void HandleAddScene( wxCommandEvent& event );
  TreeItemScene* AddScene( void );

  void HandleDelete( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar << boost::serialization::base_object<const TreeItemBase>(*this);
    const vMembers_t::size_type n = m_vMembers.size();
    ar << n;
    for ( vMembers_t::const_iterator iter = m_vMembers.begin(); iter != m_vMembers.end(); ++iter ) {
      ar << ( iter->m_type );
      switch ( iter->m_type ) {
        case IdGroup:
        {
          const TreeItemGroup* pGroup = dynamic_cast<TreeItemGroup*>( iter->m_pTreeItemBase.get() );
          ar & *pGroup;
        }
        break;
        case IdScene:
        {
          const TreeItemScene* pScene = dynamic_cast<TreeItemScene*>( iter->m_pTreeItemBase.get() );
          ar & *pScene;
        }
        break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemBase>(*this);
    vMembers_t::size_type n;
    ar & n;
    for ( vMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdGroup:
        {
          TreeItemGroup* pGroup = AddTreeItem<TreeItemGroup,IdTreeItemType>( "Group", IdGroup );
          ar & *pGroup;
        }
        break;
        case IdScene:
        {
          TreeItemScene* pScene = AddTreeItem<TreeItemScene,IdTreeItemType>( "Scene", IdScene );
          ar & *pScene;
          //pScene->SelectMusic();
        }
        break;
      }
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

void TreeItemGroup::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddScene, "Add &Scene" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleAddScene, this, MIAddScene );
  pMenu->Append( MIAddSubGroup, "Add Sub &Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleAddGroup, this, MIAddSubGroup );
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleDelete, this, MIDelete );
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemGroup::HandleAddGroup( wxCommandEvent& event ) { 
  //TreeItemGroup* p = AddGroup();
  TreeItemGroup* p = AddTreeItem<TreeItemGroup>( "Group", IdGroup );
  p->Rename();
}

void TreeItemGroup::HandleAddScene( wxCommandEvent& event ) {
  //TreeItemScene* p = AddScene();
  TreeItemScene* p = AddTreeItem<TreeItemScene>( "Scene", IdScene );
  p->Rename();
}

void TreeItemGroup::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemGroup Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}

// ================

class TreeItemRoot: public TreeItemGroup {
  friend class boost::serialization::access;
public:

  TreeItemRoot( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources ): TreeItemGroup( id, resources ) {}
  virtual ~TreeItemRoot( void ) {}

  virtual void ShowContextMenu( void );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddScreenFrame, MIAddGroup
  };

  void HandleAddScreenFrame( wxCommandEvent& event ) {  // for remote displays, will use wizard dialog
    std::cout << "Add Remote Screen Frame (un-implemented)" << std::endl; 
  }
  void HandleAddGroup( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemGroup>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemGroup>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

void TreeItemRoot::ShowContextMenu( void ) {
  //wxMenu* pMenu = new wxMenu( "Surfaces");
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddGroup, "&Add Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemRoot::HandleAddGroup, this, MIAddGroup );
  pMenu->Append( MIAddScreenFrame, "&Add Screen Frame" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemRoot::HandleAddScreenFrame, this, MIAddScreenFrame );
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemRoot::HandleAddGroup( wxCommandEvent& event ) {
  TreeItemGroup* pGroup = TreeItemGroup::AddTreeItem<TreeItemGroup>( "Group", TreeItemGroup::IdGroup );
  pGroup->Rename();
}

// ================

class TreeItemPlaceHolder: public TreeItemBase {
public:
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  typedef Outline::pOutline_t pOutline_t;
  //typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  typedef SceneManager::pSceneManager_t pSceneManager_t;
  
  TreeItemPlaceHolder( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, 
//              pPhysicalDisplay_t pScreenFrame, pOutline_t pOutline, pSceneManager_t pSceneManager );
                pOutline_t pOutline, pSceneManager_t pSceneManager );
  ~TreeItemPlaceHolder( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddPicture, MIAddVideo, MIAddGrid, MIDelete
  };
  
  typedef SceneManager::key_t key_t;
  //typedef SceneManager::pSceneElementOpenGL_t pSceneElementOpenGL_t;
  typedef boost::signals2::connection connection;
  
  // does this do anything?
  class SceneElementInfo {
  public:
    //SceneElementInfo( void ): m_mat4Transform( 1.0f ) {};
    SceneElementInfo( void ) {};
    //SceneElementInfo( connection connectTransformSupplier, glm::mat4 mat4Transform )
    //  : m_connectTransformSupplier( connectTransformSupplier ), m_mat4Transform( mat4Transform ) {}
    ~SceneElementInfo( void ) {
      m_connectTransformSupplier.disconnect();
    }
    //void HandleUpdateTransform( const glm::mat4& matrix ) {
    //  m_mat4Transform = matrix;
    //}
    void Connection( connection c ) { m_connectTransformSupplier = c; }
    void Disconnect( void ) { m_connectTransformSupplier.disconnect(); }
    //glm::mat4& GetTransform( void ) { return m_mat4Transform; }
  private:
    connection m_connectTransformSupplier;
    //glm::mat4 m_mat4Transform;  // is this actually used?
  };
  
  typedef boost::shared_ptr<SceneElementInfo> pSceneElementInfo_t;
  typedef std::map<wxTreeItemId,pSceneElementInfo_t> mapSceneElementInfo_t;
  
  bool m_bHasGrid;  // need a signal from grid when to clear, should only have one grid assigned
  // but allow multiple for now, but each is going to overwrite the supplied outline
  // unless the outline is read only, and used only as a starting point
  // will allow multiple objects on the canvas, so may not need m_bHasGrid
  
  //pPhysicalDisplay_t m_pPhysicalDisplay;
  pOutline_t m_pOutline;
  pSceneManager_t m_pSceneManager;
  
  //mapSceneElementInfo_t m_mapSceneElementInfo;
  
  void SetSelected( CommonGuiElements& );
  void RemoveSelected( CommonGuiElements& );
  void DeletingChild( wxTreeItemId );
  
  void HandleAddPicture( wxCommandEvent& event );
  void HandleAddVideo( wxCommandEvent& event );
  void HandleAddGrid( wxCommandEvent& event );
  void HandleDelete( wxCommandEvent& event );
  
};

TreeItemPlaceHolder::TreeItemPlaceHolder( 
  //wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay, pOutline_t pOutline, pSceneManager_t pSceneManager )
  wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pOutline_t pOutline, pSceneManager_t pSceneManager )
: 
  //TreeItemBase( id, resources ), m_pPhysicalDisplay( pPhysicalDisplay ), 
  TreeItemBase( id, resources ), 
  m_pOutline( pOutline ), m_pSceneManager( pSceneManager ), m_bHasGrid( false ) {
  //m_mat4Transform = glm::mat4( 1.0f );  // identity matrix to start
}

TreeItemPlaceHolder::~TreeItemPlaceHolder( void ) {
  //RemoveSelected( m_guiElements );
}

void TreeItemPlaceHolder::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIAddGrid, "Add &Grid" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddGrid, this, MIAddGrid );
  
  pMenu->Append( MIAddPicture, "Add &Image" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddPicture, this, MIAddPicture );
  
  pMenu->Append( MIAddVideo, "Add &Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddVideo, this, MIAddVideo );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleDelete, this, MIDelete );
  
  m_resources.tree.PopupMenu( pMenu );
}

void TreeItemPlaceHolder::SetSelected( CommonGuiElements& ) {
  //std::cout << "Tree Item Canvas Selected" << std::endl;
  //m_pPhysicalDisplay->GetFrame()->SetOutline( m_pOutline );
  //m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPlaceHolder::RemoveSelected( CommonGuiElements& ) {  // should happen before delete
  //pOutline_t pOutline;
  //m_pPhysicalDisplay->GetFrame()->SetOutline( pOutline );
  //m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPlaceHolder::DeletingChild( wxTreeItemId id ) {
  //mapSceneElementInfo_t::iterator iter = m_mapSceneElementInfo.find( id );
  //assert( m_mapSceneElementInfo.end() != iter );
  //iter->second->Disconnect();
  //m_mapSceneElementInfo.erase( iter );
}

void TreeItemPlaceHolder::HandleAddGrid( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_resources.tree.AppendItem( m_id, "Grid" );
  m_resources.tree.EnsureVisible( id );
  
  //TreeItemGrid* pGrid = new TreeItemGrid( id, m_resources, m_pPhysicalDisplay, m_pSceneManager );
  //TreeItemGrid* pGrid = new TreeItemGrid( id, m_resources, m_pSceneManager );
//  TreeItemGrid* pGrid = new TreeItemGrid( id, m_resources );
//  pTreeItemBase_t pTreeItemBase( pGrid );
//  m_resources.tree.Add( id, pTreeItemBase );
  
  //pSceneElementInfo_t pInfo( new SceneElementInfo );
  //m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  //namespace args = boost::phoenix::arg_names;
  //pInfo->Connection( pGrid->ConnectTransformUpdated( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) ) );
  //pGrid->GetTransformMatrix( pInfo->GetTransform() );
  
}

void TreeItemPlaceHolder::HandleAddPicture( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_resources.tree.AppendItem( m_id, "Image" );
  m_resources.tree.EnsureVisible( id );
  
  //TreeItemImage* pImage = new TreeItemImage( id, m_resources, m_pPhysicalDisplay, m_pSceneManager );
  //TreeItemImage* pImage = new TreeItemImage( id, m_resources, m_pSceneManager );
// need an index here
//  TreeItemImage* pImage = new TreeItemImage( id, m_resources );
//  pTreeItemBase_t pTreeItemBase( pImage );
//  m_resources.tree.Add( id, pTreeItemBase );
  
  //pSceneElementInfo_t pInfo( new SceneElementInfo );
  //m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  //namespace args = boost::phoenix::arg_names;
  //pInfo->Connection( pImage->ConnectTransformUpdated( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) ) );
  //pImage->GetTransformMatrix( pInfo->GetTransform() );
  
}

void TreeItemPlaceHolder::HandleAddVideo( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_resources.tree.AppendItem( m_id, "Video" );
  m_resources.tree.EnsureVisible( id );
  
  //TreeItemVideo* pVideo = new TreeItemVideo( id, m_resources, m_pPhysicalDisplay, m_pSceneManager );
  //TreeItemVideo* pVideo = new TreeItemVideo( id, m_resources, m_pSceneManager );
//  TreeItemVideo* pVideo = new TreeItemVideo( id, m_resources );
//  pTreeItemBase_t pTreeItemBase( pVideo );
//  m_resources.tree.Add( id, pTreeItemBase );
  
  //pSceneElementInfo_t pInfo( new SceneElementInfo );
  //m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  //namespace args = boost::phoenix::arg_names;
  //pInfo->Connection( pVideo->ConnectTransformUpdated( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) ) );
  //pVideo->GetTransformMatrix( pInfo->GetTransform() );
  
}

void TreeItemPlaceHolder::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemPlaceHolder Delete" << std::endl;
  m_resources.tree.Delete( this->m_id );
}


// ================

class TreeItemPhysicalDisplay: public TreeItemBase {
public:
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  typedef SceneManager::pSceneManager_t pSceneManager_t;
  
  //TreeItemPhysicalDisplay( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pScreenFrame );
  TreeItemPhysicalDisplay( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager );
  ~TreeItemPhysicalDisplay( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddPlaceHolder,
    MIAddOutline, 
//    MISelectVideo
  };
  
  typedef Outline::pOutline_t pOutline_t;
  //typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  
  pSceneManager_t m_pSceneManager;
  
  //pPhysicalDisplay_t m_pPhysicalDisplay;
  
  void HandleAddOutline( wxCommandEvent& event );
  void HandleAddPlaceHolder( wxCommandEvent& event );
  
  //void HandleLoadVideo( wxCommandEvent& event );
  
};

//TreeItemPhysicalDisplay::TreeItemPhysicalDisplay( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pPhysicalDisplay_t pPhysicalDisplay )
TreeItemPhysicalDisplay::TreeItemPhysicalDisplay( wxTreeItemId id, TreeDisplayManager::TreeItemResources& resources, pSceneManager_t pSceneManager )
: 
  TreeItemBase( id, resources ), m_pSceneManager( pSceneManager ) {
  
  // add a right click pop up to add displayable objects and surfaces
  // which are then serialized for session persistence
  // use text or enum keys to register objects, for subsequent re-creation

  // obsolete now, SceneManager created in caller
  //int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };  // WX_GL_CORE_PROFILE deprecated I think
  //m_pSceneManager.reset( new SceneManager( m_pPhysicalDisplay->GetFrame(), argsCanvas ) );
  //wxRect rect( 10, 10, 10, 10 );
  //rect = m_pPhysicalDisplay->GetFrame()->GetClientRect();
  //m_pSceneManager->SetSize( rect.GetSize() );
  //m_pSceneManager->Move( rect.GetTopLeft() );
}

TreeItemPhysicalDisplay::~TreeItemPhysicalDisplay( void ) {
}

void TreeItemPhysicalDisplay::HandleAddOutline(  wxCommandEvent& event  ) {  // for remote displays, will use wizard dialog
  std::cout << "Add Outline" << std::endl;  
  pOutline_t m_pOutline( new Outline( wxRect( 300, 300, 600, 600 ) ) );
  // no longer visible, will need to draw in opengl instead
  //m_pPhysicalDisplay->GetFrame()->SetOutline( m_pOutline );
  //m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPhysicalDisplay::HandleAddPlaceHolder( wxCommandEvent& event ) {
  std::cout << "Add PlaceHolder" << std::endl;  
  // various stages:  
  //   0) popup to get description
  //   1) tree item added - done
  //   2) outline added - done
  //   3) outline changeable - done
  //   4) handle selection event to turn outline back on for resizing events - done
  //   5) add menu items to add pictures or movies - done 
  //   6) create the canvas? - done
  //   7) handle events from outline to adjust canvas - obsolete
  
  wxTreeItemId id = m_resources.tree.AppendItem( m_id, "PlaceHolder" );
  m_resources.tree.EnsureVisible( id );
  
  pOutline_t pOutline( new Outline( wxRect( 300, 300, 600, 600 ), true, false ) );  // instead, use some ratio of the main window

  //pTreeItemBase_t pTreeItemBase( new TreeItemPlaceHolder( id, m_resources, m_pPhysicalDisplay, pOutline, m_pSceneManager ) );
  pTreeItemBase_t pTreeItemBase( new TreeItemPlaceHolder( id, m_resources, pOutline, m_pSceneManager ) );
  m_resources.tree.Add( id, pTreeItemBase );
}

void TreeItemPhysicalDisplay::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIAddPlaceHolder, "Add PlaceHolder" ); // need to have picture or video ready with dimensions to scale properly
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPhysicalDisplay::HandleAddPlaceHolder, this, MIAddPlaceHolder );
  
  pMenu->AppendSeparator();

  pMenu->Append( MIAddOutline, "&Add Outline" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPhysicalDisplay::HandleAddOutline, this, MIAddOutline );

  m_resources.tree.PopupMenu( pMenu );
}

// ====================================
    
TreeDisplayManager::TreeDisplayManager(): m_resources( *this ) {
  Init();
}

TreeDisplayManager::TreeDisplayManager( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ): m_resources( *this ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool TreeDisplayManager::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxTreeCtrl::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

TreeDisplayManager::~TreeDisplayManager() {
  // m_resources.pAudio.reset();  // implied now
}

void TreeDisplayManager::Init() {
  //m_resources.pAudio = 0; // implied now
  m_resources.sCurrentPath = std::string( ::wxGetCwd() );
}

void TreeDisplayManager::CreateControls() {    
  
  TreeDisplayManager* item = this;
  
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_MENU, &TreeDisplayManager::HandleContextMenu, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGED, &TreeDisplayManager::HandleSelectionChanged, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGING, &TreeDisplayManager::HandleSelectionChanging, this );
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_ACTIVATED, &TreeDisplayManager::HandleItemActivated, this );
  wxTreeCtrl::Bind( wxEVT_TREE_DELETE_ITEM, &TreeDisplayManager::HandleItemDeleted, this );
  
  wxTreeItemId id = wxTreeCtrl::AddRoot( "Projections" );
  m_pTreeItemRoot.reset( new TreeItemRoot( id, m_resources ) );
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), m_pTreeItemRoot ) );
  
}

void TreeDisplayManager::Append( pPhysicalDisplay_t pPhysicalDisplay ) {

  // 2015/06/09 migrate to using as a resource instead
  m_resources.vpPhysicalDisplay.push_back( pPhysicalDisplay );

  // from the creation of a TreeItemPhysicalDisplay - with TreeItemPhysicalDisplay being deprecated or changed
  typedef SceneManager::pSceneManager_t pSceneManager_t;
  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };  // WX_GL_CORE_PROFILE deprecated I think
  pSceneManager_t pSceneManager( new SceneManager( pPhysicalDisplay->GetFrame(), argsCanvas ) );
  wxRect rect( 10, 10, 10, 10 );
  rect = pPhysicalDisplay->GetFrame()->GetClientRect();
  pSceneManager->SetSize( rect.GetSize() );
  pSceneManager->Move( rect.GetTopLeft() );
  m_resources.vpSceneManager.push_back( pSceneManager );

  // get rid of adding to tree once everything is handled by scene lists
  wxTreeItemId idRoot = wxTreeCtrl::GetRootItem();
  std::string sId = boost::lexical_cast<std::string>( pPhysicalDisplay->GetId() );
  wxTreeItemId id = wxTreeCtrl::AppendItem( idRoot, "Frame " + sId );
  EnsureVisible( id );

  //pTreeItemBase_t pTreeItemBase( new TreeItemPhysicalDisplay( id, m_resources, pPhysicalDisplay ) );
  pTreeItemBase_t pTreeItemBase( new TreeItemPhysicalDisplay( id, m_resources, pSceneManager ) );
  Add( id, pTreeItemBase );

}

void TreeDisplayManager::Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase ) {
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), pTreeItemBase ) );
}

void TreeDisplayManager::Delete( wxTreeItemId id ) {
  if ( 0 == GetChildrenCount( id ) ) {
    
    // need to detect that it is deleting itself, and that it is currently selected
    if ( m_idOld.IsOk() ) {
      if ( m_idOld = id ) {
        m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
      }
    }
    
    wxTreeItemId idParent = wxTreeCtrl::GetItemParent( id );
    assert( idParent.IsOk() );
    mapDecoder_t::iterator iterParent = m_mapDecoder.find( idParent.GetID() );
    assert( m_mapDecoder.end() != iterParent );
    iterParent->second->DeletingChild( id );
    
    mapDecoder_t::iterator iterChild = m_mapDecoder.find( id.GetID() );
    assert( m_mapDecoder.end() != iterChild );
    
    wxTreeCtrl::Delete( id );
    m_idOld.Unset();
    m_mapDecoder.erase( iterChild );
  }
  else {
    std::cout << "item has children" << std::endl;
  }
}

void TreeDisplayManager::SetStaticTextInfo( wxStaticText* pstInfo ) {
  m_guiElements.pstInfo = pstInfo;
}

void TreeDisplayManager::SetSliders( wxSlider* sliderSeek, wxSlider* sliderZ, wxSlider* sliderVolume, wxSlider* sliderFader, wxSlider* sliderMaster ) {
  m_guiElements.pSliderSeek = sliderSeek;
  m_guiElements.pSliderZ = sliderZ;
  m_guiElements.pSliderVolume = sliderVolume;
  m_guiElements.pSliderFader = sliderFader;
  m_guiElements.pSliderMaster = sliderMaster;
}

void TreeDisplayManager::HandleContextMenu( wxTreeEvent& event ) {
  m_mapDecoder[ event.GetItem().GetID() ]->ShowContextMenu();
}

void TreeDisplayManager::HandleSelectionChanged( wxTreeEvent& event ) {
//  std::cout << "HandleSelectionChanged " << event.GetItem().GetID() << std::endl;
  m_idOld = event.GetItem();
  m_mapDecoder[ m_idOld.GetID() ]->SetSelected( m_guiElements );
  
}

void TreeDisplayManager::RemoveSelectOld( void ) {
  //std::cout << "HandleSelectionChanging " << event.GetItem().GetID() << std::endl;
  if ( m_idOld.IsOk() ) m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
  m_idOld.Unset();
}

void TreeDisplayManager::HandleSelectionChanging( wxTreeEvent& event ) {
  RemoveSelectOld();
}

void TreeDisplayManager::HandleItemActivated( wxTreeEvent& event ) {
  //std::cout << "HandleItemActivated" << std::endl;
}

void TreeDisplayManager::HandleItemDeleted( wxTreeEvent& event ) {
  //std::cout << "HandleItemDeleted" << std::endl;
}

wxBitmap TreeDisplayManager::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeDisplayManager::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

void TreeDisplayManager::Add( pAudio_t pAudio_ ) { 
  m_resources.pAudio = pAudio_; // global variable
}

void TreeDisplayManager::Save( boost::archive::text_oarchive& oa ) {
  size_t n( m_resources.vpSceneManager.size() );
  oa & n;

  TreeItemBase* pBase = m_pTreeItemRoot.get();
  const TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  oa & *p;
}

void TreeDisplayManager::Load( boost::archive::text_iarchive& ia ) {
  size_t n;
  ia & n;
  assert( m_resources.vpSceneManager.size() == n );

  TreeItemBase* pBase = m_pTreeItemRoot.get();
  TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  ia & *p;
}

void TreeDisplayManager::HandleBtnEvent( BtnEvent event ) {
  switch ( event ) {
    case BtnPlay:
      m_resources.pAudio->Play();
      break;
    case BtnPause:
      std::cout << "no pause" << std::endl;
      break;
    case BtnStop:
      m_resources.pAudio->Stop();
      break;
    case BtnAbort:
      m_resources.pAudio->Abort();
      break;
  }
}
  

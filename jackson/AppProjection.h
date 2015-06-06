/* 
 * File:   jackson.h
 * Author: rpb
 *
 * Created on February 28, 2015, 7:38 PM
 */

#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>

#include "Audio.h"

#include "FrameMain.h"

class AppProjection : public wxApp {
  friend class boost::serialization::access;
public:

protected:
private:
  
  boost::signals2::connection m_MenuActionSave;
  boost::signals2::connection m_MenuActionLoad;
  
  std::string m_sMediaDirectory;
  std::string m_sWorkingDirectory;
  
  FrameMain *m_pFrameMain;
  PanelSurfaceSources* m_pSurfaceSources;
  
  typedef boost::shared_ptr<Audio> pAudio_t;
  pAudio_t m_pAudio;
  
  virtual bool OnInit( );
  virtual void OnClose( wxCloseEvent& event );
  virtual int OnExit( void );
  
  void HandleSave( void );
  void HandleLoad( void );
  
  //void HandleMouseMoved( wxMouseEvent& event ) { event.Skip(); } // EVT_MOTION
  //void HandleMouseWheel( wxMouseEvent& event ) { event.Skip(); } // EVT_MOUSEWHEEL
  //void HandleMouseLeftDown( wxMouseEvent& event ) { event.Skip(); } // EVT_LEFT_DOWN
  //void HandleMouseLeftUp( wxMouseEvent& event ) { event.Skip(); }  // EVT_LEFT_UP
  //void HandleMouseRightDown( wxMouseEvent& event ) { event.Skip(); } // EVT_RIGHT_DOWN
//  void HandleMouseLeaveWindow( wxMouseEvent& event ) { event.Skip(); } // EVT_LEAVE_WINDOW
  //void HandleKeyDown( wxKeyEvent& event ) { event.Skip(); } // EVT_KEY_DOWN
  //void HandleKeyUp( wxKeyEvent& event ) { event.Skip(); }  // EVT_KEY_UP
  void HandleKey( wxKeyEvent& event );
  
  void MediaFileStats( void );
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    if ( 0 < version ) {
      wxRect rect( m_pFrameMain->GetRect() );
      int x( rect.GetX() ), y( rect.GetY() ), width( rect.GetWidth() ), height( rect.GetHeight() );
      ar & x;
      ar & y;
      ar & width;
      ar & height;
    }
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    if ( 0 < version ) {
      int x, y, width, height;
      ar & x;
      ar & y;
      ar & width;
      ar & height;
      m_pFrameMain->SetSize( x, y, width, height );
    }
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(AppProjection, 1)


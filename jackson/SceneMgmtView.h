/* 
 * File:   SceneView.h
 * Author: rpb
 *
 * Created on June 15, 2015, 10:28 AM
 */

#pragma once

#include <string>
#include <map>

#include "SceneViewCommon.h"

#define SYMBOL_CONTROLSCENEVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLSCENEVIEW_TITLE _("SceneMgmtView")
#define SYMBOL_CONTROLSCENEVIEW_IDNAME ID_CONTROLSCENEVIEW
#define SYMBOL_CONTROLSCENEVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLSCENEVIEW_POSITION wxDefaultPosition

class SceneMgmtView: public SceneViewCommon {
  DECLARE_DYNAMIC_CLASS( SceneMgmtView )
public:
  
  typedef SceneViewCommon::pSceneViewCommon_t pSceneViewCommon_t;
  typedef boost::shared_ptr<SceneMgmtView> pSceneView_t;
  
  SceneMgmtView( );
  SceneMgmtView( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLSCENEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLSCENEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLSCENEVIEW_SIZE, 
          long style = SYMBOL_CONTROLSCENEVIEW_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLSCENEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLSCENEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLSCENEVIEW_SIZE, 
          long style = SYMBOL_CONTROLSCENEVIEW_STYLE );
  ~SceneMgmtView( );
  
  boost::posix_time::time_duration GetPixelWidth( void ) { return m_tdTimePixelMapping.tdPixelWidth; }

  // in: pixel offset, out: time begin, pixel width
  TimePixelMapping UpdateMouseZoomIn( const int x );
  TimePixelMapping UpdateMouseZoomOut( const int x );
  TimePixelMapping UpdateMouseShift( const int diff );
  
  void AddClip( const std::string& sClipName, pSceneViewCommon_t pSceneViewCommon );
  void RemoveClip( const std::string& sClipName );
  void RenameClip( const std::string& sOldName, const std::string& sNewName );
  
  void DrawTime( const std::string& sTime );
  
  virtual void UpdateInteractiveCursor( int x );

protected:
  
  virtual void UnDrawCursor( wxClientDC& dc, Cursor& cursor );

  virtual void HandlePaint( wxPaintEvent& event );
  //virtual void HandleEraseBackground( wxEraseEvent& event ) { SceneViewCommon::HandleEraseBackground( event ); }
  //virtual void HandleSize( wxSizeEvent& event ) { SceneViewCommon::HandleSize( event ); }

private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLSCENEVIEW
  };

  // how are the clips going to be identified?  creation timestamp?  sequence number?  renamable auto id?
  typedef std::map<std::string,pSceneViewCommon_t> mapClips_t;
  
  wxMenu* m_pContextMenu;
  
  mapClips_t m_mapClips;

  void DrawLegend( wxClientDC& dc );
  
  void Init();
  void CreateControls();

};

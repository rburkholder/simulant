/* 
 * File:   SceneMgmtView.h
 * Author: rpb
 *
 * Created on June 15, 2015, 10:28 AM
 */

#pragma once

// A SceneView contains a number of specific views:  keyframe markers, waveforms, video stills, images
// each view will then contain a number of clips

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
  
  // can't really do either of these, as the underlying wx system handles the destroy function
  //typedef SceneViewCommon::pSceneViewCommon_t pSceneViewCommon_t;
  //typedef boost::shared_ptr<SceneMgmtView> pSceneView_t;
  
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

  // who holds the time offsets?  the manager, the view, or the clips?
  void AddView( const std::string& sViewName, SceneViewCommon* pSceneViewCommon );
  void DeleteView( const std::string& sViewName );
  void RenameView( const std::string& sOldName, const std::string& sNewName ); // is case sensitive
  
  void ClearViews( void );
  
  void DrawTime( const std::string& sTime );
  
  virtual void UpdateInteractiveCursor( int x, bool bTurnOn = true );

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

  struct View {
    SceneViewCommon* pSceneViewCommon;
    boost::signals2::connection connMouseMotion;
    boost::signals2::connection connMouseShift;
    boost::signals2::connection connZoomIn;
    boost::signals2::connection connZoomOut;
    boost::signals2::connection connMouseDeparts;
    View( void ): pSceneViewCommon( 0 ) {};
  };
  
  // how are the views going to be identified?  creation timestamp?  sequence number?  rename-able auto id?
  typedef std::map<std::string,View> mapViews_t;
  
  View m_view;  // holds our own connections;
  
  mapViews_t m_mapViews;

  wxMenu* m_pContextMenu;
  
  void ConnectSignals( View& view, SceneViewCommon* pview );
  void DisconnectSignals( View& view );
  
  void DrawLegend( wxClientDC& dc );
  
  // process self-update, then call each of the sub-views for their self-update
  void HandleMouseMotion( int x, int diff );
  void HandleMouseShift( int diff );
  void HandleZoomIn( wxCoord x );
  void HandleZoomOut( wxCoord x );
  void HandleMouseDeparts( wxCoord x );

  // in: pixel offset, out: time begin, pixel width
  TimePixelMapping UpdateMouseZoomIn( const int x );
  TimePixelMapping UpdateMouseZoomOut( const int x );
  TimePixelMapping UpdateMouseShift( const int diff );
  
  void Init();
  void CreateControls();

};

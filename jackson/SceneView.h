/* 
 * File:   SceneView.h
 * Author: rpb
 *
 * Created on June 15, 2015, 10:28 AM
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/signals2.hpp>

#include "SceneViewCommon.h"

#define SYMBOL_CONTROLSCENEVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLSCENEVIEW_TITLE _("SceneView")
#define SYMBOL_CONTROLSCENEVIEW_IDNAME ID_CONTROLSCENEVIEW
#define SYMBOL_CONTROLSCENEVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLSCENEVIEW_POSITION wxDefaultPosition

class SceneView: public SceneViewCommon {
  DECLARE_DYNAMIC_CLASS( SceneView )
public:
  SceneView( );
  SceneView( 
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
  ~SceneView( );

  void DrawTime( const std::string& sTime );
  
protected:

  //virtual void HandlePaint( wxPaintEvent& event ) { SceneViewCommon::HandlePaint( event ); }
  virtual void HandlePaint( wxPaintEvent& event );
  virtual void HandleEraseBackground( wxEraseEvent& event ) { SceneViewCommon::HandleEraseBackground( event ); }
  virtual void HandleSize( wxSizeEvent& event ) { SceneViewCommon::HandleSize( event ); }

private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLSCENEVIEW
  };

  boost::posix_time::time_duration m_tdWinStart;
  boost::posix_time::time_duration m_tdPixelWidth;
  
  wxMenu* m_pContextMenu;

  void ZoomIn( int x );
  void ZoomOut( int x );
  void Shift( int x ); // number of pixels

  void DrawDecorations( wxClientDC& dc );
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
  
};

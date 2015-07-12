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
  
  struct TimePixelMapping {
    boost::posix_time::time_duration tdWinStart;  // first pixel starts at this time, 00:00;00 is minimum
    boost::posix_time::time_duration tdPixelWidth;  // each pixel counts for this duration
    TimePixelMapping( void ) {
      static boost::posix_time::time_duration tdOneSecond = boost::posix_time::seconds( 1 );
      tdWinStart = boost::posix_time::time_duration( 0, 0, 0 );
      tdPixelWidth = tdOneSecond; // to start, one pixel is one second of waveform or video
      //m_tdPixelWidth = tdOneSecond / 100;  // 100 frames per second, one frame per pixel to start
    }
  };

  // pixel offset in, time begin, offset out
  void UpdateMouseZoomIn( const int x, TimePixelMapping& );
  void UpdateMouseZoomOut( const int x, TimePixelMapping& );
  void UpdateMouseShift( const int diff, TimePixelMapping& );
  
  void DrawTime( const std::string& sTime );
  
protected:


  virtual void UnDrawCursor( wxClientDC& dc, Cursor& cursor );

  //virtual void HandlePaint( wxPaintEvent& event ) { SceneViewCommon::HandlePaint( event ); }
  virtual void HandlePaint( wxPaintEvent& event );
  virtual void HandleEraseBackground( wxEraseEvent& event ) { SceneViewCommon::HandleEraseBackground( event ); }
  virtual void HandleSize( wxSizeEvent& event ) { SceneViewCommon::HandleSize( event ); }

private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLSCENEVIEW
  };

  // these two will be retired
  //boost::posix_time::time_duration m_tdWinStart;
  //boost::posix_time::time_duration m_tdPixelWidth;
  TimePixelMapping m_tdTimePixelMapping;
  
  wxMenu* m_pContextMenu;

  void DrawLegend( wxClientDC& dc );
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
  
};

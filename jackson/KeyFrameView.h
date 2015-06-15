/* 
 * File:   KeyFrameView.h
 * Author: rpb
 *
 * Created on June 8, 2015, 10:37 PM
 */

#pragma once

#include <boost/signals2.hpp>

#include "SceneViewCommon.h"

#define SYMBOL_CONTROLKEYFRAMEVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLKEYFRAMEVIEW_TITLE _("KeyFrameView")
#define SYMBOL_CONTROLKEYFRAMEVIEW_IDNAME ID_CONTROLKEYFRAMEVIEW
#define SYMBOL_CONTROLKEYFRAMEVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLKEYFRAMEVIEW_POSITION wxDefaultPosition

class KeyFrameView: public SceneViewCommon  {
  DECLARE_DYNAMIC_CLASS( KeyFrameView )
public:
  
  KeyFrameView( );
  KeyFrameView( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLKEYFRAMEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLKEYFRAMEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLKEYFRAMEVIEW_SIZE, 
          long style = SYMBOL_CONTROLKEYFRAMEVIEW_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLKEYFRAMEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLKEYFRAMEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLKEYFRAMEVIEW_SIZE, 
          long style = SYMBOL_CONTROLKEYFRAMEVIEW_STYLE );
  virtual ~KeyFrameView( );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLKEYFRAMEVIEW

  };
  
  enum {
    MIAddKeyFrame, MIDeleteKeyFrame, MIEditKeyFrame, MICopy, MIPaste
  };
  
  void HandlePaint( wxPaintEvent& );
  void HandleEraseBackground( wxEraseEvent& );
  void HandleSize( wxSizeEvent& );
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
  
};

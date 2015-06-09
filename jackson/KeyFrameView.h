/* 
 * File:   KeyFrameView.h
 * Author: rpb
 *
 * Created on June 8, 2015, 10:37 PM
 */

#pragma once

#include <wx/panel.h>

#define SYMBOL_CONTROLKEYFRAMEVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLKEYFRAMEVIEW_TITLE _("KeyFrameView")
#define SYMBOL_CONTROLKEYFRAMEVIEW_IDNAME ID_CONTROLKEYFRAMEVIEW
#define SYMBOL_CONTROLKEYFRAMEVIEW_SIZE wxSize(-1, 20)
#define SYMBOL_CONTROLKEYFRAMEVIEW_POSITION wxDefaultPosition

class KeyFrameView: public wxPanel {
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
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
  
};

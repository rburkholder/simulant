/* 
 * File:   TimeLineView.h
 * Author: rpb
 *
 * Created on May 31, 2015, 6:21 PM
 */

#pragma once

#include <wx/panel.h>

#define SYMBOL_CONTROLTIMELINEVIEW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CONTROLTIMELINEVIEW_TITLE _("TimeLineView")
#define SYMBOL_CONTROLTIMELINEVIEW_IDNAME ID_CONTROLTIMELINEVIEW
#define SYMBOL_CONTROLTIMELINEVIEW_SIZE wxSize(100, 20)
#define SYMBOL_CONTROLTIMELINEVIEW_POSITION wxDefaultPosition

class TimeLineView: public wxPanel {
  DECLARE_DYNAMIC_CLASS( TimeLineView )
public:
  
  TimeLineView( );
  TimeLineView( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLTIMELINEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLTIMELINEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLTIMELINEVIEW_SIZE, 
          long style = SYMBOL_CONTROLTIMELINEVIEW_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLTIMELINEVIEW_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLTIMELINEVIEW_POSITION, 
          const wxSize& size = SYMBOL_CONTROLTIMELINEVIEW_SIZE, 
          long style = SYMBOL_CONTROLTIMELINEVIEW_STYLE );
  virtual ~TimeLineView( );
  
protected:
private:

  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLTIMELINEVIEW 
  };
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
};

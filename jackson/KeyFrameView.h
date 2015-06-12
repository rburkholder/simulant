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
  
  typedef boost::signals2::signal<void (wxPoint&)> signalMouseEvent_t;
  typedef signalMouseEvent_t::slot_type slotMouseEvent_t;
  
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
  
  // how many are actually necessary for the owner
  signalMouseEvent_t m_signalMouseEventAddKeyFrame;
  signalMouseEvent_t m_signalMouseEventEditKeyFrame;
  signalMouseEvent_t m_signalMouseEventDeleteKeyFrame;
  signalMouseEvent_t m_signalMouseEventSelectKeyFrame;
  signalMouseEvent_t m_signalMouseEventMovement;
  
  virtual void UnDrawCursor( Cursor& cursor );

protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLKEYFRAMEVIEW

  };
  
  enum {
    MIAddKeyFrame, MIDeleteKeyFrame, MIEditKeyFrame, MICopy, MIPaste
  };
  
  wxColour m_colourBackground;
  
  wxMenu* m_pContextMenu;
  
  wxPoint m_pointLatestMouse;

  void HandlePaint( wxPaintEvent& );
  void HandleEraseBackground( wxEraseEvent& );
  void HandleSize( wxSizeEvent& );
  
  void HandleMouseRightUp( wxMouseEvent& );
  void HandleMouseLeftUp( wxMouseEvent& );
  
  //void HandleMouseMotion( wxMouseEvent& );

  void HandleAddKeyFrame( wxCommandEvent& event );
  void HandleDeleteKeyFrame( wxCommandEvent& event );
  void HandleEditKeyFrame( wxCommandEvent& event );
  
  void HandleCopy( wxCommandEvent& event );
  void HandlePaste( wxCommandEvent& event );

  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
  
};

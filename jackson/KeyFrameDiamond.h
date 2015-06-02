/* 
 * File:   KeyFrameDiamond.h
 * Author: rpb
 *
 * Created on May 31, 2015, 9:32 PM
 */

#pragma once

#include <wx/window.h>
#include <wx/image.h>

#define SYMBOL_CONTROLKEYFRAMEDIAMONDVIEW_STYLE wxNO_BORDER
#define SYMBOL_CONTROLKEYFRAMEDIAMONDVIEW_TITLE _("KeyFrameDiamond")
#define SYMBOL_CONTROLKEYFRAMEDIAMOND_IDNAME ID_CONTROLKEYFRAMEDIAMOND
#define SYMBOL_CONTROLKEYFRAMEDIAMOND_SIZE wxSize(12, 12)
#define SYMBOL_CONTROLKEYFRAMEDIAMOND_POSITION wxDefaultPosition

class KeyFrameDiamond: public wxWindow {
  DECLARE_DYNAMIC_CLASS( KeyFrameDiamond )
public:
  
  KeyFrameDiamond( );
  KeyFrameDiamond( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLKEYFRAMEDIAMOND_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLKEYFRAMEDIAMOND_POSITION, 
          const wxSize& size = SYMBOL_CONTROLKEYFRAMEDIAMOND_SIZE, 
          long style = SYMBOL_CONTROLKEYFRAMEDIAMONDVIEW_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_CONTROLKEYFRAMEDIAMOND_IDNAME, 
          const wxPoint& pos = SYMBOL_CONTROLKEYFRAMEDIAMOND_POSITION, 
          const wxSize& size = SYMBOL_CONTROLKEYFRAMEDIAMOND_SIZE, 
          long style = SYMBOL_CONTROLKEYFRAMEDIAMONDVIEW_STYLE );
  virtual ~KeyFrameDiamond( );
  
protected:
private:

  enum {
    ID_Null = wxID_HIGHEST,
    ID_CONTROLKEYFRAMEDIAMOND 
  };
  
  wxImage m_image;
  
  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };
};

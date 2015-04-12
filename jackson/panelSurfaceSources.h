/* 
 * File:   panelSurfaceSources.h
 * Author: rpb
 *
 * Created on February 28, 2015, 9:55 PM
 */

#ifndef PANELSURFACESOURCES_H
#define	PANELSURFACESOURCES_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/tglbtn.h>
//#include <wx/treectrl.h>

#include "TreeDisplayManager.h"

#define SYMBOL_PANELSURFACESOURCES_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELSURFACESOURCES_TITLE _("sources")
#define SYMBOL_PANELSURFACESOURCES_IDNAME ID_PANELSURFACESOURCES
#define SYMBOL_PANELSURFACESOURCES_SIZE wxSize(400, 300)
#define SYMBOL_PANELSURFACESOURCES_POSITION wxDefaultPosition

class panelSurfaceSources: public wxPanel {    
  DECLARE_DYNAMIC_CLASS( panelSurfaceSources )
public:
  
  typedef ScreenFrame::pScreenFrame_t pScreenFrame_t;
  
  panelSurfaceSources();
  panelSurfaceSources( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_PANELSURFACESOURCES_IDNAME, 
          const wxPoint& pos = SYMBOL_PANELSURFACESOURCES_POSITION, 
          const wxSize& size = SYMBOL_PANELSURFACESOURCES_SIZE, 
          long style = SYMBOL_PANELSURFACESOURCES_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_PANELSURFACESOURCES_IDNAME, 
          const wxPoint& pos = SYMBOL_PANELSURFACESOURCES_POSITION, 
          const wxSize& size = SYMBOL_PANELSURFACESOURCES_SIZE, 
          long style = SYMBOL_PANELSURFACESOURCES_STYLE );

  ~panelSurfaceSources();
  
  void Append( pScreenFrame_t pScreenFrame );

protected:  
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_PANELSURFACESOURCES,
    ID_BTN_Poly4Side,
    ID_BUTTON1,
    ID_BUTTON2,
    ID_BTN_DISPLAY,
    ID_BUTTON3,
    ID_BTN_DNDTARGET,
    ID_BTN_DNDSOURCE, 
    ID_BTN_EDITMODE, 
    ID_BTN_UNDO, 
    ID_TREE_DISPLAYS
  };
  
  bool m_bInEditMode;
  
  wxButton* m_btnPoly4Side;
  wxButton* m_btnDisplay;
  wxToggleButton* m_btnEditMode;
  wxButton* m_btnUndo;

  TreeDisplayManager* m_treeDisplays;
  
  void HandleToggleEditMode( wxCommandEvent& event );
  void HandleUndo( wxCommandEvent& event );

  void Init();
  void CreateControls();
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  static bool ShowToolTips( void ) { return true; };

};
#endif	/* PANELSURFACESOURCES_H */

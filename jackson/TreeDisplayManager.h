/* 
 * File:   TreeDisplayManager.h
 * Author: rpb
 *
 * Created on April 5, 2015, 10:11 PM
 */

#ifndef TREEDISPLAYMANAGER_H
#define	TREEDISPLAYMANAGER_H

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "ScreenFrame.h"

#define SYMBOL_TREEDISPLAYMANAGER_STYLE wxTR_HAS_BUTTONS | wxTR_SINGLE
#define SYMBOL_TREEDISPLAYMANAGER_TITLE _("Displays")
#define SYMBOL_TREEDISPLAYMANAGER_IDNAME ID_TREEDISPLAYMANAGER
#define SYMBOL_TREEDISPLAYMANAGER_SIZE wxSize(100, 200)
#define SYMBOL_TREEDISPLAYMANAGER_POSITION wxDefaultPosition

class TreeDisplayManagerMenuItemDecoder;

class TreeDisplayManager: public wxTreeCtrl {
  DECLARE_DYNAMIC_CLASS( TreeDisplayManager )
public:
  
  typedef ScreenFrame::pScreenFrame_t pScreenFrame_t;
  
  TreeDisplayManager();
  TreeDisplayManager( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEDISPLAYMANAGER_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEDISPLAYMANAGER_POSITION, 
          const wxSize& size = SYMBOL_TREEDISPLAYMANAGER_SIZE, 
          long style = SYMBOL_TREEDISPLAYMANAGER_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEDISPLAYMANAGER_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEDISPLAYMANAGER_POSITION, 
          const wxSize& size = SYMBOL_TREEDISPLAYMANAGER_SIZE, 
          long style = SYMBOL_TREEDISPLAYMANAGER_STYLE );

  ~TreeDisplayManager();
  
  void Append( pScreenFrame_t pScreenFrame );

protected:
private:
  
  typedef boost::shared_ptr<TreeDisplayManagerMenuItemDecoder> pDecoder_t;
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_TREEDISPLAYMANAGER
  };
  
  pDecoder_t m_pDecoder;
  
  void Init();
  void CreateControls();
  
  void HandleContextMenu( wxTreeEvent& event );
  void HandleSelectionChanged( wxTreeEvent& event );
  void HandleItemActivated( wxTreeEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips( void ) { return true; };
  
};

#endif	/* TREEDISPLAYMANAGER_H */


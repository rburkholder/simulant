/* 
 * File:   TreeDisplayManager.h
 * Author: rpb
 *
 * Created on April 5, 2015, 10:11 PM
 */

#ifndef TREEDISPLAYMANAGER_H
#define	TREEDISPLAYMANAGER_H

#include <map>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "PhysicalDisplay.h"

#define SYMBOL_TREEDISPLAYMANAGER_STYLE wxTR_HAS_BUTTONS | wxTR_SINGLE
#define SYMBOL_TREEDISPLAYMANAGER_TITLE _("Displays")
#define SYMBOL_TREEDISPLAYMANAGER_IDNAME ID_TREEDISPLAYMANAGER
#define SYMBOL_TREEDISPLAYMANAGER_SIZE wxSize(100, 200)
#define SYMBOL_TREEDISPLAYMANAGER_POSITION wxDefaultPosition

class TreeItemBase;

struct CommonGuiElements {
  wxStaticText* pstFrameCounter;
  CommonGuiElements( void ): pstFrameCounter( 0 ) {}
};

class TreeDisplayManager: public wxTreeCtrl {
  DECLARE_DYNAMIC_CLASS( TreeDisplayManager )
public:
  
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef boost::shared_ptr<TreeItemBase> pTreeItem_t;
  
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
  
  void Append( pPhysicalDisplay_t pPhysicalDisplay );
  
  void Add( const wxTreeItemId& id, pTreeItem_t pTreeItem );
  void Delete( wxTreeItemId id );
  
  void SetStaticTextFrameCounter( wxStaticText* pstFrameCounter );

protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_TREEDISPLAYMANAGER
  };
  
  wxTreeItemId m_idOld;
  
  typedef std::map<void*,pTreeItem_t> mapDecoder_t;  // void* is from wxTreeItemId
  mapDecoder_t m_mapDecoder;
  
  CommonGuiElements m_guiElements;
  
  void Init();
  void CreateControls();
  
  void HandleContextMenu( wxTreeEvent& event );
  void HandleSelectionChanged( wxTreeEvent& event );
  void HandleSelectionChanging( wxTreeEvent& event );
  void HandleItemActivated( wxTreeEvent& event );
  void HandleItemDeleted( wxTreeEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips( void ) { return true; };
  
};

#endif	/* TREEDISPLAYMANAGER_H */


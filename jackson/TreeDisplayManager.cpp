/* 
 * File:   TreeDisplayManager.cpp
 * Author: rpb
 * 
 * Created on April 5, 2015, 10:11 PM
 */

#include <map>

#include <boost/shared_ptr.hpp>

#include "TreeDisplayManager.h"

IMPLEMENT_DYNAMIC_CLASS( TreeDisplayManager, wxTreeCtrl )

struct MenuItemBase {
  wxTreeItemId id;
  MenuItemBase( wxTreeItemId id_ ): id( id_ ) {};
};

struct MenuItemRoot: public MenuItemBase {
  MenuItemRoot( wxTreeItemId id_ ): MenuItemBase( id_ ) {
    // add a right click pop up to add remote displays, if not found
  };
};

struct MenuItemDisplay: public MenuItemBase {
  MenuItemDisplay( wxTreeItemId id_ ): MenuItemBase( id_ ) {
    // add a right click pop up to add displayable objects and surfaces
    // which are then serialized for session persistence
    // use text or enum keys to register objects, for subsequent re-creation
  };
};

class TreeDisplayManagerMenuItemDecoder {
public:
  typedef boost::shared_ptr<MenuItemBase> pMenuItem_t;
  typedef std::map<void*,pMenuItem_t> mapDecoder_t;  // void* is from wxTreeItemId
  typedef std::pair<void*,pMenuItem_t> mapDecoder_pair_t;
  mapDecoder_t m_pmapDecoder;
protected:
private:
};

TreeDisplayManager::TreeDisplayManager() {
  Init();
}

TreeDisplayManager::TreeDisplayManager( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool TreeDisplayManager::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxTreeCtrl::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

TreeDisplayManager::~TreeDisplayManager() {
}

void TreeDisplayManager::Init() {
  m_pDecoder.reset( new TreeDisplayManagerMenuItemDecoder );
}

void TreeDisplayManager::CreateControls() {    
  
  TreeDisplayManager* item = this;
  
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_MENU, &TreeDisplayManager::HandleContextMenu, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGED, &TreeDisplayManager::HandleSelectionChanged, this );
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_ACTIVATED, &TreeDisplayManager::HandleItemActivated, this );
  
  wxTreeItemId id = wxTreeCtrl::AddRoot( "displays" );
  TreeDisplayManagerMenuItemDecoder::pMenuItem_t pMenuItem( new MenuItemRoot( id ) );
  m_pDecoder->m_pmapDecoder.insert( 
    TreeDisplayManagerMenuItemDecoder::mapDecoder_pair_t( id.GetID(), pMenuItem ) );
  
  // add the displays to the list, and each has right click context menu
}

void TreeDisplayManager::HandleContextMenu( wxTreeEvent& event ) {
  std::cout << "HandleContextMenu" << std::endl;
}

void TreeDisplayManager::HandleSelectionChanged( wxTreeEvent& event ) {
  std::cout << "HandleSelectionChanged" << std::endl;
}

void TreeDisplayManager::HandleItemActivated( wxTreeEvent& event ) {
  std::cout << "HandleItemActivated" << std::endl;
}

wxBitmap TreeDisplayManager::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeDisplayManager::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

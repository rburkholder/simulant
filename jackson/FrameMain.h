/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// Generic frame for wx based applications

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/statusbr.h>

//#include <OUCommon/FastDelegate.h>
//using namespace fastdelegate;

#define SYMBOL_FRAMEGENERIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX
#define SYMBOL_FRAMEGENERIC_TITLE _("Strategy Runner")
#define SYMBOL_FRAMEGENERIC_IDNAME ID_FRAMEGENERIC
#define SYMBOL_FRAMEGENERIC_SIZE wxSize(400, 500)
#define SYMBOL_FRAMEGENERIC_POSITION wxDefaultPosition

class FrameMain: public wxFrame {
public:

  typedef boost::signals2::signal<void ()> MenuItemSignal_t;  // signal definition
  typedef MenuItemSignal_t::slot_type MenuItemAction_t;       // slot definition

  struct structMenuItem: wxObject {
    std::string text;
    MenuItemSignal_t ActionHandler;  // will call connected slots
    unsigned long ix;
    structMenuItem( void ): ix( 0 ) {};
    structMenuItem( const std::string& text_, const MenuItemAction_t& mia, unsigned long ix_ = 0 )
      : text( text_ ), ix( ix_ ) {
      ActionHandler.connect( mia );
    };
//    structMenuItem( const structMenuItem& rhs )
//      : text( rhs.text ), OnActionHandler.( rhs.OnActionHandler ), ix( rhs.ix ) {};
  };

  typedef std::vector<structMenuItem*> vpMenuItems_t;
  
  MenuItemSignal_t m_MenuItemLoad;  // encapsulate so can hide menu item when no event attached
  MenuItemSignal_t m_MenuItemSave;  // encapsulate so can hide menu item when no event attached

  FrameMain(void);
  FrameMain( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, 
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, 
    long style = SYMBOL_FRAMEGENERIC_STYLE );
  ~FrameMain(void);

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, 
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, 
    long style = SYMBOL_FRAMEGENERIC_STYLE );

  // re-arrange this so when events added/removed, menu updated with text as well, 
  // ie, pass in structure to build menu

  void AddDynamicMenu( const std::string& root, const vpMenuItems_t& vItems );
  
  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:
//
//  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN,
//    ID_MENUACTION1, ID_MENUACTION2, ID_MENUACTION3, ID_MENUACTION4, ID_MENUACTION5, ID_DYNAMIC_MENU_ACTIONS
//  };
  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN, ID_MENULOAD, ID_MENUSAVE,
    ID_DYNAMIC_MENU_ACTIONS  // this should be last item in enumeration
  };

  wxMenuBar* m_menuBar;
  wxStatusBar* m_statusBar;

//  typedef std::vector<structMenuItem*> vPtrItems_t;  // wxWidgets take ownership of object
  vpMenuItems_t m_vPtrItems;  // not appropriate for context based menus

  void Init();
  void CreateControls();

  void OnMenuLoadClick( wxCommandEvent& event );
  void OnMenuSaveClick( wxCommandEvent& event );
  void OnMenuExitClick( wxCommandEvent& event );
  void OnDynamicActionClick( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );
};


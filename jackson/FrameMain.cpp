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

//#include "stdafx.h"

#include "FrameMain.h"

FrameMain::FrameMain(void) {
  Init();
}

FrameMain::FrameMain( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, caption, pos, size, style );
}

FrameMain::~FrameMain(void) {
}

bool FrameMain::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
  Centre();
  return true;
}

void FrameMain::Init() {
  m_menuBar = NULL;
  m_statusBar = NULL;
}

void FrameMain::CreateControls( void ) {

    FrameMain* itemFrame1 = this;

    m_menuBar = new wxMenuBar;
    itemFrame1->SetMenuBar(m_menuBar);

    wxMenu* itemMenuFile = new wxMenu;
    
    itemMenuFile->Append(ID_MENULOAD, _("Load"), wxEmptyString, wxITEM_NORMAL);
    itemMenuFile->Append(ID_MENUSAVE, _("Save"), wxEmptyString, wxITEM_NORMAL);
    itemMenuFile->Append(ID_MENUEXIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);

    m_menuBar->Append(itemMenuFile, _("File"));
    
    m_statusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    m_statusBar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(m_statusBar);
   
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuLoadClick, this, ID_MENULOAD );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuSaveClick, this, ID_MENUSAVE );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );
    Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
}

void FrameMain::OnMenuLoadClick( wxCommandEvent& event ) {
  m_MenuItemLoad();
}

void FrameMain::OnMenuSaveClick( wxCommandEvent& event ) {
  m_MenuItemSave();
}

void FrameMain::OnMenuExitClick( wxCommandEvent& event ) {
  // Exit Steps:  #1 -> Appxxx::OnClose
  this->Close();
}

void FrameMain::AddDynamicMenu( const std::string& sRootName, const vpMenuItems_t& vItems ) {
  assert( 0 != vItems.size() );
  wxMenu* itemMenuAction = new wxMenu;
  for ( vpMenuItems_t::const_iterator iter = vItems.begin(); vItems.end() != iter; ++iter ) {
//    structMenuItem* p( new structMenuItem( vItems[ ix ] ) );
    structMenuItem* p = *iter;
    p->ix = m_vPtrItems.size();
    m_vPtrItems.push_back( p );
    itemMenuAction->Append(ID_DYNAMIC_MENU_ACTIONS + p->ix, p->text, wxEmptyString, wxITEM_NORMAL);
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnDynamicActionClick, this, ID_DYNAMIC_MENU_ACTIONS + p->ix,-1, p );
  }
  m_menuBar->Append(itemMenuAction, sRootName );
}

void FrameMain::OnDynamicActionClick( wxCommandEvent& event ) {
  structMenuItem* p = dynamic_cast<structMenuItem*>( event.m_callbackUserData );
  p->ActionHandler();
}

void FrameMain::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #3 -> Appxxx::OnExit
  Unbind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
//  Unbind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );  // causes crash
  // http://docs.wxwidgets.org/trunk/classwx_close_event.html
  event.Skip();  // continue with base class stuff
}

wxBitmap FrameMain::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameMain::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}


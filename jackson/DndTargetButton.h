/* 
 * File:   DndTargetButton.h
 * Author: rpb
 *
 * Created on March 1, 2015, 5:40 PM
 */

#ifndef DNDTARGETBUTTON_H
#define	DNDTARGETBUTTON_H

#include <iostream>

#include <wx/stattext.h>

#include "DndDropTarget.h"

class DndDataObjectTarget: public wxDataObjectSimple {
public:
  DndDataObjectTarget( ) {};
  DndDataObjectTarget( const wxString &format ): wxDataObjectSimple( format ) {};
  DndDataObjectTarget( wxDataFormatId format=wxDF_INVALID ): wxDataObjectSimple( format ) {};
  
  virtual bool 	GetDataHere (void *buf) const {
    std::cout << "dataobject GetDataHere" << std::endl;
    return false;
  }
  virtual size_t 	GetDataSize () const {
    std::cout << "dataobject GetDataSize" << std::endl;
    return 0;
  }
  virtual bool 	SetData (size_t len, const void *buf) {
    std::cout << "dataobject SetData" << std::endl;
    return true;
  }
};

#define SYMBOL_BUTTON_DNDTARGET_STYLE wxTAB_TRAVERSAL
#define SYMBOL_BUTTON_DNDTARGET_TITLE _("Drag'n'Drop Target")
#define SYMBOL_BUTTON_DNDTARGET_IDNAME ID_BUTTON_DNDTARGET
#define SYMBOL_BUTTON_DNDTARGET_SIZE wxSize(-1, -1)
#define SYMBOL_BUTTON_DNDTARGET_POSITION wxDefaultPosition

class DndTargetButton: public wxStaticText {
public:
    
  DndTargetButton();
  DndTargetButton( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_BUTTON_DNDTARGET_IDNAME, 
          const wxString& text = "",
          const wxPoint& pos = SYMBOL_BUTTON_DNDTARGET_POSITION, 
          const wxSize& size = SYMBOL_BUTTON_DNDTARGET_SIZE, 
          long style = SYMBOL_BUTTON_DNDTARGET_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_BUTTON_DNDTARGET_IDNAME, 
          const wxString& text = "",
          const wxPoint& pos = SYMBOL_BUTTON_DNDTARGET_POSITION, 
          const wxSize& size = SYMBOL_BUTTON_DNDTARGET_SIZE, 
          long style = SYMBOL_BUTTON_DNDTARGET_STYLE );

  virtual ~DndTargetButton();

  void Init();

  void CreateControls();
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_BUTTON_DNDTARGET
  };
  
protected:
private:
  
  DndDropTarget* m_pDndDropTarget;
  //wxTextDataObject* m_pDndDropTextData;
  DndDataObjectTarget* m_pDndDropTextData;
  
};

#endif	/* DNDTARGETBUTTON_H */


/* 
 * File:   DndSourceButton.h
 * Author: rpb
 *
 * Created on March 1, 2015, 2:53 PM
 */

#ifndef DNDSOURCEBUTTON_H
#define	DNDSOURCEBUTTON_H


#include <iostream>

//#include <wx/button.h>
#include <wx/stattext.h>

#define SYMBOL_BUTTON_DNDSOURCE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_BUTTON_DNDSOURCE_TITLE _("Drag'n'Drop Source")
#define SYMBOL_BUTTON_DNDSOURCE_IDNAME ID_BUTTON_DNDSOURCE
#define SYMBOL_BUTTON_DNDSOURCE_SIZE wxSize(-1, -1)
#define SYMBOL_BUTTON_DNDSOURCE_POSITION wxDefaultPosition

class DndSourceButton: public wxStaticText {
public:
    
  DndSourceButton();
  DndSourceButton( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_BUTTON_DNDSOURCE_IDNAME, 
          const wxString& text = "",
          const wxPoint& pos = SYMBOL_BUTTON_DNDSOURCE_POSITION, 
          const wxSize& size = SYMBOL_BUTTON_DNDSOURCE_SIZE, 
          long style = SYMBOL_BUTTON_DNDSOURCE_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_BUTTON_DNDSOURCE_IDNAME, 
          const wxString& text = "",
          const wxPoint& pos = SYMBOL_BUTTON_DNDSOURCE_POSITION, 
          const wxSize& size = SYMBOL_BUTTON_DNDSOURCE_SIZE, 
          long style = SYMBOL_BUTTON_DNDSOURCE_STYLE );

  virtual ~DndSourceButton();

  void Init();

  void CreateControls();
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_BUTTON_DNDSOURCE
  };
  
protected:
private:
  
  bool m_bLeftDown;
  
  void HandleMouseMotion( wxMouseEvent& event );
  void HandleMouseLeftDown( wxMouseEvent& event );
  void HandleMouseLeftUp( wxMouseEvent& event );
};

#endif	/* DNDSOURCEBUTTON_H */


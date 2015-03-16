/* 
 * File:   frameProjection.h
 * Author: rpb
 *
 * Created on March 8, 2015, 12:59 PM
 */

#ifndef FRAMEPROJECTION_H
#define	FRAMEPROJECTION_H

#include <wx/wx.h>
#include <wx/frame.h>

#define SYMBOL_FRAMEPROJECTION_STYLE wxNO_BORDER
#define SYMBOL_FRAMEPROJECTION_TITLE _("Black Projection Background")
#define SYMBOL_FRAMEPROJECTION_IDNAME ID_FRAMEPROJECTION
#define SYMBOL_FRAMEPROJECTION_SIZE wxSize(1920, 1080)
#define SYMBOL_FRAMEPROJECTION_POSITION wxDefaultPosition

class FrameProjection: public wxFrame {
public:
  
  FrameProjection(void);
  FrameProjection( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEPROJECTION_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEPROJECTION_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEPROJECTION_POSITION, 
    const wxSize& size = SYMBOL_FRAMEPROJECTION_SIZE, 
    long style = SYMBOL_FRAMEPROJECTION_STYLE );

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEPROJECTION_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEPROJECTION_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEPROJECTION_POSITION, 
    const wxSize& size = SYMBOL_FRAMEPROJECTION_SIZE, 
    long style = SYMBOL_FRAMEPROJECTION_STYLE );

  virtual ~FrameProjection( );
  
  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  private:

  enum { ID_Null=wxID_HIGHEST, ID_FRAMEPROJECTION
  };
  
  void Init();
  void CreateControls();
    
};

#endif	/* FRAMEPROJECTION_H */


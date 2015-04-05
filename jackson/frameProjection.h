/* 
 * File:   frameProjection.h
 * Author: rpb
 *
 * Created on March 8, 2015, 12:59 PM
 */

#ifndef FRAMEPROJECTION_H
#define	FRAMEPROJECTION_H

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/frame.h>

#include "Outline.h"

#define SYMBOL_FRAMEPROJECTION_STYLE wxNO_BORDER
#define SYMBOL_FRAMEPROJECTION_TITLE _("Black Projection Background")
#define SYMBOL_FRAMEPROJECTION_IDNAME ID_FRAMEPROJECTION
#define SYMBOL_FRAMEPROJECTION_SIZE wxSize(1920, 1080)
#define SYMBOL_FRAMEPROJECTION_POSITION wxDefaultPosition

class FrameProjection: public wxFrame {
public:
  
  typedef boost::shared_ptr<Outline> pOutline_t;
  
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
  
  void SetOutline( pOutline_t pOutline ) { m_pOutline = pOutline; };
  pOutline_t GetOutline( void ) { return m_pOutline; };
  
protected:
private:
  
  enum { ID_Null=wxID_HIGHEST, ID_FRAMEPROJECTION
  };
  
  bool m_bMouseDown;
  pOutline_t m_pOutline;  // need to turn into shared ptr
  
  void Init();
  void CreateControls();
  
  void HandlePaint( wxPaintEvent& event );
  void HandleEraseBackground( wxEraseEvent& event );
  void HandleSize( wxSizeEvent& event );
  void HandleMouseMoved( wxMouseEvent& event );
  void HandleMouseLeftDown( wxMouseEvent& event );
  void HandleMouseLeftUp( wxMouseEvent& event );
  void HandleKey( wxKeyEvent& event );
  
  void Render( wxDC& dc ); // actual DC independent render
    
  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

};

#endif	/* FRAMEPROJECTION_H */


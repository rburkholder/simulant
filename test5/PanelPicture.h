/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

#pragma once


#include <vector>

#include <boost/shared_ptr.hpp>

//#include <wx/wx.h>
#include <wx/image.h>

#define SYMBOL_PANELPICTURE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPICTURE_TITLE _("PanelLibRawOptions")
#define SYMBOL_PANELPICTURE_IDNAME ID_PANELPICTURE
#define SYMBOL_PANELPICTURE_SIZE wxSize(400, 300)
#define SYMBOL_PANELPICTURE_POSITION wxDefaultPosition

class PanelPicture : public wxPanel {
public:

  PanelPicture(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPICTURE_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPICTURE_POSITION,
    const wxSize& size = SYMBOL_PANELPICTURE_SIZE,
    long style = SYMBOL_PANELPICTURE_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPICTURE_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPICTURE_POSITION,
    const wxSize& size = SYMBOL_PANELPICTURE_SIZE,
    long style = SYMBOL_PANELPICTURE_STYLE );

  ~PanelPicture( ) {};

  void SetPicture( wxImage* pImage );

  void OnMouseWheel( wxMouseEvent& event );


protected:
private:

  enum {
    ID_Null = wxID_HIGHEST, ID_PANELPICTURE
  };

  boost::shared_ptr<wxImage> m_pimageOriginal;  // todo:  need to use LibRaw::dcraw_clear_mem
  boost::shared_ptr<wxImage> m_pimageSubset; // used for subset of original image
  boost::shared_ptr<wxBitmap> m_pBitmapMemory;  // in memory device context for caching virgin output image
  boost::shared_ptr<wxMemoryDC> m_pdc;
  wxRect m_rectLocationOfSubsetImage; // in original image
  wxRect m_rectLocationOfScaledImage; // overall window frame (needs proper initialization)

  double m_dblAspectRatio;
  wxPoint m_pointMouse;
  wxPoint m_pointMouseOrigin; // used for mouse left drag operations
  bool m_bMouseLeftDown;

  bool m_bSizeChanged;  
  bool m_bImageChanged;  // handles situation of scale / translation, signals when to redo image calc
  bool m_bCanPaint;
  wxRect m_rectClient;

  struct Fraction {
    int numerator;
    int denominator; 
    Fraction( void ) : numerator( 0 ), denominator( 0 ) {};
    Fraction( int numerator_, int denominator_ ) : numerator( numerator_ ), denominator( denominator_ ) {};
    int operator*(int x) const { assert( 0 != denominator );  return (x * numerator) / denominator; }
  };

  typedef std::vector<Fraction> vScalingFactor_t;
  vScalingFactor_t m_vScalingFactor;
  vScalingFactor_t::const_iterator m_iterScalingFactor;

  void Init( void );

  int TranslateX1FromScaledImageToSubSampledImage( int x1 );
  int TranslateY1FromScaledImageToSubSampledImage( int y1 );
  void NormalizeSubSampledImage( void );

  void ReSize( wxSizeEvent& event );
  void ScaleSubImage( void );
  void TranslateSubImage( void );
  void DrawSubImage( void );

  void OnClose( wxCloseEvent& event );

  void OnSizing( wxSizeEvent& event );
  void OnSize( wxSizeEvent& event );
  void OnEraseBackground( wxEraseEvent& event );
  void OnMouseMotion( wxMouseEvent& event );
  void OnPaint( wxPaintEvent& event );
  void OnMouseLeftDown( wxMouseEvent& event );
  void OnMouseLeftUp( wxMouseEvent& event );
};


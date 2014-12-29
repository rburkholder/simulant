#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/image.h>

class FramePicture: public wxFrame {
public:

  FramePicture( wxWindow *parent,
    const wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    const long style )
    : wxFrame( parent, id, title, pos, size, style | wxFULL_REPAINT_ON_RESIZE ),
    m_pimageOriginal( 0 ), 
    m_bSizeChanged( false ), m_bImageChanged( false ), m_bMouseLeftDown( false )
  {

  };
  ~FramePicture( ) {};

  void SetPicture( wxImage* pImage );

protected:
private:

  boost::shared_ptr<wxImage> m_pimageOriginal;  // need to change to shared_ptr as well
  boost::shared_ptr<wxImage> m_pimageSubset; // used for subset of original image
  boost::shared_ptr<wxBitmap> m_pBitmapMemory;
  boost::shared_ptr<wxMemoryDC> m_pdc;
  wxRect m_rectLocationOfSubsetImage; // in original image
  wxRect m_rectLocationOfScaledImage; // overall window frame (needs proper initialization)

  double m_dblAspectRatio;
  //int m_intScale;  // for now 1x - 16x, later may be able to reduce size, but for now, scale and crop to match frame
  wxPoint m_pointMouse;
  wxPoint m_pointMouseOrigin; // used for mouse left drag operations
  bool m_bMouseLeftDown;

  bool m_bSizeChanged;  
  bool m_bImageChanged;  // handles situation of scale / translation, signals when to redo image calc
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

  int TranslateX1FromScaledImageToSubSampledImage( int x1 );
  int TranslateY1FromScaledImageToSubSampledImage( int y1 );
  void NormalizeSubSampledImage( void );

  void ReSize( wxSizeEvent& event );
  void ScaleSubImage( void );
  void TranslateSubImage( void );
  void DrawSubImage( void );

  void OnSizing( wxSizeEvent& event );
  void OnSize( wxSizeEvent& event );
  void OnEraseBackground( wxEraseEvent& event );
  void OnMouseWheel( wxMouseEvent& event );
  void OnMouseMotion( wxMouseEvent& event );
  void OnPaint( wxPaintEvent& event );
  void OnMouseLeftDown( wxMouseEvent& event );
  void OnMouseLeftUp( wxMouseEvent& event );
};


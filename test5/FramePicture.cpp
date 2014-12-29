#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "FramePicture.h"

void FramePicture::SetPicture( wxImage* pImage ) {

  m_vScalingFactor.push_back( Fraction( 1, 1 ) );
  m_vScalingFactor.push_back( Fraction( 7, 8 ) );
  m_vScalingFactor.push_back( Fraction( 4, 5 ) );
  m_vScalingFactor.push_back( Fraction( 2, 3 ) );
  m_vScalingFactor.push_back( Fraction( 1, 2 ) );
  m_vScalingFactor.push_back( Fraction( 1, 3 ) );
  m_vScalingFactor.push_back( Fraction( 1, 5 ) );
  m_vScalingFactor.push_back( Fraction( 1, 8 ) );
  m_vScalingFactor.push_back( Fraction( 1, 10 ) );
  m_vScalingFactor.push_back( Fraction( 1, 12 ) );
  m_vScalingFactor.push_back( Fraction( 1, 14 ) );
  m_vScalingFactor.push_back( Fraction( 1, 16 ) );
  m_vScalingFactor.push_back( Fraction( 1, 20 ) );

  m_iterScalingFactor = m_vScalingFactor.begin();

  m_pimageOriginal.reset( pImage );
  m_pimageSubset.reset( new wxImage( *m_pimageOriginal ) );
  m_dblAspectRatio = (double)pImage->GetWidth( ) / (double)pImage->GetHeight( );
  m_rectLocationOfSubsetImage 
    = m_rectLocationOfScaledImage 
    = wxRect( 0, 0, m_pimageSubset->GetWidth(), m_pimageSubset->GetHeight() );
  Bind( wxEVT_PAINT, &FramePicture::OnPaint, this );
  Bind( wxEVT_ERASE_BACKGROUND, &FramePicture::OnEraseBackground, this );
  Bind( wxEVT_MOUSEWHEEL, &FramePicture::OnMouseWheel, this );
  Bind( wxEVT_MOTION, &FramePicture::OnMouseMotion, this );
  Bind( wxEVT_SIZE, &FramePicture::OnSize, this );
  Bind( wxEVT_SIZING, &FramePicture::OnSizing, this );
  Bind( wxEVT_LEFT_DOWN, &FramePicture::OnMouseLeftDown, this );
  Bind( wxEVT_LEFT_UP, &FramePicture::OnMouseLeftUp, this );
}

void FramePicture::OnSizing( wxSizeEvent& event ) {
  ReSize( event );
}

void FramePicture::OnSize( wxSizeEvent& event ) {
  ReSize( event );
}

void FramePicture::ReSize( wxSizeEvent& event ) {
  wxRect rect = GetClientRect();
  if (rect != m_rectClient) {
    m_bSizeChanged = true;
    m_rectClient = rect;
    //FramePicture::Refresh( false );  // redundant call?
  }
  else {

  }
}

void FramePicture::OnEraseBackground( wxEraseEvent& event ) {
  event.Skip( false );  // say we have performed the operation, don't need the flash
}

int FramePicture::TranslateX1FromScaledImageToSubSampledImage( int x1 ) {
  return ((x1 - m_rectLocationOfScaledImage.GetX( ))
    * m_rectLocationOfSubsetImage.GetWidth( ))
    / m_rectLocationOfScaledImage.GetWidth( );
}

int FramePicture::TranslateY1FromScaledImageToSubSampledImage( int y1 ) {
  return ((y1 - m_rectLocationOfScaledImage.GetY( ))
    * m_rectLocationOfSubsetImage.GetHeight( ))
    / m_rectLocationOfScaledImage.GetHeight( );
}

void FramePicture::NormalizeSubSampledImage( void ) {
  // ensure sub sampled image is within bounds of original image

  int xSubSet = m_rectLocationOfSubsetImage.GetX( );
  int ySubSet = m_rectLocationOfSubsetImage.GetY();

  xSubSet = 0 > xSubSet ? 0 : xSubSet;
  ySubSet = 0 > ySubSet ? 0 : ySubSet;

  int diffWidth = m_pimageOriginal->GetWidth( ) - m_rectLocationOfSubsetImage.GetWidth( );
  int diffHeight = m_pimageOriginal->GetHeight( ) - m_rectLocationOfSubsetImage.GetHeight( );

  xSubSet = xSubSet > diffWidth ? diffWidth : xSubSet;
  ySubSet = ySubSet > diffHeight ? diffHeight : ySubSet;

  assert( 0 <= xSubSet );
  assert( 0 <= ySubSet );

  m_rectLocationOfSubsetImage.SetX( xSubSet );
  m_rectLocationOfSubsetImage.SetY( ySubSet );
}

void FramePicture::ScaleSubImage( void ) {

  // x1, y1: offset into SubImage
  int X1old = TranslateX1FromScaledImageToSubSampledImage( m_pointMouse.x );
  int Y1old = TranslateY1FromScaledImageToSubSampledImage( m_pointMouse.y );

  const Fraction& ratio( *m_iterScalingFactor);

  int widthSubSampledNew = ratio * m_pimageOriginal->GetWidth( );
  int heightSubSampledNew = ratio * m_pimageOriginal->GetHeight( );

  // x1, y1: offset into SubImage
  int X1new = (X1old * widthSubSampledNew) / m_rectLocationOfSubsetImage.GetWidth( );
  int Y1new = (Y1old * heightSubSampledNew) / m_rectLocationOfSubsetImage.GetHeight( );

  // x2, y2: offset of subimage into original image
  int X2new = m_rectLocationOfSubsetImage.GetX( ) + X1old - X1new;
  int Y2new = m_rectLocationOfSubsetImage.GetY( ) + Y1old - Y1new;

  m_rectLocationOfSubsetImage =
    wxRect( X2new, Y2new, widthSubSampledNew, heightSubSampledNew );

  NormalizeSubSampledImage( );

  if ((1 < widthSubSampledNew) && (1 < heightSubSampledNew)) {
    m_pimageSubset.reset( new wxImage( m_pimageOriginal->GetSubImage( m_rectLocationOfSubsetImage ) ) );
    m_bImageChanged = true;
  }

}

void FramePicture::TranslateSubImage( void ) {

  // x1, y1: offset into SubImage
  int X1 = TranslateX1FromScaledImageToSubSampledImage( m_pointMouse.x );
  int Y1 = TranslateY1FromScaledImageToSubSampledImage( m_pointMouse.y );

  int X1base = TranslateX1FromScaledImageToSubSampledImage( m_pointMouseOrigin.x );
  int Y1base = TranslateY1FromScaledImageToSubSampledImage( m_pointMouseOrigin.y );

  m_rectLocationOfSubsetImage.SetX( m_rectLocationOfSubsetImage.GetX( ) + X1base - X1 );
  m_rectLocationOfSubsetImage.SetY( m_rectLocationOfSubsetImage.GetY( ) + Y1base - Y1 );

  NormalizeSubSampledImage( );

  m_pimageSubset.reset( new wxImage( m_pimageOriginal->GetSubImage( m_rectLocationOfSubsetImage ) ) );
  m_bImageChanged = true;

}

void FramePicture::OnMouseWheel( wxMouseEvent& event ) {
  //event.GetWheelDelta;
  int intRotation = event.GetWheelRotation( );
  bool bNewScale( false );

  if (0 != intRotation) {
    if (0 < intRotation) { // postive rotation increases scale
      ++m_iterScalingFactor;
      if (m_vScalingFactor.end() == m_iterScalingFactor) --m_iterScalingFactor;
      else bNewScale = true;
    }
    else { // decrease scale
      if (m_vScalingFactor.begin() != m_iterScalingFactor) {
        --m_iterScalingFactor;
        bNewScale = true;
      }
    }

    if (bNewScale) {
      ScaleSubImage( );
      FramePicture::Refresh( false );
    }
  }
}

void FramePicture::OnMouseMotion( wxMouseEvent& event ) {
  // need to track further with mouse button for translation events
  wxPoint point( event.GetX(), event.GetY() );
  if (point != m_pointMouse) {
    m_pointMouse = point;
    if (m_bMouseLeftDown) {
      if (m_pointMouse != m_pointMouseOrigin) {
        TranslateSubImage();
        m_pointMouseOrigin = m_pointMouse;
      }
    }
    FramePicture::Refresh( false );
  }
}

void FramePicture::OnMouseLeftDown( wxMouseEvent& event ) {
  m_pointMouseOrigin = wxPoint( event.GetX( ), event.GetY( ) );
  m_bMouseLeftDown = true;
}

void FramePicture::OnMouseLeftUp( wxMouseEvent& event ) {
  m_bMouseLeftDown = false;
}

void FramePicture::DrawSubImage( ) {

  wxBrush brushGray( wxColour( 128, 128, 128 ) );
  wxPen penGray( wxColour( 128, 128, 128 ) );

  wxRect rect = m_rectClient;
  int width( rect.GetWidth() );
  int height( rect.GetHeight() );
  int offsetX( 0 );
  int offsetY( 0 );

  if ((4 < width) && (4 < height)) {

    // equivalent of double buffering
    m_pBitmapMemory.reset( new wxBitmap( width, height ) );
    m_pdc.reset( new wxMemoryDC( *m_pBitmapMemory ) );

    double dblAspectRatio = (double)width / (double)height;
    if (m_dblAspectRatio != dblAspectRatio) {
      m_pdc->SetBrush( brushGray );
      m_pdc->SetPen( penGray );
      if (m_dblAspectRatio > dblAspectRatio) {
        // width is ok, so reduce height, and fill space
        height = (width * m_pimageSubset->GetHeight( )) / m_pimageSubset->GetWidth( );
        offsetY = (rect.GetHeight() - height) / 2;
        m_pdc->DrawRectangle( 0, 0, width, offsetY );
        m_pdc->DrawRectangle( 0, rect.GetHeight( ) - offsetY, width, offsetY );
      }
      else {
        // height is ok, so reduce width, and fill space
        width = (height * m_pimageSubset->GetWidth( )) / m_pimageSubset->GetHeight( );
        offsetX = (rect.GetWidth() - width) / 2;
        m_pdc->DrawRectangle( 0, 0, offsetX, height );
        m_pdc->DrawRectangle( rect.GetWidth( ) - offsetX, 0, offsetX, height );
      }
    }
    
    m_rectLocationOfScaledImage = wxRect( offsetX, offsetY, width, height );

    wxImage image = m_pimageSubset->Scale( width, height, wxIMAGE_QUALITY_HIGH );
    wxBitmap bitmap( image );
    m_pdc->DrawBitmap( bitmap, offsetX, offsetY );

  }
  else {
    throw std::runtime_error( "no image" );
  }
}

void FramePicture::OnPaint( wxPaintEvent& event )   {

  wxPaintDC dcPaint( this );

  if (dcPaint.CanDrawBitmap( )) {
    try {
      if (m_bImageChanged||m_bSizeChanged) {
        DrawSubImage( );
        m_bSizeChanged = m_bImageChanged = false;
      }
      bool result = dcPaint.Blit( 0, 0, m_rectClient.GetWidth( ), m_rectClient.GetHeight( ), m_pdc.get( ), 0, 0 );
      // draw status text on top
      wxColour white( 255, 255, 255 );
      dcPaint.SetTextForeground( white );

      std::string sClientSizeWidth( boost::lexical_cast<std::string>(m_rectClient.GetWidth()));
      std::string sClientSizeHeight( boost::lexical_cast<std::string>(m_rectClient.GetHeight( )));
      std::string sMouseX( boost::lexical_cast<std::string>(m_pointMouse.x) );
      std::string sMouseY( boost::lexical_cast<std::string>(m_pointMouse.y) );
      //std::string sScaleNum( boost::lexical_cast<std::string>(m_intScale) );

      dcPaint.DrawText( 
        "Client Rect: " + sClientSizeWidth + "," + sClientSizeHeight +
        "\nMouse Down: " + (m_bMouseLeftDown ? "yes" : "no") +
        "\nMouse Position: " + sMouseX + "," + sMouseY,
        //"\nMouse Scale: " + sScale,
        4, 4 );
    }
    catch (std::runtime_error& error) {
    }
  }
}

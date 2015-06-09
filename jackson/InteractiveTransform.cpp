/* 
 * File:   InteractiveTransform.cpp
 * Author: rpb
 * 
 * Created on April 26, 2015, 3:17 PM
 */

#include <iostream>

#include <boost/lexical_cast.hpp>

#include "AspectRatio.h"
#include "InteractiveTransform.h"

InteractiveTransform::InteractiveTransform( int width, int height ) 
: m_floatFactor( 1.0f ), m_bReceivingEvents( false ), 
  m_pWin( 0 ), m_pSliderZ( 0 ), m_pSliderFader( 0 ),
                  m_oldZ( 0 ), m_oldFader( 100 ),
  m_fWidth( width ), m_fHeight( height )
{
  ResetTransformMatrix();
}

InteractiveTransform::~InteractiveTransform( ) {
  if ( m_bReceivingEvents ) DeActivate();
}

void InteractiveTransform::Set(int width, int height) {
  m_fWidth = width;
  m_fHeight = height;
  ResetTransformMatrix();
}

void InteractiveTransform::Activate( wxWindow* win, wxSlider* sliderZ, wxSlider* sliderFader ) {
  if ( !m_bReceivingEvents ) {
    m_pWin = win;
    m_pWin->Bind( wxEVT_MOUSEWHEEL, &InteractiveTransform::HandleMouseWheel, this );
    m_pWin->Bind( wxEVT_MOTION, &InteractiveTransform::HandleMouseMoved, this );
    m_pWin->Bind( wxEVT_LEFT_DOWN, &InteractiveTransform::HandleMouseLeftDown, this );

    m_pSliderZ = sliderZ;
    m_pSliderZ->SetValue( m_oldZ );
    UpdateSliderZ();
    //sliderZ->wxEVT_SCROLL_CHANGED, wxEVT_SCROLL_THUMBTRACK,  wxEVT_SCROLL_THUMBRELEASE
    m_pSliderZ->Bind( wxEVT_SCROLL_CHANGED, &InteractiveTransform::HandleScrollChangedZ, this );
    m_pSliderZ->Bind( wxEVT_SCROLL_THUMBTRACK, &InteractiveTransform::HandleScrollChangedZ, this );
    m_pSliderZ->Enable();
    
    m_pSliderFader = sliderFader;
    m_pSliderFader->SetValue( m_oldFader );
    UpdateSliderFader();
    //sliderZ->wxEVT_SCROLL_CHANGED, wxEVT_SCROLL_THUMBTRACK,  wxEVT_SCROLL_THUMBRELEASE
    m_pSliderFader->Bind( wxEVT_SCROLL_CHANGED, &InteractiveTransform::HandleScrollChangedFader, this );
    m_pSliderFader->Bind( wxEVT_SCROLL_THUMBTRACK, &InteractiveTransform::HandleScrollChangedFader, this );
    m_pSliderFader->Enable();
    
    m_bReceivingEvents = true;
  }
}

void InteractiveTransform::DeActivate( void ) {
  assert( 0 != m_pWin );
  if ( m_bReceivingEvents ) {
    m_pSliderFader->Enable( false );
    m_pSliderFader->Unbind( wxEVT_SCROLL_THUMBTRACK, &InteractiveTransform::HandleScrollChangedFader, this );
    m_pSliderFader->Unbind( wxEVT_SCROLL_CHANGED, &InteractiveTransform::HandleScrollChangedFader, this );
    m_pSliderFader = 0;
    
    m_pSliderZ->Enable( false );
    m_pSliderZ->Unbind( wxEVT_SCROLL_THUMBTRACK, &InteractiveTransform::HandleScrollChangedZ, this );
    m_pSliderZ->Unbind( wxEVT_SCROLL_CHANGED, &InteractiveTransform::HandleScrollChangedZ, this );
    m_pSliderZ = 0;
    
    m_pWin->Unbind( wxEVT_MOUSEWHEEL, &InteractiveTransform::HandleMouseWheel, this );
    m_pWin->Unbind( wxEVT_MOTION, &InteractiveTransform::HandleMouseMoved, this );
    m_pWin->Unbind( wxEVT_LEFT_DOWN, &InteractiveTransform::HandleMouseLeftDown, this );
    m_pWin = 0;
    
    m_bReceivingEvents = false;
  }
}

void InteractiveTransform::ResetTransformMatrix( void ) {
  m_oldZ = 0;
  m_oldFader = 100;
  m_mat4Rotation = glm::mat4( 1.0f );
  m_mat4Scale = glm::mat4( 1.0f );
  m_mat4Translation = glm::mat4( 1.0f );
  m_matAspectRatio = AspectRatioWindow( m_fHeight, m_fWidth );
  m_mat4Transform = glm::mat4( 1.0f ) * m_matAspectRatio;
  //m_mat4Transform = ( ( m_mat4Translation * m_mat4Scale ) * m_mat4Rotation ) * m_matAspectRatio;
  if ( 0 != m_pSliderZ ) m_pSliderZ->SetValue( m_oldZ );
  if ( 0 != m_pSliderFader ) m_pSliderFader->SetValue( m_oldFader );

  UpdateSliderZ();
  UpdateSliderFader();
  
  UpdateTransformMatrix( m_mat4Transform );
  UpdateFade( 100.0f );
}

void InteractiveTransform::UpdateSliderZ( void ) {
  if ( 0 != m_pSliderZ ) {
    std::string sSliderZ0( boost::lexical_cast<std::string>( m_oldZ ) );
    std::string sSliderZ1( "z=" + sSliderZ0 );
    m_pSliderZ->SetToolTip( sSliderZ1 );
  }
}

void InteractiveTransform::UpdateSliderFader( void ) {
  if ( 0 != m_pSliderFader ) {
    std::string sSliderFader0( boost::lexical_cast<std::string>( m_oldFader ) );
    std::string sSliderFader1( "fade=" + sSliderFader0 );
    m_pSliderFader->SetToolTip( sSliderFader1 );
  }
}

void InteractiveTransform::HandleScrollChangedZ( wxScrollEvent& event ) {
  int z = event.GetPosition();  // -z is out of window
  float zNew = (float)z / 100.0f;
  float zOld = (float)m_oldZ / 100.0f;
  glm::vec3 vTranslateOld = glm::vec3( 0.0f, 0.0f,  zOld );  // - for screen to window conversion
  glm::vec3 vTranslateNew = glm::vec3( 0.0f, 0.0f, -zNew );  // - for screen to window conversion
  m_mat4Translation *= glm::translate( vTranslateNew ) * glm::translate( vTranslateOld );
  m_oldZ = z;
  UpdateSliderZ();
  m_mat4Transform = ( ( m_mat4Translation * m_mat4Scale ) * m_mat4Rotation ) * m_matAspectRatio;
  UpdateTransformMatrix( m_mat4Transform );
  event.Skip( false );
}

void InteractiveTransform::HandleScrollChangedFader( wxScrollEvent& event ) {
  int fader = event.GetPosition(); 
  float faderNew = (float)fader / 100.0f;
  float faderOld = (float)m_oldFader / 100.0f;
  m_oldFader = fader;
  UpdateSliderFader();
  UpdateFade( faderNew );
  event.Skip( false );
}

void InteractiveTransform::HandleMouseLeftDown( wxMouseEvent& event ) {
  //std::cout << "left down " << event.GetX() << ", " << event.GetY() << std::endl;
  m_intMouseX = event.GetX();
  m_intMouseY = event.GetY();
  event.Skip();
}

void InteractiveTransform::HandleMouseMoved( wxMouseEvent& event ) {
  //std::cout << "mouse moved " << event.GetX() << ", " << event.GetY() << std::endl;
  if ( event.LeftIsDown() ) {
    int difX = event.GetX() - m_intMouseX;
    int difY = event.GetY() - m_intMouseY;
    
    float fNormalizer = m_fHeight;
    if ( m_fWidth < m_fHeight )
      fNormalizer = m_fWidth;
    float ratioX = (float) difX / fNormalizer;
    float ratioY = (float) difY / fNormalizer;
    //std::cout << "drag " << difX << ", " << difY << ", " << ratioX << ", " << ratioY << std::endl;
    if ( event.ControlDown() ) { // rotation
      glm::vec3 vRotate;
      float degrees = event.ShiftDown() ? 0.1f : 1.0f;
      vRotate = glm::vec3( 0.0f, 1.0f, 0.0f );
      m_mat4Rotation *= glm::rotate( ratioX * degrees, vRotate );
      vRotate = glm::vec3( 1.0f, 0.0f, 0.0f );
      m_mat4Rotation *= glm::rotate( ratioY * degrees, vRotate );
    }
    else { // translation
      float multiplier = 2.0 * ( event.ShiftDown() ? 0.1f : 1.0f );
      //float multiplier = ( event.ShiftDown() ? 0.1f : 1.0f );
      //std::cout << "multiplier " << multiplier << std::endl;
      glm::vec3 vTranslate = glm::vec3( multiplier * ratioX, -multiplier * ratioY, 0.0f );  // - for screen to window conversion
      m_mat4Translation *= glm::translate( vTranslate );
    }
    //m_mat4Transform = ( ( glm::mat4( 1.0f ) * m_mat4Translation ) * m_mat4Scale ) * m_mat4Rotation;
    m_mat4Transform = ( ( m_mat4Translation * m_mat4Scale ) * m_mat4Rotation ) * m_matAspectRatio;
    UpdateTransformMatrix( m_mat4Transform );
  }
  
  m_intMouseX = event.GetX();
  m_intMouseY = event.GetY();
  
  //event.Skip( false );
  event.Skip();
}

void InteractiveTransform::HandleMouseWheel( wxMouseEvent& event ) {
  
  //std::cout << "mouse wheel " << event.GetWheelDelta() << ", " << event.GetWheelRotation() << std::endl;
  static const float scaleMajor( 0.10f );
  static const float scaleMinor( 0.01f );
  
  int n( event.GetWheelRotation() );
  if ( 0 != n ) {
    if ( event.m_controlDown ) { // control selects rotation about z axis
      float degrees = event.ShiftDown() ? 0.01f : 0.10f;
      glm::vec3 vRotate = glm::vec3( 0.0f, 0.0f, 1.0f );
      if ( 0 < n ) { //positive
        m_mat4Rotation *= glm::rotate( degrees, vRotate );
      }
      else {
        m_mat4Rotation *= glm::rotate( -degrees, vRotate );
      }
    }
    else { // scale xy together
      float factor( 1.0f );
      glm::vec3 vScale;
      if ( 0 < n ) { // positive
        // zoom in, and update transform matrix
        factor = 1.0f + ( event.ShiftDown() ? scaleMinor : scaleMajor );
        vScale = glm::vec3( factor, factor, 0.0f );
      }
      else { // negative
        // zoom out, and update transform matrix
        factor = 1.0f / ( 1.0f + ( event.ShiftDown() ? scaleMinor : scaleMajor ) );
        vScale = glm::vec3( factor, factor, 0.0f );
      }
      m_floatFactor *= factor;
      //std::cout << "scale factor: " << m_floatFactor << std::endl;
      m_mat4Scale *= glm::scale( vScale );
    }
    //m_mat4Transform = ( ( glm::mat4( 1.0f ) * m_mat4Translation ) * m_mat4Scale ) * m_mat4Rotation;
    m_mat4Transform = ( ( m_mat4Translation * m_mat4Scale ) * m_mat4Rotation ) * m_matAspectRatio;
    UpdateTransformMatrix( m_mat4Transform );
  }
  //event.Skip( false );
  event.Skip();
}


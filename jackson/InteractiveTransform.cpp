/* 
 * File:   InteractiveTransform.cpp
 * Author: rpb
 * 
 * Created on April 26, 2015, 3:17 PM
 */

#include <iostream>

#include "AspectRatio.h"
#include "InteractiveTransform.h"

InteractiveTransform::InteractiveTransform( int width, int height ) 
: m_fWidth( width ), m_fHeight( height ), m_floatFactor( 1.0f ), m_bReceivingEvents( false ), m_pWin( 0 )
{
}

InteractiveTransform::~InteractiveTransform( ) {
  if ( m_bReceivingEvents ) DeActivate();
}

void InteractiveTransform::Activate( wxWindow* win ) {
  if ( !m_bReceivingEvents ) {
    win->Bind( wxEVT_MOUSEWHEEL, &InteractiveTransform::HandleMouseWheel, this );
    win->Bind( wxEVT_MOTION, &InteractiveTransform::HandleMouseMoved, this );
    win->Bind( wxEVT_LEFT_DOWN, &InteractiveTransform::HandleMouseLeftDown, this );
    m_pWin = win;
    m_bReceivingEvents = true;
  }
}

void InteractiveTransform::DeActivate( void ) {
  assert( 0 != m_pWin );
  if ( m_bReceivingEvents ) {
    m_pWin->Unbind( wxEVT_MOUSEWHEEL, &InteractiveTransform::HandleMouseWheel, this );
    m_pWin->Unbind( wxEVT_MOTION, &InteractiveTransform::HandleMouseMoved, this );
    m_pWin->Unbind( wxEVT_LEFT_DOWN, &InteractiveTransform::HandleMouseLeftDown, this );
    m_pWin = 0;
    m_bReceivingEvents = false;
  }
}
  
void InteractiveTransform::ResetTransformMatrix( void ) {
  m_mat4Transform = AspectRatio( m_fHeight, m_fWidth );
}

void InteractiveTransform::HandleMouseLeftDown( wxMouseEvent& event ) {
  //std::cout << "left down " << event.GetX() << ", " << event.GetY() << std::endl;
  m_intMouseX = event.GetX();
  m_intMouseY = event.GetY();
}

void InteractiveTransform::HandleMouseMoved( wxMouseEvent& event ) {
  //std::cout << "mouse moved " << event.GetX() << ", " << event.GetY() << std::endl;
  if ( event.LeftIsDown() ) {
    int difX = event.GetX() - m_intMouseX;
    int difY = event.GetY() - m_intMouseY;
    // need to use aspect ratio for following so consistent ratios in each direction
    //float ratioX = (float) difX / (float) m_pOglGrid->GetClientSize().GetWidth();
    float ratioX = (float) difX / m_fWidth;
    //float ratioY = (float) difY / (float) m_pOglGrid->GetClientSize().GetHeight();
    float ratioY = (float) difY / m_fHeight;
    //std::cout << "drag " << difX << ", " << difY << ", " << ratioX << ", " << ratioY << std::endl;
    if ( event.ControlDown() ) { // rotation
      glm::vec3 vRotate;
      float degrees = event.ShiftDown() ? 0.1f : 1.0f;
      vRotate = glm::vec3( 0.0f, 1.0f, 0.0f );
      m_mat4Transform *= glm::rotate( ratioX * degrees, vRotate );
      vRotate = glm::vec3( 1.0f, 0.0f, 0.0f );
      m_mat4Transform *= glm::rotate( ratioY * degrees, vRotate );
    }
    else { // translation
      float multiplier = 2.0 * ( event.ShiftDown() ? 0.1f : 1.0f );
      //std::cout << "multiplier " << multiplier << std::endl;
      glm::vec3 vTranslate = glm::vec3( multiplier * ratioX, -multiplier * ratioY, 0.0f );
      m_mat4Transform *= glm::translate( vTranslate );
    }
    UpdateTransformMatrix();
  }
  
  m_intMouseX = event.GetX();
  m_intMouseY = event.GetY();
  
  event.Skip( false );
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
        m_mat4Transform *= glm::rotate( degrees, vRotate );
      }
      else {
        m_mat4Transform *= glm::rotate( -degrees, vRotate );
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
      m_mat4Transform *= glm::scale( vScale );
    }
    UpdateTransformMatrix();
  }
  event.Skip( false );
}


/* 
 * File:   InteractiveTransform.h
 * Author: rpb
 *
 * Created on April 26, 2015, 3:17 PM
 */

#pragma once

#include <wx/wx.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class InteractiveTransform {
public:
  InteractiveTransform( int width, int height );
  virtual ~InteractiveTransform( );
protected:
  
  glm::mat4 m_mat4Transform;
  
  void Activate( wxWindow* );
  void DeActivate( void );
  
  void ResetTransformMatrix( void );
  void HandleMouseWheel( wxMouseEvent& event );
  void HandleMouseMoved( wxMouseEvent& event );
  void HandleMouseLeftDown( wxMouseEvent& event );
  
  virtual void UpdateTransformMatrix( void ) {};
  
private:
  
  float m_fHeight;
  float m_fWidth;
  float m_floatFactor;
  
  int m_intMouseX;
  int m_intMouseY;
  
  bool m_bReceivingEvents;
  
  wxWindow* m_pWin;
  
};

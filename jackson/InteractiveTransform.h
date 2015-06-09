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
  void Set( int width, int height );
protected:
  
  void Activate( wxWindow* win, wxSlider* z, wxSlider* fader );
  void DeActivate( void );
  
  void ResetTransformMatrix( void );
  void HandleMouseWheel( wxMouseEvent& event );
  void HandleMouseMoved( wxMouseEvent& event );
  void HandleMouseLeftDown( wxMouseEvent& event );
  void HandleScrollChangedZ( wxScrollEvent& event );
  void HandleScrollChangedFader( wxScrollEvent& event );
  
  void UpdateTransformMatrix( void ) { UpdateTransformMatrix( m_mat4Transform ); }
  virtual void UpdateTransformMatrix( const glm::mat4& ) {};
  const glm::mat4& GetTransformMatrix( void ) const { return m_mat4Transform; }
  
  virtual void UpdateFade( float ) {};
  
private:
  
  float m_fHeight;
  float m_fWidth;
  float m_floatFactor;
  
  int m_intMouseX;
  int m_intMouseY;
  
  bool m_bReceivingEvents;
  
  // keep each transform type separate and make composite on demand
  glm::mat4 m_matAspectRatio;
  glm::mat4 m_mat4Rotation;
  glm::mat4 m_mat4Scale;
  glm::mat4 m_mat4Translation;
  
  glm::mat4 m_mat4Transform;
  
  int m_oldZ;  // previous z
  int m_oldFader; // previous fader
  
  wxWindow* m_pWin;
  wxSlider* m_pSliderZ;
  wxSlider* m_pSliderFader;
  
  void UpdateSliderZ( void );
  void UpdateSliderFader( void );
  
  
};

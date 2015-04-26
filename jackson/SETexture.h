/* 
 * File:   SETexture.h
 * Author: rpb
 *
 * Created on April 25, 2015, 9:16 PM
 */

#pragma once

//SceneElement Texture

#include "SceneElement.h"

#include <glm/glm.hpp>

class SETexture: public SceneElement {
public:
  
  SETexture( );
  virtual ~SETexture( );
  
  virtual void Init( void );
  virtual void Paint( void );
  
  // need to send in transformation matrix instead
  void SetWindowCoords( std::vector<glm::vec4>&  vCoords );
  
  void SetImage( wxImage* pImage ) {  // deletes image on destruction, should be shared ptr
    assert( 0 != pImage );
//    if ( 0 != m_pImage ) {
//      delete m_pImage;
//      m_pImage = 0;
//    }
    m_pImage = pImage;
  }
  
protected:
  
private:

  GLuint m_idTexture;
  
  wxImage* m_pImage;  // should be shared_ptr so can be overwritten and self delete
  
  std::vector<glm::vec2> m_vtxWindowCoords;
  
};


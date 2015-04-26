/* 
 * File:   SETexture.h
 * Author: rpb
 *
 * Created on April 25, 2015, 9:16 PM
 */

#pragma once

//SceneElement Texture

#include <boost/shared_ptr.hpp>

#include "SceneElement.h"

#include <glm/glm.hpp>

class SETexture: public SceneElement {
public:
  
  typedef boost::shared_ptr<wxImage> pImage_t;
  
  SETexture( );
  virtual ~SETexture( );
  
  virtual void Init( void );
  virtual void Paint( void );
  
  // need to send in transformation matrix instead
  void SetWindowCoords( std::vector<glm::vec4>&  vCoords );
  
  void SetImage( pImage_t pImage ) {
    assert( 0 != pImage.use_count() );
    m_pImage = pImage;
  }
  
protected:
  
private:

  GLuint m_idTexture;
  
  pImage_t m_pImage;  // should be shared_ptr so can be overwritten and self delete
  
  std::vector<glm::vec2> m_vtxWindowCoords;
  
};


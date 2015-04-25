/* 
 * File:   SEGrid.h
 * Author: rpb
 *
 * Created on April 25, 2015, 12:20 PM
 */

#pragma once

//SceneElement Grid
 
#include <vector>

#include "SceneElement.h"

#include <glm/glm.hpp>

class SEGrid: public SceneElement {
public:
  
  SEGrid( );
  virtual ~SEGrid( );
  
  void UpdateTransform( const glm::mat4& mat4Transform );
  
  virtual void Init( void );
  virtual void Paint( void );
  
protected:

private:

  GLuint m_idVertexArray;
  GLuint m_idUniformTransform;
  GLuint m_idWindowCoordsVertexBuffer;
  GLuint m_idElementBuffer;
  
  std::vector<glm::vec2> m_vCoords;
  std::vector<GLuint> m_vElements;
  
  glm::mat4 m_mat4Transform;
  glm::mat4 m_mat4Basic;
  
};


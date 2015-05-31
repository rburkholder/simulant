/* 
 * File:   SEGrid.h
 * Author: rpb
 *
 * Created on April 25, 2015, 12:20 PM
 */

#pragma once

//SceneElement Grid
 
#include <vector>

#include "SceneElementOpenGL.h"

#include <glm/glm.hpp>

class SEGrid: public SceneElementOpenGL {
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
  GLuint m_idVapWindowCoords;
  
  std::vector<glm::vec2> m_vCoords;
  std::vector<GLuint> m_vElements;
  
  glm::mat4 m_mat4Transform;
  glm::mat4 m_mat4Basic;
  
};


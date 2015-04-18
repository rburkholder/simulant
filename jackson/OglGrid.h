/* 
 * File:   OglGrid.h
 * Author: rpb
 *
 * Created on April 12, 2015, 10:33 PM
 */

#pragma once

/*
 * draw a grid for setting transformation matrix
 */

#include <vector>

#include "canvasOpenGL.h"

class OglGrid: public canvasOpenGL<OglGrid> {
  friend class canvasOpenGL<OglGrid>;
public:
  
  OglGrid( wxFrame* parent, int* args );
  virtual ~OglGrid();
  
  void UpdateTransform( const glm::mat4& mat4Transform );
  
protected:
private:
  
  GLuint m_idVertexArray;
  GLuint m_idUniformTransform;
  GLuint m_idWindowCoordsVertexBuffer;
  GLuint m_idElementBuffer;
  
  std::vector<glm::vec2> m_vCoords;
  std::vector<GLuint> m_vElements;
  
  glm::mat4 m_mat4Transform;
  
  void OnPaintInit();
  void OnPaint();
  void OnResize( int w, int h );
  
};



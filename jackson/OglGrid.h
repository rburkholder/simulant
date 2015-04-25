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
#include "SEGrid.h"

class OglGrid: public canvasOpenGL<OglGrid> {
  friend class canvasOpenGL<OglGrid>;
public:
  
  OglGrid( wxFrame* parent, int* args );
  virtual ~OglGrid();
  
  void UpdateTransform( const glm::mat4& mat4Transform );
  
protected:
private:
  
  SEGrid m_seGrid;
  
  void OnPaintInit();
  void OnPaint();
  void OnResize( int w, int h );
  
};



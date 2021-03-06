/* 
 * File:   tut1.h
 * Author: rpb
 *
 * Created on March 18, 2015, 11:12 PM
 */

#ifndef TUT1_H
#define	TUT1_H

#include "CanvasOpenGL.h"

class tut1: public CanvasOpenGL<tut1> {
public:
  
  tut1( wxFrame* parent, int* args ): CanvasOpenGL<tut1>( parent, args ) {};
  virtual ~tut1() {};
  
  void OnResize( int w, int h );
  void OnPaintInit();
  void OnPaint();
  
protected:
  void InitializeVertexBuffer();
private:
  
  GLuint m_boPosition;
  GLuint m_vao;

};

#endif	/* TUT1_H */


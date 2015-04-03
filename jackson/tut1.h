/* 
 * File:   tut1.h
 * Author: rpb
 *
 * Created on March 18, 2015, 11:12 PM
 */

#ifndef TUT1_H
#define	TUT1_H

#include "canvasOpenGL.h"

class tut1: public canvasOpenGL<tut1> {
public:
  
  tut1( wxFrame* parent, int* args ): canvasOpenGL<tut1>( parent, args ) {};
  virtual ~tut1() {};
  
  void OnPaintInit() {
    init();
  }
  void OnPaint() {
    display();
  }
  void OnResize( int w, int h ) {
    reshape( w, h );
  }
protected:
  GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile);
  GLuint CreateProgram(const std::vector<GLuint> &shaderList);
  void InitializeProgram();
  void InitializeVertexBuffer();
private:
  GLuint m_program;
  GLuint boPosition;
  GLuint m_vao;

  void display();
  void init();
  void reshape (int w, int h);

};

#endif	/* TUT1_H */


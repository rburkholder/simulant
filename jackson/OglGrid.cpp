/* 
 * File:   OglGrid.cpp
 * Author: rpb
 * 
 * Created on April 12, 2015, 10:35 PM
 */

#define GL_GLEXT_PROTOTYPES

#include "OglGrid.h"

#include <GL/glext.h>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39

void  callbackOglGrid(GLenum source,GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  " << message << std::endl;
}

OglGrid::OglGrid( wxFrame* parent, int* args ): canvasOpenGL<OglGrid>( parent, args ) {
}

OglGrid::~OglGrid() {
}

void OglGrid::UpdateTransform( const glm::mat4& mat4Transform ) {
  m_seGrid.UpdateTransform( mat4Transform );
}

void OglGrid::OnPaintInit() {
  
  //std::cout << "init start" << std::endl;

  //glDebugMessageCallback( &callbackOglGrid, 0 );

  m_seGrid.Init();  
  
  //glDebugMessageCallback( 0, 0 );
  
  //std::cout << "init end" << std::endl;
  
}

void OglGrid::OnPaint() {
  
  //std::cout << "display start" << std::endl;
  
  //glDebugMessageCallback( &callbackOglGrid, 0 );

  m_seGrid.Paint();

  //glDebugMessageCallback( 0, 0 );

  //std::cout << "display end" << std::endl;
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void OglGrid::OnResize (int w, int h) {
  //std::cout << "oglgrid resize " << w << ", " << h << std::endl;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

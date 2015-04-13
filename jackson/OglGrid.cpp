/* 
 * File:   OglGrid.cpp
 * Author: rpb
 * 
 * Created on April 12, 2015, 10:35 PM
 */

#define GL_GLEXT_PROTOTYPES

#include "OglGrid.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39

void  callbackOglGrid(GLenum source,GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  " << message << std::endl;
}

OglGrid::OglGrid( wxFrame* parent, int* args ): canvasOpenGL<OglGrid>( parent, args ) {
  
  const unsigned int nCols( 10 );
  const unsigned int nRows( 10 );
  
  unsigned int cnt( 0 );
  for ( unsigned int ix = 0; ix <= nCols; ++ix ) {
    m_vCoords.push_back( glm::vec2( ix, 0 ) );
    m_vElements.push_back( cnt++ );
    m_vCoords.push_back( glm::vec2( ix, nRows ) );
    m_vElements.push_back( cnt++ );
  }
  for ( unsigned int iy = 0; iy <= nRows; ++iy ) {
    m_vCoords.push_back( glm::vec2( 0, iy ) );
    m_vElements.push_back( cnt++ );
    m_vCoords.push_back( glm::vec2( nCols, iy ) );
    m_vElements.push_back( cnt++ );
  }

  glm::mat4 m_mat4Transform = glm::mat4( 1.0f ); // identity matrix
  m_mat4Transform *= glm::translate( glm::vec3( -1.0f, -1.0f, 0.0f ) );
  m_mat4Transform *= glm::scale( glm::vec3( 2.0f / nCols, 2.0f / nRows, 0.0f ) );

}

OglGrid::~OglGrid() {
}

void OglGrid::OnPaintInit() {
  
  std::cout << "init start" << std::endl;

  glDebugMessageCallback( &callbackOglGrid, 0 );
  
  std::string prefix( "../projects/simulant/jackson/" );
  CanvasBase::LoadShader( GL_VERTEX_SHADER, prefix + "oglGrid.shvert" );
  CanvasBase::LoadShader( GL_FRAGMENT_SHADER, prefix + "oglGrid.shfrag" );
	InitializeProgram();
  
  glDebugMessageCallback( 0, 0 );
  
  std::cout << "init end" << std::endl;
  
}

void OglGrid::OnPaint() {
  
  std::cout << "display start" << std::endl;
  
  glDebugMessageCallback( &callbackOglGrid, 0 );

  // vertex array object (VAO), an object that represents the
  // vertex fetch stage of the OpenGL pipeline and is used to supply input to
  // the vertex shader  (can go in startup)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glUseProgram(m_program);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbWindowCoords;  // vertices to be deprecated
  glGenBuffers(1, &vbWindowCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vbWindowCoords);
  
//  int s1 = sizeof( glm::vec2 ) * m_vtxWindowCoords.size();
//  int s2 = sizeof(vtxTextureCoords);
//  std::cout << "s values: " << s1 << ", " << s2 << std::endl;
//  glBufferData(GL_ARRAY_BUFFER, s1, &m_vtxWindowCoords[0], GL_STATIC_DRAW);  // copy vertices to opengl

  // Specify the layout of the vertex data
  GLint attribWindowCoords = glGetAttribLocation(m_program, "vWindowCoords");
  glEnableVertexAttribArray(attribWindowCoords);
  glVertexAttribPointer(attribWindowCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

  
  // **** need to pass in grid vertices
  // **** aspect ratio comes from screen coordinates and how they map to window coordinates (-1,-1,1,1)
  
//  GLuint elements[] = {  // natural order for 
//      0, 1, 2,
//      0, 2, 3
//  };
  
  // Create an element array
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vElements.size(), &(m_vElements[0]), GL_STATIC_DRAW);  // copy m_vElements to opengl

  GLint uniformTransform = glGetUniformLocation( m_program, "mTransform" );
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, &m_mat4Transform[0][0]);

  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw a rectangle from the 2 triangles using 6 indices
  glDrawElements(GL_LINES, m_vElements.size(), GL_UNSIGNED_INT, 0);

  // uncomment after testing and clean up
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbWindowCoords);

  glDeleteVertexArrays(1, &vao);

  glDebugMessageCallback( 0, 0 );

  std::cout << "display end" << std::endl;
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void OglGrid::OnResize (int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* 
 * File:   SEGrid.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 12:20 PM
 */

#define GL_GLEXT_PROTOTYPES

#include "SEGrid.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39

SEGrid::SEGrid( ): SceneElementOpenGL() {

  const GLuint nCols( 10 );  // but will n + 1, iterate 0 .. n
  const GLuint nRows( 10 );  // but will n + 1, iterate 0 .. n
  
  GLuint cnt( 0 );
  for ( GLuint ix = 0; ix <= nCols; ++ix ) {
    m_vCoords.push_back( glm::vec2( (float)ix, 0.0f ) );
    m_vElements.push_back( cnt++ );
    m_vCoords.push_back( glm::vec2( (float)ix, (float)nRows ) );
    m_vElements.push_back( cnt++ );
  }
  for ( GLuint iy = 0; iy <= nRows; ++iy ) {
    m_vCoords.push_back( glm::vec2( 0.0f, (float)iy ) );
    m_vElements.push_back( cnt++ );
    m_vCoords.push_back( glm::vec2( (float)nCols, (float)iy ) );
    m_vElements.push_back( cnt++ );
  }
  
  glm::vec3 vScaleTo2( 2.0f / nCols, 2.0f / nRows, 0.0f );
  glm::vec3 vTranslateAround0( -1.0f, -1.0f, 0.0f );
  glm::vec3 vScaleDownABit( 0.99f, 0.99f, 0.0f );

  m_mat4Basic = glm::mat4( 1.0f ); // identity matrix
  m_mat4Basic *= glm::scale( vScaleDownABit ); // gets all four sides into the window
  m_mat4Basic *= glm::translate( vTranslateAround0 ); // then translate to straddle zero
  m_mat4Basic *= glm::scale( vScaleTo2 );  // first scale to 0..2
  
  m_mat4Transform = m_mat4Basic;
  
}

SEGrid::~SEGrid( ) {
  if ( HadInit() ) {
    glDeleteBuffers(1, &m_idElementBuffer);
    glDeleteBuffers(1, &m_idWindowCoordsVertexBuffer);
    glDeleteVertexArrays(1, &m_idVertexArray);
  }
}

void SEGrid::UpdateTransform( const glm::mat4& mat4Transform ) {
  //m_mat4Transform *= mat4Transform;
  glm::vec3 vTranslateToCenter( -1.0f, -1.0f, 0.0f );
  glm::vec3 vTranslateFromCenter( 1.0f, 1.0f, 0.0f );
  //m_mat4Transform = m_mat4Basic * glm::translate( vTranslateFromCenter ) * mat4Transform * glm::translate( vTranslateToCenter );
  m_mat4Transform = mat4Transform * m_mat4Basic;
  //canvasOpenGL<OglGrid>::Refresh();
}

void SEGrid::Init( void ) {
  
  SceneElementOpenGL::Init();
  
  std::string prefix( "/home/rpb/projects/simulant/jackson/" );
  m_managerShader.LoadShader( GL_VERTEX_SHADER, prefix + "oglGrid.shvert" );
  m_managerShader.LoadShader( GL_FRAGMENT_SHADER, prefix + "oglGrid.shfrag" );
	m_managerShader.InitializeProgram( m_idProgram );
  
  glUseProgram(m_idProgram);
  
  // vertex array object (VAO), an object that represents the
  // vertex fetch stage of the OpenGL pipeline and is used to supply input to
  // the vertex shader  (can go in startup) -- page 20 of superbible
  glGenVertexArrays( 1, &m_idVertexArray );
  // can be used to switch between a bunch of different buffer collections
  // http://www.openglsuperbible.com/2013/12/09/vertex-array-performance/
  glBindVertexArray( m_idVertexArray );  

  m_idUniformTransform = glGetUniformLocation( m_idProgram, "mTransform" );
  
  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &m_idWindowCoordsVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_idWindowCoordsVertexBuffer); // works with vertex array object
  // notice this uses an implicit buffer id, need to figure out how this works
  int s1 = sizeof( glm::vec2 ) * m_vCoords.size();
  //int s2 = m_vElements.size();
  //std::cout << "s values: " << s1 << ", " << s2 << std::endl;
  glBufferData(GL_ARRAY_BUFFER, s1, &m_vCoords[0], GL_STATIC_DRAW);  // assumes just allocated buffer

  // Specify the layout of the vertex data - works with vertex array object
  m_idVapWindowCoords = glGetAttribLocation(m_idProgram, "vWindowCoords");  
  glVertexAttribPointer(m_idVapWindowCoords, 2, GL_FLOAT, GL_FALSE, 0, 0); // assumes GL_ARRAY_BUFFER, set in the VertexArrayObject
  //glEnableVertexAttribArray( m_idVapWindowCoords );
  glDisableVertexAttribArray( m_idVapWindowCoords ); // keep off until we require it
  
  // Create an element array
  glGenBuffers(1, &m_idElementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idElementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_vElements.size(), &(m_vElements[0]), GL_STATIC_DRAW);  // copy m_vElements to opengl
  
  glUseProgram(0);
  
}

void SEGrid::Paint( void ) {
  
  SceneElementOpenGL::Paint();
  
  glUseProgram(m_idProgram);
  glBindVertexArray( m_idVertexArray );  
  
  glEnableVertexAttribArray( m_idVapWindowCoords );

  // optimize this out, can we update only on change?  
  // may need to ensure we have the correct vertex array object selected
  glUniformMatrix4fv(m_idUniformTransform, 1, GL_FALSE, &m_mat4Transform[0][0]);

  // Draw the grid -- https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDrawElements.xml
  // OpenGL Programming Guide, 8e, v4.3, pg116:
  // ... causes vertices to be read from the enabled vertex-attribute arrays and used to construct primitives of the type
  // specified by mode. Vertex-attribute arrays are enabled using glEnableVertexAttribArray(). glDrawElements() uses the
  //indices in the element array buffer to index into the vertex attribute arrays.
  
  glDrawElements(GL_LINES, m_vElements.size(), GL_UNSIGNED_INT, 0);
  
  glDisableVertexAttribArray( m_idVapWindowCoords );
  
  glUseProgram(0);
  
}

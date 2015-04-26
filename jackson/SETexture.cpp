/* 
 * File:   SETexture.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 9:16 PM
 */

#define GL_GLEXT_PROTOTYPES

// https://open.gl/textures
// https://open.gl/content/code/c3_basic.txt

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/rawbmp.h>

#include "SETexture.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

SETexture::SETexture( ): SceneElement() {
  
  using namespace boost::assign;
  
  m_vtxWindowCoords +=
       glm::vec2( 0.0f,  0.0f ), // Bottom-left
       glm::vec2( 1.0f,  0.0f ), // Bottom-right
       glm::vec2( 1.0f,  1.0f ), // Top-right
       glm::vec2( 0.0f,  1.0f )  // Top-left
    ;
  
  m_vtxTextureCoords +=  // element order for inverted mapping to texture onto projection... maybe use projection matrix at some point
       glm::vec2( 0.0f,  1.0f ),  // Top-left
       glm::vec2( 1.0f,  1.0f ),  // Top-right
       glm::vec2( 1.0f,  0.0f ),  // Bottom-right
       glm::vec2( 0.0f,  0.0f )   // Bottom-left
    ;
  
  m_vElements += // natural order for 
      0, 1, 2,
      0, 2, 3
    ;
  
  m_mat4Transform = glm::mat4( 1.0f ); // identity matrix
  m_mat4Transform *= glm::translate( glm::vec3( -1.0f, 1.0f, 0.0f ) );  // translate to window coordinates
//  mat4Transform *= glm::scale( glm::vec3( 2.0f, -2.0f, 1.0f ) );  // invert image and expand to window coordinates
//  mat4Transform *= glm::translate( glm::vec3( 1.0f, 1.0f, 0.0f ) );  // translate to window coordinates
//  mat4Transform *= glm::scale( glm::vec3( 1.0f, 1.0f, 1.0f ) );  // invert image and expand to window coordinates

}

SETexture::~SETexture( ) {
  if ( HadInit() ) {
    glDeleteBuffers(1, &m_idElements);
    glDeleteBuffers(1, &m_idVertexBufferForWindowCoords);
    glDeleteBuffers(1, &m_idVertexBufferForTextureCoords);
    glDeleteTextures(1, &m_idTexture);
    glDeleteVertexArrays(1, &m_idVertexArray);
  }
}

void SETexture::SetWindowCoords( std::vector<glm::vec4>&  vCoords ) {
  m_vtxWindowCoords.clear();
  for ( size_t ix = 0; ix < vCoords.size(); ++ix ) {
    m_vtxWindowCoords.push_back( glm::vec2( vCoords[ix].x, vCoords[ix].y ) );
    std::cout << "coords set: "  << vCoords[ix].x << ", " << vCoords[ix].y << std::endl;
  }
}

void SETexture::Init( void ) {
  
  SceneElement::Init();

  boost::filesystem::path path;
  path = boost::filesystem::current_path();
  std::cout << "path: " << path << std::endl;
  
  std::string prefix( "../projects/simulant/jackson/" );
  m_managerShader.LoadShader( GL_VERTEX_SHADER, prefix + "tex2.shvert" );
  m_managerShader.LoadShader( GL_FRAGMENT_SHADER, prefix + "tex2.shfrag" );
	m_managerShader.InitializeProgram( m_idProgram );
  
  glUseProgram(m_idProgram);
  
  glGenVertexArrays(1, &m_idVertexArray);
  glBindVertexArray(m_idVertexArray);

  m_idUniformTransform = glGetUniformLocation( m_idProgram, "mTransform" );

  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &m_idVertexBufferForWindowCoords);
  glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferForWindowCoords);
  int s1 = sizeof( glm::vec2 ) * m_vtxWindowCoords.size();
  glBufferData(GL_ARRAY_BUFFER, s1, &m_vtxWindowCoords[0], GL_STATIC_DRAW);

  // Specify the layout of the vertex data
  m_idVapWindowCoords = glGetAttribLocation(m_idProgram, "vWindowCoords");
  glVertexAttribPointer(m_idVapWindowCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
  //glEnableVertexAttribArray(m_idVapWindowCoords);
  glDisableVertexAttribArray(m_idVapWindowCoords);

  glGenTextures(1, &m_idTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_idTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );//GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );//GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  wxImagePixelData data( *m_pImage );
  int width = data.GetWidth();
  int height = data.GetHeight();
  //int stride = data.GetRowStride();
  //wxSize size = data.GetSize();
  wxImagePixelData::Iterator pDest( data );
  
  // http://stackoverflow.com/questions/10918684/strange-color-shift-after-loading-a-gl-rgb-texture
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // GL doesn't like packed structures, used to get the RGB out
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pDest.m_pRGB );
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 10, 10, 0, GL_RGB, GL_UNSIGNED_BYTE, col );

  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &m_idVertexBufferForTextureCoords);
  glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferForTextureCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof( glm::vec2) * m_vtxTextureCoords.size(), &m_vtxTextureCoords[0], GL_STATIC_DRAW);

  // Specify the layout of the vertex data
  m_idVapTextureCoords = glGetAttribLocation(m_idProgram, "vTextureCoords");
  //glEnableVertexAttribArray(m_idVapTextureCoords);
  glVertexAttribPointer(m_idVapTextureCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glDisableVertexAttribArray(m_idVapTextureCoords);

  // Create an element array
  glGenBuffers(1, &m_idElements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idElements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_vElements.size(), &m_vElements[0], GL_STATIC_DRAW);  // copy elements to opengl

  glUseProgram(0);
}

void SETexture::Paint( void ) {
  
  SceneElement::Paint();
  
  glUseProgram(m_idProgram);
  glBindVertexArray(m_idVertexArray);
  
  glEnableVertexAttribArray(m_idVapWindowCoords);
  glEnableVertexAttribArray(m_idVapTextureCoords);

  glUniformMatrix4fv(m_idUniformTransform, 1, GL_FALSE, &m_mat4Transform[0][0]);

  // this is probably a problem as we need to identify the specific elements.
  // Draw a rectangle from the 2 triangles using 6 indices
  glDrawElements(GL_TRIANGLES, m_vElements.size(), GL_UNSIGNED_INT, 0);
  
  glDisableVertexAttribArray(m_idVapTextureCoords);
  glDisableVertexAttribArray(m_idVapWindowCoords);

  glUseProgram(0);
  
}

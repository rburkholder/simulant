/* 
 * File:   SceneElementOpenGL.cpp
 * Author: rpb
 * 
 * Created on May 31, 2015, 5:36 PM
 */

#define GL_GLEXT_PROTOTYPES

#include "SceneElementOpenGL.h"

#include <GL/glext.h>

SceneElementOpenGL::SceneElementOpenGL( const std::string& sPathForShaders ): 
SceneElement(), m_bHadInit( false ), m_idProgram( 0 ), m_sPathForShaders( sPathForShaders )
{
}

SceneElementOpenGL::~SceneElementOpenGL( ) {
  
  if ( GL_FALSE != glIsProgram( m_idProgram ) ) {
    glDeleteProgram( m_idProgram ); // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDeleteProgram.xml
  }
  m_idProgram = 0;
  
}


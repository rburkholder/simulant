/* 
 * File:   SceneElement.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 10:50 AM
 */

#define GL_GLEXT_PROTOTYPES

#include "SceneElement.h"

#include <GL/glext.h>

SceneElement::SceneElement( ): 
m_bHadInit( false ), m_idProgram( 0 ) 
{
}

SceneElement::~SceneElement( ) {
  
  if ( GL_FALSE != glIsProgram( m_idProgram ) ) {
    glDeleteProgram( m_idProgram ); // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDeleteProgram.xml
  }
  m_idProgram = 0;
  
}


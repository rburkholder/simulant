/* 
 * File:   CanvasBase.cpp
 * Author: rpb
 * 
 * Created on April 4, 2015, 10:55 PM
 */

#include <sstream>
#include <fstream>

#define GL_GLEXT_PROTOTYPES

#include "CanvasBase.h"

#include <GL/glext.h>

CanvasBase::CanvasBase(  wxFrame* parent, int* args  )
: wxGLCanvas( parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ), 
  m_idProgram( 0 )
{
}

// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDeleteProgram.xml
CanvasBase::~CanvasBase( void ) {
  if ( GL_FALSE != glIsProgram( m_idProgram ) ) {
    glDeleteProgram( m_idProgram ); // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDeleteProgram.xml
  }
  m_idProgram = 0;
};


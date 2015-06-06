/* 
 * File:   SceneElementOpenGL.h
 * Author: rpb
 *
 * Created on May 31, 2015, 5:36 PM
 */

#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "ShaderManager.h"

#include "SceneElement.h"

class SceneElementOpenGL: public SceneElement {
public:
  
  SceneElementOpenGL( );
  virtual ~SceneElementOpenGL( );
  
  virtual void Init( void ) { assert( false == m_bHadInit ); m_bHadInit = true; };
  virtual void Paint( void ) {};
  
  bool HadInit( void ) const { return m_bHadInit; }
  
  virtual void OnFrameTrigger( void ) {};
  
protected:
  
  GLuint m_idProgram;
  
  ShaderManager m_managerShader;
  
private:
  
  bool m_bHadInit;
  
};

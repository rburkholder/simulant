/* 
 * File:   SceneElement.h
 * Author: rpb
 *
 * Created on April 25, 2015, 10:50 AM
 * 
 * Fully fleshed scene elements inherit from this, and are then used in the SceneManager
 * 
 */

#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "ShaderManager.h"

class SceneElement {
public:
  
  SceneElement( );
  virtual ~SceneElement( );
  
  virtual void Init( void ) { assert( false == m_bHadInit ); m_bHadInit = true; };
  virtual void Paint( void ) {};
  
  bool HadInit( void ) { return m_bHadInit; }
  
protected:
  
  GLuint m_idProgram;
  
  ShaderManager m_managerShader;
  
private:
  
  bool m_bHadInit;
  
};


/* 
 * File:   CanvasBase.h
 * Author: rpb
 *
 * Created on April 4, 2015, 10:55 PM
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "ShaderManager.h"

#include <glm/glm.hpp>

class CanvasBase: public wxGLCanvas {   
  // so we can keep in regular vector, otherwise will need a fusion vector
  // basically the opengl viewport via the projector
  // rough rectangular area of an opengl canvas surface in the VisualRepresentation
  // in screen coords now, but should be relative coords so can use any screen area
  // and thus auto adjusts resolution based upon screen area available
  // ie, dual use:  the real projection, preview projections on controller
public:
  
  typedef boost::shared_ptr<CanvasBase> pCanvas_t;
  
  CanvasBase(  wxFrame* parent, int* args  );
  virtual ~CanvasBase( void );
  
  void SetName( const std::string& sName ) { m_sName = sName; }
  const std::string GetName( void ) { return m_sName; }
  
protected:
  
  GLuint m_idProgram;  // to be deprecated?
  
  ShaderManager m_managerShader;
  
private:
  
  std::string m_sName;
  
};

/* 
 * File:   CanvasBase.h
 * Author: rpb
 *
 * Created on April 4, 2015, 10:55 PM
 */

#ifndef CANVASBASE_H
#define	CANVASBASE_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/glcanvas.h>

#define GL_GLEXT_PROTOTYPES

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

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
  
  // GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
  void AddShader( GLenum eShaderType, const std::string& strShaderCode );
  void LoadShader( GLenum eShaderType, const std::string& strShaderFileName );
  void InitializeProgram();
  
protected:
  
  typedef std::vector<GLuint> vShader_t;
  
  vShader_t m_vShader;
  
  GLuint m_idProgram;
  
private:
  
  bool m_bProgramCreated;
  
  std::string m_sName;
  
  GLuint CreateShader( GLenum eShaderType, const std::string& strShaderCode );
  bool CreateProgram( const vShader_t& shaderList, GLuint& program );
  
};

#endif	/* CANVASBASE_H */


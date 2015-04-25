/* 
 * File:   ShaderManager.h
 * Author: rpb
 *
 * Created on April 25, 2015, 12:48 PM
 */

#pragma once

#include <vector>
#include <string>

#define GL_GLEXT_PROTOTYPES

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

class ShaderManager {
public:
  
  ShaderManager( );
  virtual ~ShaderManager( );
  
  // GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
  void AddShader( GLenum eShaderType, const std::string& strShaderCode );
  void LoadShader( GLenum eShaderType, const std::string& strShaderFileName );
  void InitializeProgram( GLuint& idProgram );
  
private:

  typedef std::vector<GLuint> vShader_t;
  
  vShader_t m_vShader;
  
  GLuint CreateShader( GLenum eShaderType, const std::string& strShaderCode );
  bool CreateProgram( const vShader_t& shaderList, GLuint& program );
  
};


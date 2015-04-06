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
  m_bProgramCreated( false )
{
}

CanvasBase::~CanvasBase( void ) {
  if (m_bProgramCreated) glDeleteProgram(m_program);
  m_bProgramCreated = false;
};


GLuint CanvasBase::CreateShader( GLenum eShaderType, const std::string &strShaderCode ) {
  
	GLuint shader = glCreateShader(eShaderType);
	const char* strFileData = strShaderCode.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType) {
      case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
      case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
      case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		//fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
    std::cout << "Compile failure in " << strShaderType << " shader: " << strInfoLog << std::endl;
		delete[] strInfoLog;
	}

	return shader;
}

bool CanvasBase::CreateProgram( const vShader_t& vShader, GLuint& program ) {
  
  bool bOk( true );
  
	program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < vShader.size(); iLoop++)
		glAttachShader(program, vShader[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
    
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		//fprintf(stderr, "Linker failure: %s\n", strInfoLog);
    std::cout << "Linker failure: " << strInfoLog << std::endl;
		delete[] strInfoLog;
    
    bOk = false;
	}

	for(size_t iLoop = 0; iLoop < vShader.size(); iLoop++)
		glDetachShader(program, vShader[iLoop]);

	return bOk;
}

void CanvasBase::InitializeProgram() {
  
  if ( m_bProgramCreated = CreateProgram( m_vShader, m_program ) ) {
  };
  
  std::for_each(m_vShader.begin(), m_vShader.end(), glDeleteShader);
  m_vShader.clear();
}

void CanvasBase::AddShader( GLenum eShaderType, const std::string& strShaderCode ) {
  
  m_vShader.push_back( CreateShader( eShaderType, strShaderCode ) );
  
}

// http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
void CanvasBase::LoadShader( GLenum eShaderType, const std::string& strShaderFileName ) {
  
  std::ifstream inFile;
  inFile.open(strShaderFileName);//open the input file
  
  if ( inFile.fail() ) 
    throw std::runtime_error( "can't open " + strShaderFileName );

  std::stringstream ss;
  ss << inFile.rdbuf();//read the file
  std::string strShaderCode = ss.str();//str holds the content of the file
  
  AddShader( eShaderType, strShaderCode );

}

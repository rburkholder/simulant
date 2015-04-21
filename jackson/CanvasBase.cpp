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


GLuint CanvasBase::CreateShader( GLenum eShaderType, const std::string &strShaderCode ) {
  
	GLuint idShader = glCreateShader(eShaderType);
	const char* strFileData = strShaderCode.c_str();
	glShaderSource(idShader, 1, &strFileData, NULL); // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glShaderSource.xml
	glCompileShader(idShader); // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCompileShader.xml

	GLint status;
	glGetShaderiv(idShader, GL_COMPILE_STATUS, &status);
  
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(idShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(idShader, infoLogLength, NULL, strInfoLog);

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

	return idShader;
}

bool CanvasBase::CreateProgram( const vShader_t& vShader, GLuint& idProgram ) {
  
  bool bOk( true );
  
	idProgram = glCreateProgram();  // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCreateProgram.xml

	for( vShader_t::const_iterator iter = vShader.begin(); vShader.end() != iter; ++iter )
		glAttachShader(idProgram, *iter);  // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glAttachShader.xml

	glLinkProgram( idProgram );  // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glLinkProgram.xml -- helpful

	GLint status;
	glGetProgramiv( idProgram, GL_LINK_STATUS, &status );
	if (status == GL_FALSE) {
    
		GLint infoLogLength;
		glGetProgramiv(idProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(idProgram, infoLogLength, NULL, strInfoLog);
		//fprintf(stderr, "Linker failure: %s\n", strInfoLog);
    std::cout << "Linker info: " << strInfoLog << std::endl;
		delete[] strInfoLog;
    
    bOk = false;
	}

	for( vShader_t::const_iterator iter = vShader.begin(); vShader.end() != iter; ++iter ) {
		glDetachShader(idProgram, *iter);  // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDetachShader.xml
    glDeleteShader( *iter );
  }

	return bOk;
}

void CanvasBase::InitializeProgram() {
  
  if ( !CreateProgram( m_vShader, m_idProgram ) ) {  // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glCreateProgram.xml
//    assert( false );  // need to fix this
  };
  
//  std::for_each(m_vShader.begin(), m_vShader.end(), glDeleteShader);
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

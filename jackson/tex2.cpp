
#define GL_GLEXT_PROTOTYPES

//#ifdef __WXMAC__
//#include "OpenGL/glu.h"
//#include "OpenGL/gl.h"
//#else
//#include <GL/glu.h>
//#include <GL/gl.h>
//#endif

//#include <SOIL/SOIL.h>

#include "tex2.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39

void  callbackTex2(GLenum source,GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  " << message << std::endl;
}

tex2::tex2( wxFrame* parent, int* args ): CanvasOpenGL<tex2>( parent, args ) {
}

tex2::~tex2() {
}

void tex2::OnPaintInit() {
  
  std::cout << "tex2 init start" << std::endl;

  glDebugMessageCallback( &callbackTex2, 0 );
  
  m_seTexture.Init();
  
  std::cout << "tex2 init linked" << std::endl;
  
  glDebugMessageCallback( 0, 0 );
  
  std::cout << "tex2 init end" << std::endl;
  
}

void tex2::OnPaint() {
  
  std::cout << "display start" << std::endl;
  
  glDebugMessageCallback( &callbackTex2, 0 );
  
  m_seTexture.Paint();

  glDebugMessageCallback( 0, 0 );

  std::cout << "display end" << std::endl;
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void tex2::OnResize (int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

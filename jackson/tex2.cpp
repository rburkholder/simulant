
// https://open.gl/textures
// https://open.gl/content/code/c3_basic.txt

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/rawbmp.h>

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

// Shader sources
//const GLchar* sourceVertex = "\
//  \
//  #version 430 core \n \
//  layout(location=0) in vec2 vWindowCoords; \
//  layout(location=1) in vec2 vTextureCoords; \
//  uniform mat4 mTransform; \
//  out vec2 vUV; \
//  void main(void) { \
//    gl_Position = vec4(vWindowCoords,0,1); \
//    vUV = vTextureCoords; \
//  } \
//  ";
//    gl_Position = vec4(vVertex*2.0-1,0,1); \
//    gl_Position = mTransform * vec4(vWindowCoords,0,1);
//    vUV = vVertex; \

//const GLchar* sourceFragment = "\
//  \
//#version 430 core \n \
//in vec2 vUV; \
//layout (location=0) out vec3 vFragColor; \
//uniform sampler2D textureMap; \
//\
//void main(void) { \
//  vFragColor = texture(textureMap, vUV).rgb; \
//}  \
//";

// layout (binding=0) uniform sampler2D textureMap; \

void  callback(GLenum source,GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  " << message << std::endl;
}

tex2::tex2( wxFrame* parent, int* args ): canvasOpenGL<tex2>( parent, args ), m_pImage( 0 ) {
  using namespace boost::assign;
  m_vtxWindowCoords +=
       glm::vec2( 0.0f,  0.0f ), // Bottom-left
       glm::vec2( 1.0f,  0.0f ), // Bottom-right
       glm::vec2( 1.0f,  1.0f ), // Top-right
       glm::vec2( 0.0f,  1.0f )  // Top-left
    ;
}

tex2::~tex2() {
}

void tex2::SetWindowCoords( std::vector<glm::vec4>&  vCoords ) {
  m_vtxWindowCoords.clear();
  for ( size_t ix = 0; ix < vCoords.size(); ++ix ) {
    m_vtxWindowCoords.push_back( glm::vec2( vCoords[ix].x, vCoords[ix].y ) );
    std::cout << "coords set: "  << vCoords[ix].x << ", " << vCoords[ix].y << std::endl;
  }
}

void tex2::OnPaintInit() {
  
  std::cout << "init start" << std::endl;

  glDebugMessageCallback( &callback, 0 );
  
  boost::filesystem::path path;
  path = boost::filesystem::current_path();
  std::cout << "path: " << path << std::endl;
  
  std::string prefix( "../projects/simulant/jackson/" );
  CanvasBase::LoadShader( GL_VERTEX_SHADER, prefix + "tex2.shvert" );
  CanvasBase::LoadShader( GL_FRAGMENT_SHADER, prefix + "tex2.shfrag" );
	InitializeProgram();
  
  std::cout << "init linked" << std::endl;
  
  glDebugMessageCallback( 0, 0 );
  
  std::cout << "init end" << std::endl;
  
}

void tex2::OnPaint() {
  
  std::cout << "display start" << std::endl;
  
  glDebugMessageCallback( &callback, 0 );

  wxImagePixelData data( *m_pImage );
  int width = data.GetWidth();
  int height = data.GetHeight();
  int stride = data.GetRowStride();
  wxSize size = data.GetSize();
  wxImagePixelData::Iterator pDest( data );
  
  // pg 148 texture coordinates are 0.0 - 1.0
  
  // vertex array object (VAO), an object that represents the
  // vertex fetch stage of the OpenGL pipeline and is used to supply input to
  // the vertex shader  (can go in startup)
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glm::mat4 mat4Transform = glm::mat4( 1.0f ); // identity matrix
  mat4Transform *= glm::translate( glm::vec3( -1.0f, 1.0f, 0.0f ) );  // translate to window coordinates
//  mat4Transform *= glm::scale( glm::vec3( 2.0f, -2.0f, 1.0f ) );  // invert image and expand to window coordinates
//  mat4Transform *= glm::translate( glm::vec3( 1.0f, 1.0f, 0.0f ) );  // translate to window coordinates
//  mat4Transform *= glm::scale( glm::vec3( 1.0f, 1.0f, 1.0f ) );  // invert image and expand to window coordinates

  glUseProgram(m_program);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbWindowCoords;  // vertices to be deprecated
  glGenBuffers(1, &vbWindowCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vbWindowCoords);
  int s1 = sizeof( glm::vec2 ) * m_vtxWindowCoords.size();
//  int s2 = sizeof(vtxTextureCoords);
//  std::cout << "s values: " << s1 << ", " << s2 << std::endl;
  glBufferData(GL_ARRAY_BUFFER, s1, &m_vtxWindowCoords[0], GL_STATIC_DRAW);  // copy vertices to opengl

  // Specify the layout of the vertex data
  GLint attribWindowCoords = glGetAttribLocation(m_program, "vWindowCoords");
  glEnableVertexAttribArray(attribWindowCoords);
  glVertexAttribPointer(attribWindowCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenTextures(1, &m_texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );//GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );//GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // http://stackoverflow.com/questions/10918684/strange-color-shift-after-loading-a-gl-rgb-texture
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // GL doesn't like packed structures, used to get the RGB out
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pDest.m_pRGB );
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 10, 10, 0, GL_RGB, GL_UNSIGNED_BYTE, col );

  glm::vec2 vtxTextureCoords[] = { // element order for inverted mapping to texture onto projection... maybe use projection matrix at some point
       glm::vec2( 0.0f,  1.0f ),  // Top-left
       glm::vec2( 1.0f,  1.0f ),  // Top-right
       glm::vec2( 1.0f,  0.0f ),  // Bottom-right
       glm::vec2( 0.0f,  0.0f ),  // Bottom-left
  };
  
  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbTextureCoords;  // vertices to be deprecated
  glGenBuffers(1, &vbTextureCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vbTextureCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtxTextureCoords), vtxTextureCoords, GL_STATIC_DRAW);  // copy vertices to opengl

  GLuint elements[] = {  // natural order for 
      0, 1, 2,
      0, 2, 3
  };
  
  // Create an element array
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);  // copy elements to opengl

  // Specify the layout of the vertex data
  GLint attribTextureCoords = glGetAttribLocation(m_program, "vTextureCoords");
  glEnableVertexAttribArray(attribTextureCoords);
  glVertexAttribPointer(attribTextureCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

  GLint uniformTransform = glGetUniformLocation( m_program, "mTransform" );
  glUniformMatrix4fv(uniformTransform, 1, GL_FALSE, &mat4Transform[0][0]);

  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw a rectangle from the 2 triangles using 6 indices
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDeleteTextures(1, &m_texture);
        
  // uncomment after testing and clean up
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbWindowCoords);
  glDeleteBuffers(1, &vbTextureCoords);

  glDeleteVertexArrays(1, &vao);

  //SOIL_free_image_data(pData);        

  glDebugMessageCallback( 0, 0 );

  std::cout << "display end" << std::endl;
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void tex2::OnResize (int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* 
 * File:   SETexture.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 9:16 PM
 */

#define GL_GLEXT_PROTOTYPES

// https://open.gl/textures
// https://open.gl/content/code/c3_basic.txt

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/rawbmp.h>

#include "SETexture.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "AspectRatio.h"

SETexture::SETexture( ): 
  SceneElementOpenGL(), 
  m_idVertexArray( 0 ), m_idTexture( 0 ), m_idElements( 0 ), 
  m_idVertexBufferForImageCoords( 0 ), m_idVertexBufferForTextureCoords( 0 ), 
  m_idVapImageCoords( 0 ), m_idVapTextureCoords( 0 ), 
  m_idUniformTransform( 0 ), 
  m_bNewImageAvailable( false ), m_bNewRawImageAvailable( false )
{
  
  using namespace boost::assign;
  
  m_vtxImageCoords +=  // default full window
       glm::vec2( -1.0f,  1.0f ), // Top-left
       glm::vec2(  1.0f,  1.0f ), // Top-right
       glm::vec2(  1.0f, -1.0f ), // Bottom-right
       glm::vec2( -1.0f, -1.0f )  // Bottom-left
    ;
  
  m_vtxTextureCoords +=  // element order for inverted mapping to texture onto projection... maybe use projection matrix at some point
       glm::vec2( 0.0f,  1.0f ),  // Top-left
       glm::vec2( 1.0f,  1.0f ),  // Top-right
       glm::vec2( 1.0f,  0.0f ),  // Bottom-right
       glm::vec2( 0.0f,  0.0f )   // Bottom-left
    ;
  
  m_vElements += // natural order for two triangles from four vertices
      0, 1, 2,
      0, 2, 3
    ;
  
  m_mat4BasicTransform = glm::mat4( 1.0f );
  m_mat4BasicTransform *= glm::scale( glm::vec3( 1.0f, -1.0f, 1.0f ) );  // invert image, based upon default coords
  
  SetTransform( glm::mat4( 1.0f ) ); // identity matrix

}

SETexture::~SETexture( ) {
  if ( HadInit() ) {
    // m_idVapWindowCoords ?
    // m_idVapTextureCoords ?
    glDeleteBuffers(1, &m_idElements);
    glDeleteBuffers(1, &m_idVertexBufferForImageCoords);
    glDeleteBuffers(1, &m_idVertexBufferForTextureCoords);
    glDeleteTextures(1, &m_idTexture);
    glDeleteVertexArrays(1, &m_idVertexArray);
  }
}

boost::signals2::connection SETexture::Connect(const slotFrame_t& slot ) {
  return m_signal.connect( slot );
}

void SETexture::SetBasicTransform( void ) {
  
  bool bImageAvailable( false );
  float height( 0.0 );
  float width( 0.0 );
  
  if ( 0 != m_pImage.use_count() ) {
    height = m_pImage->GetHeight();
    width = m_pImage->GetWidth();
    bImageAvailable = true;
  }
    
  if ( 0 != m_pRawImage.use_count() ) {
    height = m_pRawImage->GetHeight();
    width = m_pRawImage->GetWidth();
    bImageAvailable = true;
  }
    
  if ( bImageAvailable ) {
    m_vtxImageCoords.clear();
    using namespace boost::assign;
    m_vtxImageCoords +=  // regular pixel screen coordinates
       glm::vec2(         0.0f,          0.0f ), // Top-left 
       glm::vec2( width - 1.0f,          0.0f ), // Top-right
       glm::vec2( width - 1.0f, height - 1.0f ), // Bottom-right
       glm::vec2(         0.0f, height - 1.0f )  // Bottom-left
    ;
    
    glm::mat4 mat4AspectRatio = AspectRatioImage( height, width );
    
    m_mat4BasicTransform = glm::mat4( 1.0f );
    m_mat4BasicTransform *= mat4AspectRatio;
    m_mat4BasicTransform *= glm::scale( glm::vec3( 2.0f, 2.0f, 0.0f ) ); // scale to window coordinates
    m_mat4BasicTransform *= glm::translate( glm::vec3( -0.5f, -0.5f, 0.0f ) ); // shift to center over 0,0
    m_mat4BasicTransform *= glm::scale( glm::vec3( 1.0f / width, 1.0f / height, 1.0 ) );  // normalize
    //m_mat4BasicTransform *= glm::translate( glm::vec3( 0.0f, (float)height, 0.0f ) ); // shift in to positive again
    //m_mat4BasicTransform *= glm::scale( glm::vec3( 1.0f, -1.0f, 1.0f ) );  // invert the image
    
    //m_mat4BasicTransform = mat4Transform;
  }
}

void SETexture::SetTransform( const glm::mat4& mat4Transform ) { 
  m_mat4SuppliedTransform = mat4Transform; 
  m_mat4FinalTransform = m_mat4SuppliedTransform * m_mat4BasicTransform;
}

void SETexture::SetImage( pImage_t pImage ) {
  
  assert( 0 != pImage.use_count() );
  m_pImage = pImage;
  SetBasicTransform();
  m_bNewImageAvailable = true;
  
}

void SETexture::SetImage( pRawImage_t pRawImage ) {
  
  assert( 0 != pRawImage.use_count() );
  m_pRawImage = pRawImage;
  SetBasicTransform();
  m_bNewRawImageAvailable = true;
  
}

void SETexture::SetImage( void ) {
  
  bool bSetImageCoords( false );
  
  if ( m_bNewImageAvailable ) {
    assert( 0 != m_pImage.use_count() );
    GLboolean b;
    b = glIsTexture( m_idTexture );
    if ( GL_TRUE == b ) {
      AssignImageToTexture();
      //std::cout << "assigned to idTexture " << m_idTexture << std::endl;
    }
    m_bNewImageAvailable = false;
    bSetImageCoords = true;
  }
  
  if ( m_bNewRawImageAvailable ) {
    assert( 0 != m_pRawImage.use_count() );
    GLboolean b;
    b = glIsTexture( m_idTexture );
    if ( GL_TRUE == b ) {
      AssignRawImageToTexture();
      //std::cout << "assigned to idTexture " << m_idTexture << std::endl;
    }
    m_bNewRawImageAvailable = false;
    bSetImageCoords = true;
  }
  
  if ( bSetImageCoords ) {
    GLboolean b;
    b = glIsBuffer( m_idVertexBufferForImageCoords );
    if ( GL_TRUE == b ) {
      SetImageCoords();
      //std::cout << "assigned to idVertex " << m_idVertexBufferForImageCoords << std::endl;
    }
  }
  
}

void SETexture::AssignImageToTexture( void ) {
  
  if ( 0 != m_pImage.use_count() ) {
    wxImagePixelData data( *m_pImage );
    int width = data.GetWidth();
    int height = data.GetHeight();
    //int stride = data.GetRowStride();
    //wxSize size = data.GetSize();
    wxImagePixelData::Iterator pDest( data );

    // https://www.opengl.org/wiki/Common_Mistakes
    // http://stackoverflow.com/questions/10918684/strange-color-shift-after-loading-a-gl-rgb-texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // GL doesn't like packed structures, used to get the RGB out
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pDest.m_pRGB );
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 10, 10, 0, GL_RGB, GL_UNSIGNED_BYTE, col );
  }
  
}
  
void SETexture::AssignRawImageToTexture( void ) {
  
  if ( 0 != m_pRawImage.use_count() ) {
    int width = m_pRawImage->GetWidth();
    int height = m_pRawImage->GetHeight();

    // https://www.opengl.org/wiki/Common_Mistakes
    // http://stackoverflow.com/questions/10918684/strange-color-shift-after-loading-a-gl-rgb-texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // GL doesn't like packed structures, used to get the RGB out
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_pRawImage->GetBuffer() );
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 10, 10, 0, GL_RGB, GL_UNSIGNED_BYTE, col );
  }
  
}
  
void SETexture::AddTexture( void ) {
  
  glGenTextures(1, &m_idTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_idTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );//GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );//GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  if ( 0 != m_pImage.use_count() ) AssignImageToTexture();
  if ( 0 != m_pRawImage.use_count() ) AssignRawImageToTexture();

}

void SETexture::SetWindowCoords( std::vector<glm::vec4>&  vCoords ) {
  assert( 4 == vCoords.size() );
  m_vtxImageCoords.clear();
  for ( size_t ix = 0; ix < vCoords.size(); ++ix ) {
    m_vtxImageCoords.push_back( glm::vec2( vCoords[ix].x, vCoords[ix].y ) );
    std::cout << "coords set: "  << vCoords[ix].x << ", " << vCoords[ix].y << std::endl;
  }
}

void SETexture::SetImageCoords( void ) {
  GLboolean b = glIsBuffer( m_idVertexBufferForImageCoords );
  if ( GL_FALSE == b ) {
    glGenBuffers(1, &m_idVertexBufferForImageCoords);  
  }
  glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferForImageCoords);
  int s1 = sizeof( glm::vec2 ) * m_vtxImageCoords.size();
  glBufferData(GL_ARRAY_BUFFER, s1, &m_vtxImageCoords[0], GL_STATIC_DRAW);
}

void SETexture::Init( void ) {
  
  SceneElementOpenGL::Init();

  boost::filesystem::path path;
  path = boost::filesystem::current_path();
  std::cout << "path: " << path << std::endl;
  
  std::string prefix( "/home/rpb/projects/simulant/jackson/" );
  m_managerShader.LoadShader( GL_VERTEX_SHADER, prefix + "tex2.shvert" );
  m_managerShader.LoadShader( GL_FRAGMENT_SHADER, prefix + "tex2.shfrag" );
	m_managerShader.InitializeProgram( m_idProgram );
  
  glUseProgram(m_idProgram);
  
  // state management
  glGenVertexArrays(1, &m_idVertexArray);
  glBindVertexArray(m_idVertexArray);

  m_idUniformTransform = glGetUniformLocation( m_idProgram, "mTransform" );

  // Create a Vertex Buffer Object and copy the vertex data to it
  SetImageCoords();

  // Specify the layout of the vertex data
  m_idVapImageCoords = glGetAttribLocation(m_idProgram, "vImageCoords");
  glVertexAttribPointer(m_idVapImageCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glDisableVertexAttribArray(m_idVapImageCoords);

  // Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &m_idVertexBufferForTextureCoords);
  glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferForTextureCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof( glm::vec2) * m_vtxTextureCoords.size(), &m_vtxTextureCoords[0], GL_STATIC_DRAW);

  // Specify the layout of the vertex data
  m_idVapTextureCoords = glGetAttribLocation(m_idProgram, "vTextureCoords");
  glVertexAttribPointer(m_idVapTextureCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glDisableVertexAttribArray(m_idVapTextureCoords);
  
  // http://gamedev.stackexchange.com/questions/13794/how-to-render-a-texture-partly-transparent
  // http://stackoverflow.com/questions/721705/how-do-i-set-the-opacity-of-a-vertex-in-opengl
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  AddTexture();
  
  // Create an element array - uses arrays enabled via glEnableVertexAttribArray
  glGenBuffers(1, &m_idElements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idElements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_vElements.size(), &m_vElements[0], GL_STATIC_DRAW);  // copy elements to opengl

  glUseProgram(0);
}

void SETexture::Paint( void ) {
  
  SceneElementOpenGL::Paint();
  
//  if ( 0 != m_pImage.use_count() ) {
    if ( ( 0 != m_pImage.use_count() ) || ( 0 != m_pRawImage.use_count() ) ) {
  
    glUseProgram(m_idProgram);

    glBindVertexArray(m_idVertexArray);

    glEnableVertexAttribArray(m_idVapImageCoords);
    glEnableVertexAttribArray(m_idVapTextureCoords);
    
    glBindTexture(GL_TEXTURE_2D, m_idTexture);
    glActiveTexture(GL_TEXTURE0);

    SetImage();

    glUniformMatrix4fv(m_idUniformTransform, 1, GL_FALSE, &m_mat4FinalTransform[0][0]);

    glDrawElements(GL_TRIANGLES, m_vElements.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(m_idVapTextureCoords);
    glDisableVertexAttribArray(m_idVapImageCoords);

    glUseProgram(0);

  }
}

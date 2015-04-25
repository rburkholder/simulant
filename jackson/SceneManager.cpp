/* 
 * File:   SceneManager.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 10:28 AM
 */

#include <boost/foreach.hpp>

#include "SceneElement.h"
#include "SceneManager.h"

//#include <GL/glext.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39
// **** aspect ratio comes from screen coordinates and how they map to window coordinates (-1,-1,1,1)

void  callbackSceneManager(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  " << message << std::endl;
}

SceneManager::SceneManager( wxFrame* parent, int* args ): canvasOpenGL<SceneManager>( parent, args ) {
}

SceneManager::~SceneManager( ) {
}

void SceneManager::Add( pSceneElement_t pSceneElement ) {
  m_vSceneElement.push_back( pSceneElement );
}

void SceneManager::OnPaintInit( void ) {
  std::cout << "init start" << std::endl;

  //glDebugMessageCallback( &callbackOglGrid, 0 );
  
  // stuff goes here
  BOOST_FOREACH( pSceneElement_t pElement, m_vSceneElement ) {
    pElement->Init();
  }
  
  //glDebugMessageCallback( 0, 0 );
  
  std::cout << "init end" << std::endl;
}

void SceneManager::OnPaint( void ) {
  
  //std::cout << "display start" << std::endl;
  
  //glDebugMessageCallback( &callbackOglGrid, 0 );
  
  // stuff goes here
  BOOST_FOREACH( pSceneElement_t pElement, m_vSceneElement ) {
    pElement->Paint();
  }

  //glDebugMessageCallback( 0, 0 );

  //std::cout << "display end" << std::endl;
}

void SceneManager::OnResize( int w, int h ) {
  //std::cout << "oglgrid resize " << w << ", " << h << std::endl;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

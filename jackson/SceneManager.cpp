/* 
 * File:   SceneManager.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 10:28 AM
 */

#include <boost/foreach.hpp>

#define GL_GLEXT_PROTOTYPES

#include "SceneElement.h"
#include "SceneManager.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39
// **** aspect ratio comes from screen coordinates and how they map to window coordinates (-1,-1,1,1)

void  callbackSceneManager(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  (" << userParam << ") " << message << std::endl;
}

SceneManager::SceneManager( wxFrame* parent, int* args ): canvasOpenGL<SceneManager>( parent, args ), m_cntMapSceneElement( 0 ) {
}

SceneManager::~SceneManager( ) {
}

SceneManager::key_t SceneManager::Add( pSceneElement_t pSceneElement ) {
  key_t key( ++m_cntMapSceneElement ); 
  m_mapSceneElement.insert( mapSceneElement_t::value_type( key, pSceneElement ) );
  if ( Initialized() ) {
    pSceneElement->Init();
  }
  return key;
}

void SceneManager::Delete( key_t key ) {
  mapSceneElement_t::const_iterator iter = m_mapSceneElement.find( key );
  if ( m_mapSceneElement.end() == iter ) {
    throw std::runtime_error( "key not found" );
  }
  else {
    m_mapSceneElement.erase( iter );
  }
}

void SceneManager::OnPaintInit( void ) {
  
  std::cout << "init start" << std::endl;
  
  // stuff goes here
  BOOST_FOREACH( mapSceneElement_t::value_type element, m_mapSceneElement ) {
    glDebugMessageCallback( &callbackSceneManager, (const void*) element.first );
    element.second->Init();
    glDebugMessageCallback( 0, 0 );
  }
  
  std::cout << "init end" << std::endl;
}

void SceneManager::OnPaint( void ) {
  
  //std::cout << "paint start" << std::endl;
  
  // stuff goes here
  
  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  BOOST_FOREACH( mapSceneElement_t::value_type element, m_mapSceneElement ) {
    glDebugMessageCallback( &callbackSceneManager, (const void*) element.first );
    element.second->Paint();
    glDebugMessageCallback( 0, 0 );
  }

  //std::cout << "paint end" << std::endl;
}

void SceneManager::OnResize( int w, int h ) {
  //std::cout << "resize " << w << ", " << h << std::endl;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* 
 * File:   SceneManager.cpp
 * Author: rpb
 * 
 * Created on April 25, 2015, 10:28 AM
 */

//#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <boost/chrono/chrono_io.hpp>

#include <boost/foreach.hpp>

#define GL_GLEXT_PROTOTYPES

#include "FpsGenerator.h"

#include "SceneElement.h"
#include "SceneManager.h"

#include <GL/glext.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>

// NDS Normalized Device Space: x,y: -1.0,-1.0 -> 1.0, 1.0 ; depth: 0.0 -> 1.0  pg 39
// **** aspect ratio comes from screen coordinates and how they map to window coordinates (-1,-1,1,1)

FpsGenerator fpsGenerator;  // generate signals for frame rate control

void  callbackSceneManager(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  std::cout << "gl error:  (" << userParam << ") " << message << std::endl;
}

SceneManager::SceneManager( wxFrame* parent, int* args ): 
  m_bActive( false ), 
  canvasOpenGL<SceneManager>( parent, args ), m_cntMapSceneElement( 0 ) 
{
  wxApp::GetInstance()->Bind( EVENT_GENERATEFRAME, &SceneManager::HandleRefresh, this ); 
  //m_pScreenFrame->GetFrame()->Bind( EVENT_GENERATEFRAME, &TreeItemCanvasGrid::HandleRefresh, this );  // doesn't propagate properly
  
  m_bActive = true;
  
}

SceneManager::~SceneManager( ) {
  wxApp::GetInstance()->Unbind( EVENT_GENERATEFRAME, &SceneManager::HandleRefresh, this );
  m_bActive = false;
}

SceneManager::key_t SceneManager::Add( FPS fps, pSceneElement_t pSceneElement ) {
  
  key_t key( ++m_cntMapSceneElement ); 
  m_mapSceneElement.insert( mapSceneElement_t::value_type( key, SceneDescription_t( fps, pSceneElement ) ) );
  m_vSceneElementToInit.push_back( key );
  
  mapSceneElementsToRefreshAtFps_t::iterator iter = m_mapSceneElementsToRefreshAtFps.find( fps );
  std::pair<mapSceneElementsToRefresh_t::iterator, bool> result2;
  mapSceneElementsToRefresh_t::value_type entry( key, pSceneElement );
  
  mapSceneElementsToRefresh_t::size_type nElements( 0 );
  
  if ( m_mapSceneElementsToRefreshAtFps.end() == iter ) {
    std::pair<mapSceneElementsToRefreshAtFps_t::iterator, bool> result1;
    mapSceneElementsToRefreshAtFps_t::value_type vt( fps, FpsRelatedRefresh_t() );
    result1 = m_mapSceneElementsToRefreshAtFps.insert( vt );
    assert( true == result1.second );
    iter = result1.first;
    
    result2 = result1.first->second.mapSceneElementsToRefresh.insert( entry );
    nElements = result1.first->second.mapSceneElementsToRefresh.size();
  }
  else {
    result2 = iter->second.mapSceneElementsToRefresh.insert( entry );
    nElements = iter->second.mapSceneElementsToRefresh.size();
  }
  assert( true == result2.second );
  if ( 1 == nElements ) {
    namespace args = boost::phoenix::arg_names;
    iter->second.connection = fpsGenerator.Connect( fps, boost::phoenix::bind( &SceneManager::HandleRefreshTimer, this, args::arg1 ) );
  }
  return key;
}

void SceneManager::Delete( key_t key ) {
  mapSceneElement_t::const_iterator iter1 = m_mapSceneElement.find( key );
  if ( m_mapSceneElement.end() == iter1 ) {
    throw std::runtime_error( "key1 not found" );
  }
  else {
    FPS fps( iter1->second.fps );
    m_mapSceneElement.erase( iter1 );
    
    mapSceneElementsToRefreshAtFps_t::iterator iter2 = m_mapSceneElementsToRefreshAtFps.find( fps );
    if ( m_mapSceneElementsToRefreshAtFps.end() == iter2 ) {
      throw std::runtime_error( "key2 not found" );
    }
    else {
      mapSceneElementsToRefresh_t::iterator iter3 = iter2->second.mapSceneElementsToRefresh.find( key );
      if ( iter2->second.mapSceneElementsToRefresh.end() == iter3 ) {
        throw std::runtime_error( "key3 not found" );
      }
      else {
        iter2->second.mapSceneElementsToRefresh.erase( iter3 );
        if ( 0 == iter2->second.mapSceneElementsToRefresh.size() ) {
          iter2->second.connection.disconnect();
        }
      }
    }
  }
}

void SceneManager::OnPaintInit( void ) {
  
  //std::cout << "init start" << std::endl;
  
  // stuff goes here
  
  //std::cout << "init end" << std::endl;
}

void SceneManager::OnPaint( void ) {
  
  //std::cout << "paint start" << std::endl;
  
  // stuff goes here
  
  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  if ( !m_vSceneElementToInit.empty() ) {
    std::cout << "init start" << std::endl;
    BOOST_FOREACH( key_t key, m_vSceneElementToInit ) {
      mapSceneElement_t::iterator iter = m_mapSceneElement.find( key );
      assert( m_mapSceneElement.end() != iter );
      glDebugMessageCallback( &callbackSceneManager, (const void*) iter->first );
      iter->second.pSceneElement->Init();
      glDebugMessageCallback( 0, 0 );
    }
    m_vSceneElementToInit.clear();
    std::cout << "init end" << std::endl;
  }  
  
  // need to paint using mapSceneElementsToRefreshAtFps_t instead
  BOOST_FOREACH( mapSceneElement_t::value_type element, m_mapSceneElement ) { 
    glDebugMessageCallback( &callbackSceneManager, (const void*) element.first );
    //std::cout << "painting " << element.first << std::endl;
    element.second.pSceneElement->Paint();
    //std::cout << "done painting" << std::endl;
    glDebugMessageCallback( 0, 0 );
  }

  //std::cout << "paint end" << std::endl;
}

void SceneManager::OnResize( int w, int h ) {
  //std::cout << "resize " << w << ", " << h << std::endl;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void SceneManager::HandleRefreshTimer( FpsGenerator::FPS fps ) {
  if ( m_bActive ) { // cross thread action
    wxApp::GetInstance()->QueueEvent( new EventGenerateFrame( EVENT_GENERATEFRAME, this->GetId(), fps ) );
  }
}

void SceneManager::HandleRefresh( EventGenerateFrame& event ) {
  if ( this->GetId() == event.GetId() ) {
    // need to perform a call back to the SceneElement 
    FPS fps = event.GetFps();
    mapSceneElementsToRefreshAtFps_t::iterator iter = m_mapSceneElementsToRefreshAtFps.find( fps );
    assert( m_mapSceneElementsToRefreshAtFps.end() != iter );
    BOOST_FOREACH( mapSceneElementsToRefresh_t::value_type element, iter->second.mapSceneElementsToRefresh ) {
      element.second->OnFrameTrigger();
    }
    this->Refresh();  // need to step through the checks
  }
  else {
    //std::cout << "not our event" << std::endl;  // this does get hit, so the if above is appropriate
  }
  event.Skip( true );  // let other ones process this as well
}


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

FpsGenerator fps;  // generate signals for frame rate control

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
  
  namespace args = boost::phoenix::arg_names;
  //m_slotTimer = fps.Connect( FpsGenerator::fps24, boost::phoenix::bind( &SceneManager::HandleRefreshTimer, this, args::arg1 ) );
  m_vSlotTimer.push_back( fps.Connect( FpsGenerator::fps24, boost::phoenix::bind( &SceneManager::HandleRefreshTimer, this, args::arg1 ) ) );
 
  m_bActive = true;
  
}

SceneManager::~SceneManager( ) {
  BOOST_FOREACH( boost::signals2::connection& connection, m_vSlotTimer ) {
    connection.disconnect();
  }
  //m_slotTimer.disconnect();
  wxApp::GetInstance()->Unbind( EVENT_GENERATEFRAME, &SceneManager::HandleRefresh, this );
  m_bActive = false;
}

SceneManager::key_t SceneManager::Add( FPS fps, pSceneElement_t pSceneElement ) {
  key_t key( ++m_cntMapSceneElement ); 
  m_mapSceneElement.insert( mapSceneElement_t::value_type( key, SceneDescription_t( fps, pSceneElement ) ) );
  m_vSceneElementToInit.push_back( pSceneElement );
  mapSceneElementsToRefreshAtFps_t::iterator iter = m_mapSceneElementsToRefreshAtFps.find( fps );
  if ( m_mapSceneElementsToRefreshAtFps.end() == iter ) {
    std::pair<mapSceneElementsToRefreshAtFps_t::iterator, bool> result1;
    mapSceneElementsToRefreshAtFps_t::value_type vt( fps, mapSceneElementsToRefresh_t() );
    result1 = m_mapSceneElementsToRefreshAtFps.insert( vt );
    assert( true == result1.second );
    std::pair<mapSceneElementsToRefresh_t::iterator, bool> result2;
    mapSceneElementsToRefresh_t::value_type entry( key, pSceneElement );
    result2 = result1.first->second.insert( entry );
    assert( true == result2.second );
  }
  else {
    std::pair<mapSceneElementsToRefresh_t::iterator, bool> result2;
    mapSceneElementsToRefresh_t::value_type entry( key, pSceneElement );
    result2 = iter->second.insert( entry );
    assert( true == result2.second );
  }
  return key;
}

void SceneManager::Delete( key_t key ) {
  mapSceneElement_t::const_iterator iter = m_mapSceneElement.find( key );
  if ( m_mapSceneElement.end() == iter ) {
    throw std::runtime_error( "key1 not found" );
  }
  else {
    FPS fps( iter->second.fps );
    m_mapSceneElement.erase( iter );
    mapSceneElementsToRefreshAtFps_t::iterator iter2 = m_mapSceneElementsToRefreshAtFps.find( fps );
    if ( m_mapSceneElementsToRefreshAtFps.end() == iter2 ) {
      throw std::runtime_error( "key2 not found" );
    }
    else {
      mapSceneElementsToRefresh_t::iterator iter3 = iter2->second.find( key );
      if ( iter2->second.end() == iter3 ) {
        throw std::runtime_error( "key3 not found" );
      }
      else {
        iter2->second.erase( iter3 );
      }
    }
  }
}

void SceneManager::OnPaintInit( void ) {
  
  std::cout << "init start" << std::endl;
  
  // stuff goes here
  BOOST_FOREACH( mapSceneElement_t::value_type element, m_mapSceneElement ) {
    glDebugMessageCallback( &callbackSceneManager, (const void*) element.first );
    element.second.pSceneElement->Init();
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
  
  if ( !m_vSceneElementToInit.empty() ) {
    BOOST_FOREACH( pSceneElement_t p, m_vSceneElementToInit ) {
      p->Init();
    }
    m_vSceneElementToInit.clear();
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
    //wxApp::GetInstance()->QueueEvent( new EventGenerateFrame( EVENT_GENERATEFRAME, m_pPhysicalDisplay->GetFrame()->GetId() ) );
    wxApp::GetInstance()->QueueEvent( new EventGenerateFrame( EVENT_GENERATEFRAME, this->GetId(), fps ) );
  }
}

void SceneManager::HandleRefresh( EventGenerateFrame& event ) {
  //if ( m_pPhysicalDisplay->GetFrame()->GetId() == event.GetId() ) {
  if ( this->GetId() == event.GetId() ) {
    this->Refresh();  // need to step through the checks
    //m_pSceneManager->Refresh(); // this isn't the right way, as it will get called to many times
    //  when registering, registers with a specific fps queue
    // this should set a flag, so SceneManager draws everything, or does an auto refresh.
  }
  else {
    //std::cout << "not our event" << std::endl;  // this does get hit, so the if above is appropriate
  }
  event.Skip( true );  // let other ones process this as well
}


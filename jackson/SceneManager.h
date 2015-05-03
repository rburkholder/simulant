/* 
 * File:   SceneManager.h
 * Author: rpb
 *
 * Created on April 25, 2015, 10:28 AM
 */

#pragma once

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "EventGenerateFrame.h"

#include "canvasOpenGL.h"

class SceneElement;

class SceneManager: public canvasOpenGL<SceneManager> {
  friend class canvasOpenGL<SceneManager>;
public:
  
  typedef size_t key_t;
  typedef FpsGenerator::FPS FPS;
  typedef boost::shared_ptr<SceneElement> pSceneElement_t;
  
  SceneManager( wxFrame* parent, int* args );
  virtual ~SceneManager( );
  
  key_t Add( FPS, pSceneElement_t );  // returns key element
  void Delete( size_t key );
  
protected:
private:
  
  struct SceneDescription_t {
    FPS fps;
    pSceneElement_t pSceneElement;
    SceneDescription_t( FPS fps_, pSceneElement_t pSceneElement_ )
      : fps( fps_ ), pSceneElement( pSceneElement_ ) {}
  };
  
  typedef std::map<key_t,pSceneElement_t> mapSceneElementsToRefresh_t;
  
  struct FpsRelatedRefresh_t {
    boost::signals2::connection connection;
    mapSceneElementsToRefresh_t mapSceneElementsToRefresh;
    ~FpsRelatedRefresh_t( void ) {
      connection.disconnect();
    }
  };
  
  typedef std::map<key_t,SceneDescription_t> mapSceneElement_t;  
  typedef std::vector<key_t> vSceneElementKey_t;
  
  typedef std::map<FPS,FpsRelatedRefresh_t> mapSceneElementsToRefreshAtFps_t; 
  
  volatile bool m_bActive;
  
  GLuint m_idVertexArray;
  
  size_t m_cntMapSceneElement;
  
  mapSceneElement_t m_mapSceneElement;
  vSceneElementKey_t m_vSceneElementToInit;
  
  mapSceneElementsToRefreshAtFps_t m_mapSceneElementsToRefreshAtFps;
  
  void OnPaintInit( void );
  void OnPaint( void );
  void OnResize( int w, int h );

  void HandleRefreshTimer( FpsGenerator::FPS fps );  // is in work thread
  void HandleRefresh( EventGenerateFrame& event );
  
};


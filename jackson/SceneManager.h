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

#include "CanvasOpenGL.h"

class SceneElementOpenGL;

class SceneManager: public CanvasOpenGL<SceneManager> {
  friend class CanvasOpenGL<SceneManager>;
public:
  
  typedef size_t key_t;
  typedef FpsGenerator::FPS FPS;
  typedef boost::shared_ptr<SceneElementOpenGL> pSceneElementOpenGL_t;
  
  SceneManager( wxFrame* parent, int* args );
  virtual ~SceneManager( );
  
  key_t Add( FPS, pSceneElementOpenGL_t );  // returns key element
  key_t Add( size_t num, size_t den, pSceneElementOpenGL_t );  // returns key element
  void Delete( size_t key );
  
protected:
private:
  
  struct SceneDescription_t {
    FPS fps;
    pSceneElementOpenGL_t pSceneElementOpenGL;
    SceneDescription_t( FPS fps_, pSceneElementOpenGL_t pSceneElementOpenGL_ )
      : fps( fps_ ), pSceneElementOpenGL( pSceneElementOpenGL_ ) {}
  };
  
  typedef std::map<key_t,pSceneElementOpenGL_t> mapSceneElementsToRefresh_t;
  
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


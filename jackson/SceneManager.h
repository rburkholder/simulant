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

#include "canvasOpenGL.h"

class SceneElement;

class SceneManager: public canvasOpenGL<SceneManager> {
  friend class canvasOpenGL<SceneManager>;
public:
  
  typedef size_t key_t;
  typedef boost::shared_ptr<SceneElement> pSceneElement_t;
  
  SceneManager( wxFrame* parent, int* args );
  virtual ~SceneManager( );
  
  key_t Add( pSceneElement_t );  // returns key element
  void Delete( size_t key );
  
protected:
private:
  
  typedef std::map<key_t,pSceneElement_t> mapSceneElement_t;  
  typedef std::vector<pSceneElement_t> vSceneElement_t;
  
  GLuint m_idVertexArray;
  
  size_t m_cntMapSceneElement;
  
  mapSceneElement_t m_mapSceneElement;
  vSceneElement_t m_vSceneElementToInit;
  
  void OnPaintInit( void );
  void OnPaint( void );
  void OnResize( int w, int h );

};


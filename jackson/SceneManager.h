/* 
 * File:   SceneManager.h
 * Author: rpb
 *
 * Created on April 25, 2015, 10:28 AM
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

#include "canvasOpenGL.h"

class SceneElement;

class SceneManager: public canvasOpenGL<SceneManager> {
  friend class canvasOpenGL<SceneManager>;
public:
  
  typedef boost::shared_ptr<SceneElement> pSceneElement_t;
  
  SceneManager( wxFrame* parent, int* args );
  virtual ~SceneManager( );
  
  void Add( pSceneElement_t );
  
protected:
private:
  
  typedef std::vector<pSceneElement_t> vSceneElement_t;
  
  GLuint m_idVertexArray;
  
  vSceneElement_t m_vSceneElement;
  
  void OnPaintInit( void );
  void OnPaint( void );
  void OnResize( int w, int h );

};


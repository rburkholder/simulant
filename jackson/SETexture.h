/* 
 * File:   SETexture.h
 * Author: rpb
 *
 * Created on April 25, 2015, 9:16 PM
 */

#pragma once

//SceneElement Texture

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "RawImage.h"
#include "SceneElementOpenGL.h"

#include <glm/glm.hpp>

class SETexture: public SceneElementOpenGL {
public:
  
  typedef boost::signals2::signal<void()> signalFrame_t;
  typedef signalFrame_t::slot_type slotFrame_t;
  typedef boost::shared_ptr<wxImage> pImage_t;
  typedef RawImage::pRawImage_t pRawImage_t;
  
  SETexture( );
  virtual ~SETexture( );
  
  virtual void Init( void );
  virtual void Paint( void );
  
  void SetWindowCoords( std::vector<glm::vec4>&  vCoords );  // to be deprecated
  
  void SetTransform( const glm::mat4& mat4Transform );
  
  void SetImage( pImage_t pImage );
  void SetImage( pRawImage_t pRawImage );
  
  virtual void OnFrameTrigger( void ) { m_signal(); };
  boost::signals2::connection Connect( const slotFrame_t& );
  
protected:
  
private:
  
  GLuint m_idVertexArray;
  GLuint m_idTexture;
  GLuint m_idElements;
  GLuint m_idVertexBufferForImageCoords;
  GLuint m_idVertexBufferForTextureCoords;
  
  GLuint m_idVapImageCoords;
  GLuint m_idVapTextureCoords;
  
  GLuint m_idUniformTransform;
  
  pImage_t m_pImage;
  bool m_bNewImageAvailable;
  
  pRawImage_t m_pRawImage;
  bool m_bNewRawImageAvailable;
  
  signalFrame_t m_signal;
  
  glm::mat4 m_mat4BasicTransform;
  glm::mat4 m_mat4SuppliedTransform;
  glm::mat4 m_mat4FinalTransform;
  
  std::vector<glm::vec2> m_vtxImageCoords;
  std::vector<glm::vec2> m_vtxTextureCoords;
  
  std::vector<GLuint> m_vElements;
  
  void SetImage( void );
  
  void SetImageCoords( void );
  
  void AddTexture( void );
  void AssignImageToTexture( void );
  void AssignRawImageToTexture( void );
  
  void SetBasicTransform( void );
  
};


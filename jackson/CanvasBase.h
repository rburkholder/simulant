/* 
 * File:   CanvasBase.h
 * Author: rpb
 *
 * Created on April 4, 2015, 10:55 PM
 */

#ifndef CANVASBASE_H
#define	CANVASBASE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>

#include <wx/glcanvas.h>

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>

class CanvasBase: public wxGLCanvas {   
  // so we can keep in regular vector, otherwise will need a fusion vector
  // basically the opengl viewport via the projector
  // rough rectangular area of an opengl canvas surface in the VisualRepresentation
  // in screen coords now, but should be relative coords so can use any screen area
  // and thus auto adjusts resolution based upon screen area available
  // ie, dual use:  the real projection, preview projections on controller
public:
  
  typedef boost::shared_ptr<CanvasBase> pCanvas_t;
  
//  CanvasBase( void ) 
  CanvasBase(  wxFrame* parent, int* args  )
  : wxGLCanvas( parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE )
  {
//    vtxCoordsScreenFrame[ 0 ] = glm::vec2( 0.0f, 0.0f ); // default to full ScreenFrame
//    vtxCoordsScreenFrame[ 1 ] = glm::vec2( 0.0f, 1.0f );
//    vtxCoordsScreenFrame[ 2 ] = glm::vec2( 1.0f, 1.0f );
//    vtxCoordsScreenFrame[ 3 ] = glm::vec2( 1.0f, 0.0f );
  };
  virtual ~CanvasBase( void ) {};
  
  void SetName( const std::string& sName ) { m_sName = sName; }
  const std::string GetName( void ) { return m_sName; }
  
protected:
private:
  std::string m_sName;
//  glm::vec2 vtxCoordsScreenFrame[ 4 ]; // will be remapped/scaled/translated to suitable screen coords
};

#endif	/* CANVASBASE_H */


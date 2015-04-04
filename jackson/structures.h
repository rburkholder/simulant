/* 
 * File:   structures.h
 * Author: rpb
 *
 * Created on April 3, 2015, 12:01 PM
 */

#ifndef STRUCTURES_H
#define	STRUCTURES_H

// 2015/04/03
//   need to scale everything two different ways:
//     local machine liveview mini windows
//     remote machine actual projection

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/display.h>

#include <glm/glm.hpp>

#include "frameProjection.h"

class TextureBase {
  // maybe one or more textures per model
  // start with a flat movie screen model 
public:
  TextureBase( void ) {};
  TextureBase( const std::string& sDescription ): m_sDescription( sDescription ) {}
protected:
private:
  std::string m_sDescription;
};

class TextureMovieScreen: public TextureBase {
public:
  TextureMovieScreen( void ) {  
    vtxCoords[ 0 ] = glm::vec3( 0.0f, 0.0f, 0.0f );  // default to full Canvas
    vtxCoords[ 1 ] = glm::vec3( 0.0f, 1.0f, 0.0f );
    vtxCoords[ 2 ] = glm::vec3( 1.0f, 1.0f, 0.0f );
    vtxCoords[ 3 ] = glm::vec3( 1.0f, 0.0f, 0.0f );
  }
protected:
private:
  glm::vec3 vtxCoords[ 4 ]; // can be all or part of the supplied texture, whether picture or movie, one frame at a time
};

class ModelBase {
public:
protected:
private:
  std::string m_sDescription;
};

class ModelMovieScreen: public ModelBase {
  // this is where most of the activity for this project happens
  // a four vertex polygon for showing pictures or movies
  // will be perspective and projection modified to fit with in rectangular surfaces hung on walls
  // the four vertices are manipulated independently within the Canvas to fill the hung rectangular surfaces
public:
  ModelMovieScreen( void ) {  
    vtxCoords[ 0 ] = glm::vec3( 0.0f, 0.0f, 0.0f );  // default to full Canvas
    vtxCoords[ 1 ] = glm::vec3( 0.0f, 1.0f, 0.0f );
    vtxCoords[ 2 ] = glm::vec3( 1.0f, 1.0f, 0.0f );
    vtxCoords[ 3 ] = glm::vec3( 1.0f, 0.0f, 0.0f );
  }
protected:
private:
  glm::vec3 vtxCoords[ 4 ]; // will be transformed to match projection surface on the wall
};

class Canvas {  // basically the opengl viewport via the projector
  // rough rectangular area of an opengl canvas surface in the VisualRepresentation
  // in screen coords now, but should be relative coords so can use any screen area
  // and thus auto adjusts resolution based upon screen area available
  // ie, dual use:  the real projection, preview projections on controller
public:
  Canvas( void ) {
    vtxCoords[ 0 ] = glm::vec2( 0.0f, 0.0f ); // default to full ScreenFrame
    vtxCoords[ 1 ] = glm::vec2( 0.0f, 1.0f );
    vtxCoords[ 2 ] = glm::vec2( 1.0f, 1.0f );
    vtxCoords[ 3 ] = glm::vec2( 1.0f, 0.0f );
  }
protected:
private:
  std::string m_sDescription;
  glm::vec2 vtxCoords[ 4 ]; // will be remapped/scaled/translated to suitable screen coords
};

class ScreenFrame {  // maybe subset of multi-monitor setup, or an individual remote monitor
  // maps to the pixels emitted by a single projector
  // remote controller may be used to populate the resolution here
  // will need to add remote machine information here to represent remote displays
public:
  ScreenFrame( unsigned int nDisplay, wxWindow* parent ): 
      ScreenFrame( nDisplay, parent, wxPoint( 0, 0 ), wxSize( 100, 200 ) ) {
  }
  ScreenFrame( unsigned int nDisplay, wxWindow* parent, wxPoint point_, wxSize size_ ): 
    display( nDisplay ), point( point_ ), size( size_ ) 
  {
    // for debugging purposes
    wxVideoMode vmDisplay = display.GetCurrentMode();
    wxRect rectGeometry = display.GetGeometry();
    wxArrayVideoModes modes = display.GetModes();
    wxString sName = display.GetName();
    bool bPrimary = display.IsPrimary();
    
    // do the frame for projection display
    pFrame = new FrameProjection( parent, -1, "", wxPoint( 10, 10 ), wxSize( 10, 10 ) );  // use default point and size to for redraw to bypass bug
    pFrame->SetSize( size );
    pFrame->Move( point );
    pFrame->Show();
  }
  ScreenFrame( void ): display( 0 ), point( 0, 0 ), size( 100, 200 ) {
  }
  FrameProjection* GetFrame( void ) { return pFrame; }
protected:
private:
  
  typedef std::vector<Canvas> vCanvas_t;
  
  std::string m_sDescription;
  wxDisplay display;  // non copyable
  wxPoint point;     // location in client area, bug work around in gtk3, chooses different location on primary display
  wxSize size;  // use default, but may override
  FrameProjection* pFrame;  // windowing system takes care of destruction
  vCanvas_t vCanvas;  
};

typedef boost::shared_ptr<ScreenFrame> pScreenFrame_t;
typedef std::vector<pScreenFrame_t> vScreenFrame_t;



#endif	/* STRUCTURES_H */

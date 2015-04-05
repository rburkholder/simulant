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

#include <glm/glm.hpp>

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



#endif	/* STRUCTURES_H */

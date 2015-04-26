/* 
 * File:   AspectRatio.cpp
 * Author: rpb
 * 
 * Created on April 26, 2015, 5:02 PM
 */

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "AspectRatio.h"

glm::mat4 AspectRatioWindow( float height, float width ) {
  
  float floatHeightScale( 1.0f );
  float floatWidthScale( 1.0f );
  
  float ar = height / width;  // aspect ratio
  
  if ( 1.0 != ar ) {  // rescale if not a square
    if ( 1.0 < ar ) { // height > width, so scale to width
      floatHeightScale = 1.0f / ar;
    }
    else { // width > height, so scale to height
      floatWidthScale = 1.0f * ar;
    }
  }
  
  glm::mat4 mat4Transform = glm::mat4( 1.0f ) * glm::scale( glm::vec3( floatWidthScale, floatHeightScale, 0.0f ) );
  
  std::cout << "ar(win): " << floatHeightScale << ", " << floatWidthScale << std::endl;
  
  return mat4Transform;
}

glm::mat4 AspectRatioImage( float height, float width ) {
  
  float floatHeightScale( 1.0f );
  float floatWidthScale( 1.0f );
  
  float ar = width / height;  // aspect ratio
  
  if ( 1.0 != ar ) {  // rescale if not a square
    if ( 1.0 < ar ) { // height > width, so scale to width
      floatHeightScale = 1.0f / ar;
    }
    else { // width > height, so scale to height
      floatWidthScale = 1.0f * ar;
    }
  }
  
  glm::mat4 mat4Transform = glm::mat4( 1.0f ) * glm::scale( glm::vec3( floatWidthScale, floatHeightScale, 0.0f ) );
  
  std::cout << "ar(img): " << floatHeightScale << ", " << floatWidthScale << std::endl;
  
  return mat4Transform;
}




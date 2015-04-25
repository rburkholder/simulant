/* 
 * File:   SceneElement.h
 * Author: rpb
 *
 * Created on April 25, 2015, 10:50 AM
 * 
 * Fully fleshed scene elements inherit from this, and are then used in the SceneManager
 * 
 */

#pragma once

class SceneElement {
public:
  
  SceneElement( );
  virtual ~SceneElement( );
  
  virtual void Init( void ) {};
  virtual void Paint( void ) {};
  
protected:
  
private:

};


/* 
 * File:   tex2.h
 * Author: rpb
 *
 * Created on March 29, 2015, 8:33 PM
 */

#ifndef TEX2_H
#define	TEX2_H

#include <vector>

#include "canvasOpenGL.h"

class tex2: public canvasOpenGL<tex2> {
public:
  
  tex2( wxFrame* parent, int* args );
  virtual ~tex2();
  
  void SetWindowCoords( std::vector<glm::vec4>&  vCoords );
  
  void SetImage( wxImage* pImage ) {
    assert( 0 != pImage );
    m_pImage = pImage;
  }
  
  void OnPaintInit() {
    init();
  }
  void OnPaint() {
    display();
  }
  void OnResize( int w, int h ) {
    reshape( w, h );
  }
protected:
private:
  
  GLuint m_program;
  GLuint m_texture;
  
  std::vector<glm::vec2> m_vtxWindowCoords;
  
  wxImage* m_pImage;  // should be shared_ptr so can be overwritten and self delete

  void display();
  void init();
  void reshape (int w, int h);

};

#endif	/* TEX2_H */


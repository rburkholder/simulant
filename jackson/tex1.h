/* 
 * File:   tex1.h
 * Author: rpb
 *
 * Created on March 28, 2015, 8:33 PM
 */

#ifndef TEX1_H
#define	TEX1_H

#include "canvasOpenGL.h"

class tex1: public canvasOpenGL<tex1> {
public:
  
  tex1( wxFrame* parent, int* args ): canvasOpenGL<tex1>( parent, args ), m_pImage( 0 ) {};
  virtual ~tex1() {};
  
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
  
  wxImage* m_pImage;  // should be shared_ptr so can be overwritten and self delete

  void display();
  void init();
  void reshape (int w, int h);

};

#endif	/* TEX1_H */


/* 
 * File:   tex2.h
 * Author: rpb
 *
 * Created on March 29, 2015, 8:33 PM
 */

#ifndef TEX2_H
#define	TEX2_H

#include <vector>

#include "CanvasOpenGL.h"

#include "SETexture.h"

class tex2: public canvasOpenGL<tex2> {
public:
  
  tex2( wxFrame* parent, int* args );
  virtual ~tex2();
  
  void SetWindowCoords( std::vector<glm::vec4>&  vCoords ) { m_seTexture.SetWindowCoords( vCoords ); }
  
  void SetImage( SETexture::pImage_t pImage ) { m_seTexture.SetImage( pImage ); }
  
  void OnPaintInit();
  void OnPaint();
  void OnResize( int w, int h );
  
protected:
private:
  
  SETexture m_seTexture;

};

#endif	/* TEX2_H */

